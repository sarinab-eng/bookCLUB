#include "publisherpage.h"
#include "addeditbookdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QMessageBox>

PublisherPage::PublisherPage(AuthManager *authManager, QWidget *parent)
    : QWidget(parent), m_authManager(authManager)
{
    setupUi();

    connect(m_authManager, &AuthManager::profileReceived, this, &PublisherPage::onProfileReceived);
    connect(m_authManager, &AuthManager::passwordChanged, this, &PublisherPage::onPasswordChanged);
    connect(m_authManager, &AuthManager::publisherBooksReceived, this, &PublisherPage::onPublisherBooksReceived);
    connect(m_authManager, &AuthManager::bookPublished, this, &PublisherPage::onBookPublished);
    connect(m_authManager, &AuthManager::bookUpdated, this, &PublisherPage::onBookUpdated);
    connect(m_authManager, &AuthManager::bookActiveStatusChanged, this, &PublisherPage::onBookActiveStatusChanged);
    connect(m_authManager, &AuthManager::publisherStatsReceived, this, &PublisherPage::onPublisherStatsReceived);
}

void PublisherPage::setUsername(const QString &username)
{
    m_username = username;
    m_usernameValueLabel->setText(username);
    refresh();
}

void PublisherPage::refresh()
{
    if (m_username.isEmpty()) return;
    m_authManager->requestProfile(m_username);
    m_authManager->requestPublisherBooks(m_username);
    m_authManager->requestPublisherStats(m_username);
}

void PublisherPage::setupUi()
{
      setObjectName("PublisherPage");
        setStyleSheet(
            /* پس‌زمینه کلی صفحه همچنان یاسی/صورتی خیلی روشن بماند تا متن سرخابی خودش را نشان دهد */
            "#PublisherPage { background-color: #FFF0F5; font-family: \"Segoe UI\", \"Tahoma\"; }"

            /* تمام لیبل‌ها (نوشته‌های ثابت مثل نام کاربری، نقش و غیره) */
            "QLabel { color: #FF1493; font-family: \"Segoe UI\", \"Tahoma\"; font-weight: 500; }"

            /* استایل فیلدهای ورودی */
            "QLineEdit { border: 2px solid #FFC0CB; border-radius: 10px; padding: 6px; background: #FFFFFF; color: #333333; }"
            "QLineEdit:focus { border: 2px solid #FF1493; }" /* موقع کلیک لبه‌ها سرخابی شود */

            /* دکمه‌های اصلی مثل دکمه تغییر رمز */
            "QPushButton { background-color: #FF1493; color: #FFFFFF; border-radius: 10px; padding: 8px 15px; font-weight: bold; }"
            "QPushButton:hover { background-color: #C71585; }" /* موقع هاور تیره‌تر شود */

            /* استایل جداول */
            "QTableWidget, QTableView { background-color: #FFFFFF; border: 2px solid #FFC0CB; border-radius: 15px;"
            "  gridline-color: #FFF0F5; selection-background-color: #FF69B4; selection-color: #FFFFFF; }"
            "QHeaderView::section { background-color: #FF1493; color: #FFFFFF; padding: 6px; border: none; font-weight: bold; }"

            /* استایل تب‌ها (حساب کاربری، کتاب‌های من و ...) */
            "QTabWidget::pane { border: 2px solid #FFC0CB; border-radius: 10px; }"
            "QTabBar::tab { background: #FFFFFF; color: #FF1493; border: 2px solid #FFC0CB; border-bottom: none;"
            "  border-top-left-radius: 10px; border-top-right-radius: 10px; padding: 8px 16px; font-weight: bold; }"
            "QTabBar::tab:selected { background: #FF1493; color: #FFFFFF; }"

            /* دکمه خروج */
            "#logoutBtn { background-color: #E25858; color: #FFFFFF; }"
            "#logoutBtn:hover { background-color: #C0392B; }"

            /* تیتر پنل (پنل ناشر) */
            "#panelTitle { color: #C71585; font-size: 18px; font-weight: bold; }"
            );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    QLabel *panelTitleLabel = new QLabel("پنل ناشر");
    panelTitleLabel->setObjectName("panelTitle");
    QPushButton *logoutButton = new QPushButton("خروج");
    logoutButton->setObjectName("logoutBtn");
    headerLayout->addWidget(panelTitleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(logoutButton);
    mainLayout->addLayout(headerLayout);
    connect(logoutButton, &QPushButton::clicked, this, &PublisherPage::logoutRequested);

    QTabWidget *tabs = new QTabWidget;
    mainLayout->addWidget(tabs);

    // ================= تب ۱: حساب کاربری =================
    QWidget *accountTab = new QWidget;
    QVBoxLayout *accountLayout = new QVBoxLayout(accountTab);

    QLabel *infoTitle = new QLabel("اطلاعات حساب ناشر");
    infoTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    accountLayout->addWidget(infoTitle);

    QFormLayout *infoForm = new QFormLayout;
    m_usernameValueLabel = new QLabel;
    m_roleValueLabel = new QLabel;
    m_registrationDateValueLabel = new QLabel;
    infoForm->addRow("نام کاربری:", m_usernameValueLabel);
    infoForm->addRow("نقش:", m_roleValueLabel);
    infoForm->addRow("تاریخ عضویت:", m_registrationDateValueLabel);
    accountLayout->addLayout(infoForm);

    QLabel *passwordTitle = new QLabel("تغییر رمز عبور");
    passwordTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    accountLayout->addWidget(passwordTitle);

    QFormLayout *passwordForm = new QFormLayout;
    m_oldPasswordEdit = new QLineEdit;
    m_oldPasswordEdit->setEchoMode(QLineEdit::Password);
    m_newPasswordEdit = new QLineEdit;
    m_newPasswordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit = new QLineEdit;
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    passwordForm->addRow("رمز عبور فعلی:", m_oldPasswordEdit);
    passwordForm->addRow("رمز عبور جدید:", m_newPasswordEdit);
    passwordForm->addRow("تکرار رمز عبور جدید:", m_confirmPasswordEdit);
    accountLayout->addLayout(passwordForm);

    m_changePasswordButton = new QPushButton("تغییر رمز عبور");
    accountLayout->addWidget(m_changePasswordButton);
    accountLayout->addStretch();

    connect(m_changePasswordButton, &QPushButton::clicked, this, &PublisherPage::onChangePasswordClicked);

    tabs->addTab(accountTab, "حساب کاربری");

    // ================= تب ۲: کتاب‌های من =================
    QWidget *booksTab = new QWidget;
    QVBoxLayout *booksLayout = new QVBoxLayout(booksTab);

    m_booksTable = new QTableWidget(0, 6);
    m_booksTable->setHorizontalHeaderLabels({"عنوان", "ژانر", "قیمت", "تخفیف", "امتیاز", "وضعیت"});
    m_booksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_booksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_booksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    booksLayout->addWidget(m_booksTable);

    QHBoxLayout *booksButtonsLayout = new QHBoxLayout;
    m_addBookButton = new QPushButton("افزودن کتاب جدید");
    m_editBookButton = new QPushButton("ویرایش کتاب");
    m_toggleActiveButton = new QPushButton("فعال/غیرفعال کردن کتاب");
    booksButtonsLayout->addWidget(m_addBookButton);
    booksButtonsLayout->addWidget(m_editBookButton);
    booksButtonsLayout->addWidget(m_toggleActiveButton);
    booksLayout->addLayout(booksButtonsLayout);

    connect(m_addBookButton, &QPushButton::clicked, this, &PublisherPage::onAddBookClicked);
    connect(m_editBookButton, &QPushButton::clicked, this, &PublisherPage::onEditBookClicked);
    connect(m_toggleActiveButton, &QPushButton::clicked, this, &PublisherPage::onToggleActiveClicked);

    tabs->addTab(booksTab, "کتاب‌های من");

    // ================= تب ۳: داشبورد آمار =================
    QWidget *statsTab = new QWidget;
    QVBoxLayout *statsLayout = new QVBoxLayout(statsTab);

    QHBoxLayout *summaryLayout = new QHBoxLayout;
    m_totalBooksLabel = new QLabel("تعداد کل کتاب‌های منتشرشده: 0");
    m_totalRevenueLabel = new QLabel("مجموع درآمد: 0 تومان");
    summaryLayout->addWidget(m_totalBooksLabel);
    summaryLayout->addWidget(m_totalRevenueLabel);
    statsLayout->addLayout(summaryLayout);

    statsLayout->addWidget(new QLabel("پرفروش‌ترین کتاب‌ها:"));
    m_bestSellersTable = new QTableWidget(0, 3);
    m_bestSellersTable->setHorizontalHeaderLabels({"عنوان", "تعداد فروش", "امتیاز"});
    m_bestSellersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_bestSellersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    statsLayout->addWidget(m_bestSellersTable);

    statsLayout->addWidget(new QLabel("کم‌فروش‌ترین کتاب‌ها:"));
    m_worstSellersTable = new QTableWidget(0, 3);
    m_worstSellersTable->setHorizontalHeaderLabels({"عنوان", "تعداد فروش", "امتیاز"});
    m_worstSellersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_worstSellersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    statsLayout->addWidget(m_worstSellersTable);

    tabs->addTab(statsTab, "آمار و گزارش");
}

// ================= حساب کاربری =================

void PublisherPage::onProfileReceived(const QJsonObject &profile)
{
    if (profile.value("username").toString() != m_username) return;
    m_roleValueLabel->setText(profile.value("role").toString());
    m_registrationDateValueLabel->setText(profile.value("registration_date").toString());
}

void PublisherPage::onChangePasswordClicked()
{
    QString oldPass = m_oldPasswordEdit->text();
    QString newPass = m_newPasswordEdit->text();
    QString confirmPass = m_confirmPasswordEdit->text();

    if (oldPass.isEmpty() || newPass.isEmpty() || confirmPass.isEmpty()) {
        QMessageBox::warning(this, "خطا", "لطفاً تمام فیلدها را پر کنید.");
        return;
    }
    if (newPass != confirmPass) {
        QMessageBox::warning(this, "خطا", "رمز عبور جدید و تکرار آن یکسان نیستند.");
        return;
    }
    m_authManager->changePassword(m_username, oldPass, newPass);
}

void PublisherPage::onPasswordChanged(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "موفق", message);
        m_oldPasswordEdit->clear();
        m_newPasswordEdit->clear();
        m_confirmPasswordEdit->clear();
    } else {
        QMessageBox::warning(this, "خطا", message);
    }
}

// ================= کتاب‌های من =================

void PublisherPage::onPublisherBooksReceived(const QJsonArray &books)
{
    m_myBooks = books;
    m_booksTable->setRowCount(0);

    for (int i = 0; i < books.size(); ++i) {
        QJsonObject book = books[i].toObject();
        m_booksTable->insertRow(i);

        QTableWidgetItem *titleItem = new QTableWidgetItem(book["title"].toString());
        titleItem->setData(Qt::UserRole, book["id"].toString());
        m_booksTable->setItem(i, 0, titleItem);
        m_booksTable->setItem(i, 1, new QTableWidgetItem(book["genre"].toString()));
        m_booksTable->setItem(i, 2, new QTableWidgetItem(QString::number(book["price"].toDouble(), 'f', 0)));
        m_booksTable->setItem(i, 3, new QTableWidgetItem(QString("%1%").arg(book.value("discountPercent").toDouble())));
        m_booksTable->setItem(i, 4, new QTableWidgetItem(QString::number(book.value("averageRating").toDouble(), 'f', 1)));

        bool active = book.value("isActive").toBool(true);
        QTableWidgetItem *statusItem = new QTableWidgetItem(active ? "فعال" : "غیرفعال");
        m_booksTable->setItem(i, 5, statusItem);
    }
}

QJsonObject PublisherPage::selectedBook() const
{
    int row = m_booksTable->currentRow();
    if (row < 0) return QJsonObject();

    QString bookId = m_booksTable->item(row, 0)->data(Qt::UserRole).toString();
    for (const QJsonValue &v : m_myBooks) {
        if (v.toObject()["id"].toString() == bookId) return v.toObject();
    }
    return QJsonObject();
}

void PublisherPage::onAddBookClicked()
{
    AddEditBookDialog dialog(QJsonObject(), this);
    if (dialog.exec() != QDialog::Accepted) return;

    QJsonObject data = dialog.bookData();
    if (data.value("title").toString().isEmpty() || data.value("author").toString().isEmpty()) {
        QMessageBox::warning(this, "خطا", "نام کتاب و نویسنده نمی‌توانند خالی باشند.");
        return;
    }
    m_authManager->publishBook(m_username, data);
}

void PublisherPage::onEditBookClicked()
{
    QJsonObject book = selectedBook();
    if (book.isEmpty()) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک کتاب را از جدول انتخاب کنید.");
        return;
    }

    AddEditBookDialog dialog(book, this);
    if (dialog.exec() != QDialog::Accepted) return;

    m_authManager->updateBook(m_username, book["id"].toString(), dialog.bookData());
}

void PublisherPage::onToggleActiveClicked()
{
    QJsonObject book = selectedBook();
    if (book.isEmpty()) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک کتاب را از جدول انتخاب کنید.");
        return;
    }

    bool active = book.value("isActive").toBool(true);
    QString bookId = book["id"].toString();

    if (active) {
        if (QMessageBox::question(this, "غیرفعال‌سازی کتاب",
                "این کتاب از فروشگاه و نتایج جستجو حذف می‌شود؛ کاربرانی که قبلاً خریده‌اند "
                "همچنان به آن دسترسی دارند. ادامه می‌دهید؟") != QMessageBox::Yes)
            return;
        m_authManager->deactivateBook(m_username, bookId);
    } else {
        m_authManager->activateBook(m_username, bookId);
    }
}

void PublisherPage::onBookPublished(bool success, const QString &message, const QJsonObject &book)
{
    Q_UNUSED(book);
    if (success) {
        QMessageBox::information(this, "موفق", message);
        refresh();
    } else {
        QMessageBox::warning(this, "خطا", message);
    }
}

void PublisherPage::onBookUpdated(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "موفق", message);
        refresh();
    } else {
        QMessageBox::warning(this, "خطا", message);
    }
}

void PublisherPage::onBookActiveStatusChanged(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "موفق", message);
        refresh();
    } else {
        QMessageBox::warning(this, "خطا", message);
    }
}

// ================= آمار و گزارش =================

void PublisherPage::populateStatsTable(QTableWidget *table, const QJsonArray &books)
{
    table->setRowCount(0);
    for (int i = 0; i < books.size(); ++i) {
        QJsonObject book = books[i].toObject();
        table->insertRow(i);
        table->setItem(i, 0, new QTableWidgetItem(book["title"].toString()));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(book["salesCount"].toInt())));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(book["averageRating"].toDouble(), 'f', 1)));
    }
}

void PublisherPage::onPublisherStatsReceived(const QJsonObject &stats)
{
    m_totalBooksLabel->setText(QString("تعداد کل کتاب‌های منتشرشده: %1").arg(stats.value("totalBooksCount").toInt()));
    m_totalRevenueLabel->setText(QString("مجموع درآمد: %1 تومان")
                                      .arg(QString::number(stats.value("totalRevenue").toDouble(), 'f', 0)));

    populateStatsTable(m_bestSellersTable, stats.value("bestSellers").toArray());
    populateStatsTable(m_worstSellersTable, stats.value("worstSellers").toArray());
}
