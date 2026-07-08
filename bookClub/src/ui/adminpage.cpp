#include "adminpage.h"
#include "ui_adminpage.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>

AdminPage::AdminPage(AuthManager *authManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminPage),
    m_authManager(authManager)
{
    ui->setupUi(this);
    setupTableHeaders();

    // اتصالات دکمه‌ها
    connect(ui->refreshButton, &QPushButton::clicked, this, &AdminPage::onRefreshButtonClicked);
    connect(ui->btnBlockUser, &QPushButton::clicked, this, &AdminPage::onBlockClicked);
    connect(ui->btnDeleteUser, &QPushButton::clicked, this, &AdminPage::onDeleteClicked);

    // اتصال کلیک روی جدول
    connect(ui->usersTable, &QTableWidget::cellClicked, this, &AdminPage::onUserSelected);

    // --- فعال‌سازی فیلترهای بالا ---
    connect(ui->txtSearch, &QLineEdit::textChanged, this, &AdminPage::filterUsers);
    connect(ui->cmbRoleFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminPage::filterUsers);

    // اتصال به AuthManager
    if (m_authManager) {
        connect(m_authManager, &AuthManager::usersListReceived, this, &AdminPage::handleUsersList);
        connect(m_authManager, &AuthManager::actionFinished, this, &AdminPage::handleActionResponse);
    }
}

AdminPage::~AdminPage() {
    delete ui;
}

void AdminPage::setupTableHeaders() {
    ui->usersTable->setColumnCount(2);
    ui->usersTable->setHorizontalHeaderLabels({"نام کاربری", "نقش کاربری"});
    ui->usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

void AdminPage::loadData() {
    if (m_authManager) m_authManager->requestUsersList();
}

void AdminPage::onRefreshButtonClicked() {
    loadData();
}

// ۱. دریافت لیست از سرور و ذخیره در لیست اصلی
void AdminPage::handleUsersList(const QJsonArray &users) {
    m_allUsers = users;  // لیست اصلی را آپدیت کن
    filterUsers();       // بلافاصله فیلتر را اعمال کن تا جدول پر شود
}

// ۲. منطق فیلتر کردن داده‌ها
void AdminPage::filterUsers() {
    QString searchText = ui->txtSearch->text().trimmed().toLower();
    int roleIndex = ui->cmbRoleFilter->currentIndex();

    QJsonArray tempArray;

    for (int i = 0; i < m_allUsers.size(); ++i) {
        QJsonObject userObj = m_allUsers[i].toObject();
        QString username = userObj["username"].toString().toLower();
        QString role = userObj["role"].toString().toLower();

        bool matchesSearch = searchText.isEmpty() || username.contains(searchText);

        bool matchesRole = false;
        if (roleIndex == 0) matchesRole = true; // همه
        else if (roleIndex == 1 && role == "admin") matchesRole = true;
        else if (roleIndex == 2 && role == "publisher") matchesRole = true;
        else if (roleIndex == 3 && (role == "customer" || role == "user")) matchesRole = true;

        if (matchesSearch && matchesRole) {
            tempArray.append(userObj);
        }
    }

    m_filteredUsers = tempArray;
    updateTable(m_filteredUsers);
    resetDetailLabels(); // بعد از هر فیلتر، جزئیات سمت راست را پاک کن
}

// ۳. نمایش داده‌ها در جدول
void AdminPage::updateTable(const QJsonArray &usersToShow) {
    ui->usersTable->setRowCount(0);
    for (int i = 0; i < usersToShow.size(); ++i) {
        QJsonObject userObj = usersToShow[i].toObject();
        ui->usersTable->insertRow(i);

        QString role = userObj["role"].toString();
        QString displayRole = (role == "admin" ? "مدیر سیستم" : (role == "publisher" ? "ناشر" : "کاربر عادی"));

        ui->usersTable->setItem(i, 0, new QTableWidgetItem(userObj["username"].toString()));
        ui->usersTable->setItem(i, 1, new QTableWidgetItem(displayRole));
    }
}

// ۴. وقتی کاربری از جدول انتخاب می‌شود
void AdminPage::onUserSelected(int row, int column) {
    Q_UNUSED(column);
    if (row < 0 || row >= m_filteredUsers.size()) return;
    QJsonObject userObj = m_filteredUsers[row].toObject();
    m_selectedUsername = userObj["username"].toString();

    // پر کردن لیبل‌ها
    ui->lblDetailUsername->setText("نام کاربری: " + m_selectedUsername);
    ui->lblDetailRegDate->setText("تاریخ عضویت: " + userObj["registration_date"].toString("ثبت نشده"));

    QString status = userObj["status"].toString("active");
    if (status == "blocked") {
        ui->lblDetailStatus->setText("وضعیت: مسدود شده");
        ui->lblDetailStatus->setStyleSheet("color: red;");
        ui->btnBlockUser->setText("رفع مسدودیت");
    } else {
        ui->lblDetailStatus->setText("وضعیت: فعال");
        ui->lblDetailStatus->setStyleSheet("color: green;");
        ui->btnBlockUser->setText("مسدود کردن");
    }

    // نمایش اطلاعات اختصاصی بر اساس نقش
    QString role = userObj["role"].toString();
    if (role == "publisher") {
        ui->lblDetailSpecific->setText("نام انتشارات: " + userObj["publisher_name"].toString("---"));
    } else {
        ui->lblDetailSpecific->setText("اطلاعات اختصاصی: ندارد");
    }
}

void AdminPage::onBlockClicked() {
    if (m_selectedUsername.isEmpty()) return;
    m_authManager->adminAction("block_user", m_selectedUsername);
}

void AdminPage::onDeleteClicked() {
    if (m_selectedUsername.isEmpty()) return;
    if (QMessageBox::question(this, "تایید", "حذف کاربر؟") == QMessageBox::Yes) {
        m_authManager->adminAction("delete_user", m_selectedUsername);
    }
}

void AdminPage::handleActionResponse(bool success, const QString &message) {
    if (success) {
        loadData(); // لیست را دوباره بگیر تا تغییرات اعمال شود
    }
    QMessageBox::information(this, "نتیجه", message);
}

void AdminPage::resetDetailLabels() {
    m_selectedUsername = "";
    ui->lblDetailUsername->setText("نام کاربری: -");
    ui->lblDetailStatus->setText("وضعیت: -");
    ui->lblDetailStatus->setStyleSheet("");
    ui->btnBlockUser->setText("مسدود کردن");
}