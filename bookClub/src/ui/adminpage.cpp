#include "adminpage.h"
#include "ui_adminpage.h"
#include "addeditbookdialog.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

AdminPage::AdminPage(AuthManager *authManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminPage),
    m_authManager(authManager)
{
    ui->setupUi(this);
    setupTableHeaders();
    buildContentTabs();

    connect(ui->refreshButton, &QPushButton::clicked, this, &AdminPage::onRefreshButtonClicked);
    connect(ui->btnBlockUser, &QPushButton::clicked, this, &AdminPage::onBlockClicked);
    connect(ui->btnDeleteUser, &QPushButton::clicked, this, &AdminPage::onDeleteClicked);

    connect(ui->usersTable, &QTableWidget::cellClicked, this, &AdminPage::onUserSelected);

    connect(ui->txtSearch, &QLineEdit::textChanged, this, &AdminPage::filterUsers);
    connect(ui->cmbRoleFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AdminPage::filterUsers);

    if (m_authManager) {
        connect(m_authManager, &AuthManager::usersListReceived, this, &AdminPage::handleUsersList);
        connect(m_authManager, &AuthManager::actionFinished, this, &AdminPage::handleActionResponse);
        connect(m_authManager, &AuthManager::adminBooksReceived, this, &AdminPage::onAdminBooksReceived);
        connect(m_authManager, &AuthManager::adminBookUpdated, this, &AdminPage::onAdminBookUpdated);
        connect(m_authManager, &AuthManager::adminBookDeleted, this, &AdminPage::onAdminBookDeleted);
        connect(m_authManager, &AuthManager::adminReviewsReceived, this, &AdminPage::onAdminReviewsReceived);
        connect(m_authManager, &AuthManager::adminReviewDeleted, this, &AdminPage::onAdminReviewDeleted);
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
    if (!m_authManager) return;
    m_authManager->requestUsersList();
    m_authManager->requestAllBooksForAdmin();
    m_authManager->requestAllReviewsForAdmin();
}

// چون ویجت‌های ادمین با geometry مطلق (بدون layout مادر) ساخته شدن، اون‌ها رو
// عیناً با همون موقعیت داخل یه تبِ جدید «کاربران» می‌ذاریم و کنارش دو تب جدید
// «کتاب‌ها» و «نظرات» برای نظارت بر محتوا اضافه می‌کنیم
void AdminPage::buildContentTabs() {
    QWidget *usersTab = new QWidget;
    ui->detailsGroupBox->setParent(usersTab);
    ui->layoutWidget->setParent(usersTab);

    // ---- تب کتاب‌ها ----
    QWidget *booksTab = new QWidget;
    QVBoxLayout *booksLayout = new QVBoxLayout(booksTab);

    m_booksTable = new QTableWidget(0, 6);
    m_booksTable->setHorizontalHeaderLabels({"عنوان", "ناشر", "ژانر", "قیمت", "امتیاز", "وضعیت"});
    m_booksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_booksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_booksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    booksLayout->addWidget(m_booksTable);

    QHBoxLayout *booksButtonsLayout = new QHBoxLayout;
    m_editBookButton = new QPushButton("ویرایش کتاب");
    m_deleteBookButton = new QPushButton("حذف کتاب");
    booksButtonsLayout->addWidget(m_editBookButton);
    booksButtonsLayout->addWidget(m_deleteBookButton);
    booksLayout->addLayout(booksButtonsLayout);

    connect(m_editBookButton, &QPushButton::clicked, this, &AdminPage::onEditBookClicked);
    connect(m_deleteBookButton, &QPushButton::clicked, this, &AdminPage::onDeleteBookClicked);

    // ---- تب نظرات ----
    QWidget *reviewsTab = new QWidget;
    QVBoxLayout *reviewsLayout = new QVBoxLayout(reviewsTab);

    m_reviewsTable = new QTableWidget(0, 4);
    m_reviewsTable->setHorizontalHeaderLabels({"کاربر", "کتاب", "امتیاز", "متن نظر"});
    m_reviewsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_reviewsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_reviewsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    reviewsLayout->addWidget(m_reviewsTable);

    m_deleteReviewButton = new QPushButton("حذف نظر");
    reviewsLayout->addWidget(m_deleteReviewButton);
    connect(m_deleteReviewButton, &QPushButton::clicked, this, &AdminPage::onDeleteReviewClicked);

    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(usersTab, "کاربران");
    tabs->addTab(booksTab, "کتاب‌ها");
    tabs->addTab(reviewsTab, "نظرات");

    QHBoxLayout *headerLayout = new QHBoxLayout;
    QLabel *panelTitleLabel = new QLabel("پنل مدیر سیستم");
    panelTitleLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    QPushButton *logoutButton = new QPushButton("خروج");
    logoutButton->setStyleSheet(
        "QPushButton { background-color: #E25858; color: white; border-radius: 10px;"
        " padding: 8px 15px; font-weight: bold; }"
        "QPushButton:hover { background-color: #C0392B; }");
    headerLayout->addWidget(panelTitleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(logoutButton);
    connect(logoutButton, &QPushButton::clicked, this, &AdminPage::logoutRequested);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(tabs);
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

    if(role=="customer")
    {
        qDebug() << userObj;
        QJsonArray genres=userObj["favoriteGenres"].toArray();

        QStringList list;

        for(auto g:genres)
            list<<g.toString();

        ui->lblDetailSpecific->setText(
            "ژانرهای مورد علاقه: "+list.join(" ، ")
            );
    }

    else if (role == "publisher") {
        // فیلد publisher_name هیچ‌جای ثبت‌نام جمع‌آوری نمی‌شه؛ به‌جاش تعداد کتاب‌های
        // واقعاً منتشرشده‌ی این ناشر رو (که از پنل مدیریت کتاب‌ها در دسترسه) نشون می‌دیم
        int bookCount = 0;
        for (const QJsonValue &v : m_allBooks) {
            if (v.toObject().value("publisherUsername").toString() == m_selectedUsername)
                bookCount++;
        }
        ui->lblDetailSpecific->setText(QString("تعداد کتاب‌های منتشرشده: %1").arg(bookCount));
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

// ================= مدیریت کتاب‌ها و محتوا =================

void AdminPage::onAdminBooksReceived(const QJsonArray &books) {
    m_allBooks = books;
    m_booksTable->setRowCount(0);

    for (int i = 0; i < books.size(); ++i) {
        QJsonObject book = books[i].toObject();
        m_booksTable->insertRow(i);

        QTableWidgetItem *titleItem = new QTableWidgetItem(book["title"].toString());
        titleItem->setData(Qt::UserRole, book["id"].toString());
        m_booksTable->setItem(i, 0, titleItem);
        m_booksTable->setItem(i, 1, new QTableWidgetItem(book["publisher"].toString()));
        m_booksTable->setItem(i, 2, new QTableWidgetItem(book["genre"].toString()));
        m_booksTable->setItem(i, 3, new QTableWidgetItem(QString::number(book["price"].toDouble(), 'f', 0)));
        m_booksTable->setItem(i, 4, new QTableWidgetItem(QString::number(book.value("averageRating").toDouble(), 'f', 1)));

        bool active = book.value("isActive").toBool(true);
        m_booksTable->setItem(i, 5, new QTableWidgetItem(active ? "فعال" : "غیرفعال"));
    }
}

QJsonObject AdminPage::selectedBook() const {
    int row = m_booksTable->currentRow();
    if (row < 0) return QJsonObject();

    QString bookId = m_booksTable->item(row, 0)->data(Qt::UserRole).toString();
    for (const QJsonValue &v : m_allBooks) {
        if (v.toObject()["id"].toString() == bookId) return v.toObject();
    }
    return QJsonObject();
}

void AdminPage::onEditBookClicked() {
    QJsonObject book = selectedBook();
    if (book.isEmpty()) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک کتاب را از جدول انتخاب کنید.");
        return;
    }

    AddEditBookDialog dialog(book, this);
    if (dialog.exec() != QDialog::Accepted) return;

    m_authManager->adminUpdateBook(book["id"].toString(), dialog.bookData());
}

void AdminPage::onDeleteBookClicked() {
    QJsonObject book = selectedBook();
    if (book.isEmpty()) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک کتاب را از جدول انتخاب کنید.");
        return;
    }
    if (QMessageBox::question(this, "حذف کتاب",
            "این کتاب برای همیشه از سامانه حذف می‌شود (برخلاف غیرفعال‌سازی توسط ناشر، "
            "این عملیات قابل بازگشت نیست). ادامه می‌دهید؟") != QMessageBox::Yes)
        return;

    m_authManager->adminDeleteBook(book["id"].toString());
}

void AdminPage::onAdminBookUpdated(bool success, const QString &message) {
    QMessageBox::information(this, success ? "موفق" : "خطا", message);
    if (success) m_authManager->requestAllBooksForAdmin();
}

void AdminPage::onAdminBookDeleted(bool success, const QString &message) {
    QMessageBox::information(this, success ? "موفق" : "خطا", message);
    if (success) m_authManager->requestAllBooksForAdmin();
}

// ================= نظارت بر نظرات =================

void AdminPage::onAdminReviewsReceived(const QJsonArray &reviews) {
    m_allReviews = reviews;
    m_reviewsTable->setRowCount(0);

    for (int i = 0; i < reviews.size(); ++i) {
        QJsonObject review = reviews[i].toObject();
        m_reviewsTable->insertRow(i);

        QTableWidgetItem *userItem = new QTableWidgetItem(review["username"].toString());
        userItem->setData(Qt::UserRole, review["review_id"].toString());
        m_reviewsTable->setItem(i, 0, userItem);
        m_reviewsTable->setItem(i, 1, new QTableWidgetItem(review["bookTitle"].toString()));
        m_reviewsTable->setItem(i, 2, new QTableWidgetItem(QString::number(review["rating"].toInt())));
        m_reviewsTable->setItem(i, 3, new QTableWidgetItem(review["comment"].toString()));
    }
}

void AdminPage::onDeleteReviewClicked() {
    int row = m_reviewsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک نظر را از جدول انتخاب کنید.");
        return;
    }
    QString reviewId = m_reviewsTable->item(row, 0)->data(Qt::UserRole).toString();

    if (QMessageBox::question(this, "حذف نظر", "آیا از حذف این نظر مطمئن هستید؟") != QMessageBox::Yes)
        return;

    m_authManager->adminDeleteReview(reviewId);
}

void AdminPage::onAdminReviewDeleted(bool success, const QString &message) {
    QMessageBox::information(this, success ? "موفق" : "خطا", message);
    if (success) m_authManager->requestAllReviewsForAdmin();
}