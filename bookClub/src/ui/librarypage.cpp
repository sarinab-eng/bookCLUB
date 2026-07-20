#include "librarypage.h"
#include "ui_librarypage.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QJsonObject>
#include <QJsonValue>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QLabel>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QHeaderView>

LibraryPage::LibraryPage(AuthManager *authManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LibraryPage),
    m_authManager(authManager)
{
    ui->setupUi(this);

    // تنظیمات تکمیلی جداول (غیر قابل ویرایش کردن سلول‌ها و فیت کردن ستون‌ها)
    setupTables();
    buildExtraTabs();

    // اتصال سیگنال‌های AuthManager به اسلات‌های این صفحه
    connect(m_authManager, &AuthManager::libraryReceived, this, &LibraryPage::onLibraryReceived);
    connect(m_authManager, &AuthManager::purchaseHistoryReceived, this, &LibraryPage::onPurchaseHistoryReceived);
    connect(m_authManager, &AuthManager::savedBooksReceived, this, &LibraryPage::onSavedBooksReceived);
    connect(m_authManager, &AuthManager::savedBookChanged, this, &LibraryPage::onSavedBookChanged);
    connect(m_authManager, &AuthManager::shelvesReceived, this, &LibraryPage::onShelvesReceived);

    // اتصال دکمه رفرش طراحی شده در UI به متد بروزرسانی
    connect(ui->refreshButton, &QPushButton::clicked, this, &LibraryPage::requestLibraryRefresh);
}

LibraryPage::~LibraryPage()
{
    delete ui;
}

void LibraryPage::setUsername(const QString &username)
{
    m_username = username;
}

void LibraryPage::requestLibraryRefresh()
{
    if (m_username.isEmpty()) return;

    // ارسال درخواست‌ها به سرور
    m_authManager->requestLibrary(m_username);
    m_authManager->requestPurchaseHistory(m_username);
    m_authManager->requestSavedBooks(m_username);
    m_authManager->requestShelves(m_username);
}

void LibraryPage::setupTables()
{
    ui->libraryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->libraryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->libraryTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void LibraryPage::buildExtraTabs()
{
    QTabWidget *tabs = new QTabWidget(this);

    // ---- تب ۱: کتاب‌های من ----
    QWidget *myBooksTab = new QWidget;
    QVBoxLayout *myBooksLayout = new QVBoxLayout(myBooksTab);
    myBooksLayout->setContentsMargins(0, 0, 0, 0);
    myBooksLayout->addWidget(ui->label);
    myBooksLayout->addWidget(ui->libraryCountLabel);
    myBooksLayout->addWidget(ui->libraryTable);

    QHBoxLayout *myBooksButtons = new QHBoxLayout;
    QPushButton *viewDetailsBtn = new QPushButton("مشاهده جزئیات");
    QPushButton *openFileBtn = new QPushButton("باز کردن فایل");
    myBooksButtons->addWidget(viewDetailsBtn);
    myBooksButtons->addWidget(openFileBtn);
    myBooksLayout->addLayout(myBooksButtons);
    myBooksLayout->addWidget(ui->refreshButton);

    connect(viewDetailsBtn, &QPushButton::clicked, this, &LibraryPage::onViewDetailsClicked);
    connect(openFileBtn, &QPushButton::clicked, this, &LibraryPage::onOpenFileClicked);

    tabs->addTab(myBooksTab, "کتاب‌های من");

    // ---- تب ۲: کتاب‌های ذخیره‌شده ----
    m_savedTab = new QWidget;
    QVBoxLayout *savedLayout = new QVBoxLayout(m_savedTab);

    m_savedTable = new QTableWidget(0, 3, m_savedTab);
    m_savedTable->setHorizontalHeaderLabels({"عنوان", "نویسنده", "قیمت"});
    m_savedTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_savedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_savedTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_removeSavedButton = new QPushButton("حذف از لیست ذخیره‌شده");
    connect(m_removeSavedButton, &QPushButton::clicked, this, &LibraryPage::onRemoveSavedClicked);

    savedLayout->addWidget(new QLabel("کتاب‌های ذخیره‌شده برای مطالعه در آینده:"));
    savedLayout->addWidget(m_savedTable);
    savedLayout->addWidget(m_removeSavedButton);

    tabs->addTab(m_savedTab, "ذخیره‌شده");

    // ---- تب ۳: قفسه‌ها ----
    m_shelvesTab = new QWidget;
    QHBoxLayout *shelvesLayout = new QHBoxLayout(m_shelvesTab);

    QVBoxLayout *shelvesLeftLayout = new QVBoxLayout;
    m_shelvesList = new QListWidget;
    m_newShelfButton = new QPushButton("قفسه جدید");
    m_deleteShelfButton = new QPushButton("حذف قفسه");
    shelvesLeftLayout->addWidget(new QLabel("قفسه‌های من:"));
    shelvesLeftLayout->addWidget(m_shelvesList);
    shelvesLeftLayout->addWidget(m_newShelfButton);
    shelvesLeftLayout->addWidget(m_deleteShelfButton);

    QVBoxLayout *shelvesRightLayout = new QVBoxLayout;
    m_shelfContentsTable = new QTableWidget(0, 2);
    m_shelfContentsTable->setHorizontalHeaderLabels({"عنوان", "نویسنده"});
    m_shelfContentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_shelfContentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_shelfContentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QHBoxLayout *shelfActionsLayout = new QHBoxLayout;
    m_addToShelfCombo = new QComboBox;
    m_addBookToShelfButton = new QPushButton("افزودن به قفسه");
    m_removeBookFromShelfButton = new QPushButton("حذف از قفسه");
    shelfActionsLayout->addWidget(m_addToShelfCombo);
    shelfActionsLayout->addWidget(m_addBookToShelfButton);
    shelfActionsLayout->addWidget(m_removeBookFromShelfButton);

    shelvesRightLayout->addWidget(new QLabel("محتویات قفسه:"));
    shelvesRightLayout->addWidget(m_shelfContentsTable);
    shelvesRightLayout->addLayout(shelfActionsLayout);

    shelvesLayout->addLayout(shelvesLeftLayout, 1);
    shelvesLayout->addLayout(shelvesRightLayout, 2);

    connect(m_shelvesList, &QListWidget::currentRowChanged, this, &LibraryPage::onShelfSelectionChanged);
    connect(m_newShelfButton, &QPushButton::clicked, this, &LibraryPage::onNewShelfClicked);
    connect(m_deleteShelfButton, &QPushButton::clicked, this, &LibraryPage::onDeleteShelfClicked);
    connect(m_addBookToShelfButton, &QPushButton::clicked, this, &LibraryPage::onAddBookToShelfClicked);
    connect(m_removeBookFromShelfButton, &QPushButton::clicked, this, &LibraryPage::onRemoveBookFromShelfClicked);

    tabs->addTab(m_shelvesTab, "قفسه‌ها");

    // ---- تب ۴: تاریخچه خرید ----
    QWidget *historyTab = new QWidget;
    QVBoxLayout *historyLayout = new QVBoxLayout(historyTab);
    historyLayout->setContentsMargins(0, 0, 0, 0);
    historyLayout->addWidget(ui->label_2);
    historyLayout->addWidget(ui->historyTable);

    tabs->addTab(historyTab, "تاریخچه خرید");

    QLayout *oldLayout = layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr)
            delete item; 
        delete oldLayout;
    }
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(tabs);
}

void LibraryPage::onLibraryReceived(const QJsonArray &books)
{
    m_libraryBooks = books;

    ui->libraryTable->setRowCount(0); // پاک کردن داده‌های قبلی جدول کتاب‌ها

    for (int i = 0; i < books.size(); ++i) {
        QJsonObject bookObj = books[i].toObject();
        ui->libraryTable->insertRow(i);

        QTableWidgetItem *titleItem = new QTableWidgetItem(bookObj["title"].toString());
        titleItem->setData(Qt::UserRole, bookObj["id"].toString());
        ui->libraryTable->setItem(i, 0, titleItem);
        ui->libraryTable->setItem(i, 1, new QTableWidgetItem(bookObj["genre"].toString()));
        ui->libraryTable->setItem(i, 2, new QTableWidgetItem(bookObj["author"].toString()));

        QString purchaseDate = bookObj.contains("purchaseDate") ? bookObj["purchaseDate"].toString() : "-";
        ui->libraryTable->setItem(i, 3, new QTableWidgetItem(purchaseDate));
    }

    ui->libraryCountLabel->setText(QString("تعداد کتاب‌های خریداری‌شده: %1").arg(books.size()));

    // به‌روزرسانی لیست کتاب‌های قابل افزودن به قفسه‌ی انتخاب‌شده
    onShelfSelectionChanged();
}

void LibraryPage::onPurchaseHistoryReceived(const QJsonArray &history)
{
    ui->historyTable->setRowCount(0); // پاک کردن داده‌های قبلی جدول تراکنش‌ها

    for (int i = 0; i < history.size(); ++i) {
        QJsonObject historyObj = history[i].toObject();
        ui->historyTable->insertRow(i);

        QString txId = QString("TX-%1").arg(i + 1);
        double total = historyObj["total"].toDouble();
        double discount = historyObj["discountAmount"].toDouble();
        double finalAmount = historyObj["finalAmount"].toDouble();
        QString dateStr = historyObj["date"].toString();

        ui->historyTable->setItem(i, 0, new QTableWidgetItem(txId));
        ui->historyTable->setItem(i, 1, new QTableWidgetItem(QString::number(total, 'f', 2)));
        ui->historyTable->setItem(i, 2, new QTableWidgetItem(QString::number(discount, 'f', 2)));
        ui->historyTable->setItem(i, 3, new QTableWidgetItem(QString::number(finalAmount, 'f', 2)));
        ui->historyTable->setItem(i, 4, new QTableWidgetItem(dateStr));
    }
}

void LibraryPage::onViewDetailsClicked()
{
    int row = ui->libraryTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک کتاب را از جدول انتخاب کنید.");
        return;
    }
    QString bookId = ui->libraryTable->item(row, 0)->data(Qt::UserRole).toString();
    for (const QJsonValue &v : m_libraryBooks) {
        if (v.toObject()["id"].toString() == bookId) {
            emit bookDetailRequested(v.toObject());
            return;
        }
    }
}

void LibraryPage::onOpenFileClicked()
{
    int row = ui->libraryTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک کتاب را از جدول انتخاب کنید.");
        return;
    }
    QString bookId = ui->libraryTable->item(row, 0)->data(Qt::UserRole).toString();
    for (const QJsonValue &v : m_libraryBooks) {
        QJsonObject book = v.toObject();
        if (book["id"].toString() != bookId) continue;

        QString fileUrl = book.value("fileURL").toString();
        if (fileUrl.isEmpty()) {
            QMessageBox::information(this, "فایل کتاب", "فایل PDF برای این کتاب تعریف نشده است.");
            return;
        }
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(fileUrl))) {
            QMessageBox::warning(this, "خطا", "باز کردن فایل ممکن نشد: " + fileUrl);
        }
        return;
    }
}

// ================= کتاب‌های ذخیره‌شده =================

void LibraryPage::onSavedBooksReceived(const QJsonArray &items)
{
    m_savedTable->setRowCount(0);
    for (int i = 0; i < items.size(); ++i) {
        QJsonObject book = items[i].toObject();
        m_savedTable->insertRow(i);

        QTableWidgetItem *titleItem = new QTableWidgetItem(book["title"].toString());
        titleItem->setData(Qt::UserRole, book["id"].toString());
        m_savedTable->setItem(i, 0, titleItem);
        m_savedTable->setItem(i, 1, new QTableWidgetItem(book["author"].toString()));

        double price = book["price"].toDouble();
        m_savedTable->setItem(i, 2, new QTableWidgetItem(price > 0 ? QString::number(price, 'f', 0) : "رایگان"));
    }
}

void LibraryPage::onSavedBookChanged(bool success, const QString &message)
{
    if (!success) {
        QMessageBox::warning(this, "خطا", message);
        return;
    }
    if (!m_username.isEmpty()) m_authManager->requestSavedBooks(m_username);
}

void LibraryPage::onRemoveSavedClicked()
{
    int row = m_savedTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک کتاب را از لیست انتخاب کنید.");
        return;
    }
    QString bookId = m_savedTable->item(row, 0)->data(Qt::UserRole).toString();
    m_authManager->unsaveBook(m_username, bookId);
}

// ================= قفسه‌ها =================

QJsonObject LibraryPage::currentShelf() const
{
    for (const QJsonValue &v : m_shelves) {
        QJsonObject shelf = v.toObject();
        if (shelf["id"].toString() == m_selectedShelfId) return shelf;
    }
    return QJsonObject();
}

void LibraryPage::onShelvesReceived(bool success, const QString &message, const QJsonArray &shelves)
{
    if (!success && !message.isEmpty()) {
        QMessageBox::warning(this, "خطا", message);
    }

    m_shelves = shelves;

    QString previouslySelected = m_selectedShelfId;
    m_shelvesList->clear();
    int rowToSelect = -1;
    for (int i = 0; i < shelves.size(); ++i) {
        QJsonObject shelf = shelves[i].toObject();
        QListWidgetItem *item = new QListWidgetItem(shelf["name"].toString());
        item->setData(Qt::UserRole, shelf["id"].toString());
        m_shelvesList->addItem(item);
        if (shelf["id"].toString() == previouslySelected) rowToSelect = i;
    }

    if (rowToSelect >= 0) m_shelvesList->setCurrentRow(rowToSelect);
    else if (m_shelvesList->count() > 0) m_shelvesList->setCurrentRow(0);
    else onShelfSelectionChanged();
}

void LibraryPage::onShelfSelectionChanged()
{
    QListWidgetItem *item = m_shelvesList->currentItem();
    m_selectedShelfId = item ? item->data(Qt::UserRole).toString() : QString();

    QJsonObject shelf = currentShelf();
    QJsonArray books = shelf.value("books").toArray();

    m_shelfContentsTable->setRowCount(0);
    for (int i = 0; i < books.size(); ++i) {
        QJsonObject book = books[i].toObject();
        m_shelfContentsTable->insertRow(i);
        QTableWidgetItem *titleItem = new QTableWidgetItem(book["title"].toString());
        titleItem->setData(Qt::UserRole, book["id"].toString());
        m_shelfContentsTable->setItem(i, 0, titleItem);
        m_shelfContentsTable->setItem(i, 1, new QTableWidgetItem(book["author"].toString()));
    }

    m_addToShelfCombo->clear();
    for (const QJsonValue &v : m_libraryBooks) {
        QJsonObject book = v.toObject();
        QString bookId = book["id"].toString();
        bool alreadyInShelf = false;
        for (const QJsonValue &bv : books) {
            if (bv.toObject()["id"].toString() == bookId) { alreadyInShelf = true; break; }
        }
        if (!alreadyInShelf) m_addToShelfCombo->addItem(book["title"].toString(), bookId);
    }
}

void LibraryPage::onNewShelfClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "قفسه جدید", "نام قفسه:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    m_authManager->createShelf(m_username, name.trimmed());
}

void LibraryPage::onDeleteShelfClicked()
{
    if (m_selectedShelfId.isEmpty()) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک قفسه را انتخاب کنید.");
        return;
    }
    if (QMessageBox::question(this, "حذف قفسه", "آیا از حذف این قفسه مطمئن هستید؟") != QMessageBox::Yes)
        return;
    m_authManager->deleteShelf(m_username, m_selectedShelfId);
}

void LibraryPage::onAddBookToShelfClicked()
{
    if (m_selectedShelfId.isEmpty()) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک قفسه را انتخاب کنید.");
        return;
    }
    QString bookId = m_addToShelfCombo->currentData().toString();
    if (bookId.isEmpty()) {
        QMessageBox::warning(this, "خطا", "کتابی برای افزودن موجود نیست.");
        return;
    }
    m_authManager->addBookToShelf(m_username, m_selectedShelfId, bookId);
}

void LibraryPage::onRemoveBookFromShelfClicked()
{
    if (m_selectedShelfId.isEmpty()) return;
    int row = m_shelfContentsTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا یک کتاب را از قفسه انتخاب کنید.");
        return;
    }
    QString bookId = m_shelfContentsTable->item(row, 0)->data(Qt::UserRole).toString();
    m_authManager->removeBookFromShelf(m_username, m_selectedShelfId, bookId);
}
