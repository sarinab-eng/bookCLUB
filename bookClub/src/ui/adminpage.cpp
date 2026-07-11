#include "adminpage.h"
#include "ui_adminpage.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>

AdminPage::AdminPage(AuthManager *authManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminPage),
    m_authManager(authManager)
{
    ui->setupUi(this);
    setupTableHeaders();

    connect(ui->refreshButton, &QPushButton::clicked, this, &AdminPage::onRefreshButtonClicked);
    connect(ui->btnBlockUser, &QPushButton::clicked, this, &AdminPage::onBlockClicked);
    connect(ui->btnDeleteUser, &QPushButton::clicked, this, &AdminPage::onDeleteClicked);

    connect(ui->usersTable, &QTableWidget::cellClicked, this, &AdminPage::onUserSelected);

    connect(ui->txtSearch, &QLineEdit::textChanged, this, &AdminPage::filterUsers);
    connect(ui->cmbRoleFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminPage::filterUsers);

    if (m_authManager) {
        connect(m_authManager, &AuthManager::usersListReceived, this, &AdminPage::handleUsersList);
        connect(m_authManager, &AuthManager::actionFinished, this, &AdminPage::handleActionResponse);
    }
    loadData();
}

AdminPage::~AdminPage() {
    delete ui;
}

void AdminPage::setupTableHeaders() {
    ui->usersTable->setColumnCount(2);
    ui->usersTable->setHorizontalHeaderLabels({"نام کاربری", "نقش کاربری"});
    ui->usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->usersTable->verticalHeader()->setVisible(false);
    ui->usersTable->setShowGrid(false);
    ui->usersTable->setAlternatingRowColors(true);
    ui->usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

void AdminPage::loadData() {
    if (m_authManager) m_authManager->requestUsersList();
}

void AdminPage::onRefreshButtonClicked() {
    qDebug() << "Refresh button clicked!";
    loadData();
}

void AdminPage::handleUsersList(const QJsonArray &users) {
    qDebug() << "Received users count:" << users.size();
    m_allUsers = users;
    m_filteredUsers = users;
    //filterUsers();
    updateTable(m_allUsers);
    resetDetailLabels();
}

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
    resetDetailLabels();
}

void AdminPage::updateTable(const QJsonArray &usersToShow) {
    ui->usersTable->setRowCount(0);
    for (int i = 0; i < usersToShow.size(); ++i) {
        QJsonObject userObj = usersToShow[i].toObject();
        ui->usersTable->insertRow(i);

        QString role = userObj["role"].toString().toLower();
        QString displayRole;
        if (role == "admin") displayRole = "مدیر سیستم";
        else if (role == "publisher") displayRole = "ناشر";
        else displayRole = "کاربر عادی";
        ui->usersTable->setItem(i, 0, new QTableWidgetItem(userObj["username"].toString()));
        ui->usersTable->setItem(i, 1, new QTableWidgetItem(displayRole));
    }
}

void AdminPage::onUserSelected(int row, int column) {
    Q_UNUSED(column);
    if (row < 0 || row >= m_filteredUsers.size()) return;
    QJsonObject userObj = m_filteredUsers[row].toObject();
    m_selectedUsername = userObj["username"].toString();

    ui->lblDetailUsername->setText("نام کاربری: " + m_selectedUsername);
    ui->lblDetailRegDate->setText("تاریخ عضویت: " + userObj["registration_date"].toString("ثبت نشده"));

    QString role = userObj["role"].toString().toLower();
    QString roleDisplay = "نقش: ";
    if (role == "admin") roleDisplay += "مدیر سیستم";
    else if (role == "publisher") roleDisplay += "ناشر";
    else if (role == "customer" || role == "user") roleDisplay += "کاربر عادی";
    else roleDisplay += "نامشخص";

    ui->lblDetailRole->setText(roleDisplay); // نام لیبل نقش را در UI چک کنید (احتمالا lblDetailRole است)

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
        loadData();
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