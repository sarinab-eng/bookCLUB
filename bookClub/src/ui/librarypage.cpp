#include "librarypage.h"
#include "ui_librarypage.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonValue>

LibraryPage::LibraryPage(AuthManager *authManager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LibraryPage),
    m_authManager(authManager)
{
    ui->setupUi(this);

    // تنظیمات تکمیلی جداول (غیر قابل ویرایش کردن سلول‌ها و فیت کردن ستون‌ها)
    setupTables();

    // اتصال سیگنال‌های AuthManager به اسلات‌های این صفحه
    connect(m_authManager, &AuthManager::libraryReceived, this, &LibraryPage::onLibraryReceived);
    connect(m_authManager, &AuthManager::purchaseHistoryReceived, this, &LibraryPage::onPurchaseHistoryReceived);

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
}

void LibraryPage::setupTables()
{
    // استفاده از نام‌های دقیق UI شما (librTable و histTable)
    ui->libraryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->libraryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void LibraryPage::onLibraryReceived(const QJsonArray &books)
{
    ui->libraryTable->setRowCount(0); // پاک کردن داده‌های قبلی جدول کتاب‌ها

    for (int i = 0; i < books.size(); ++i) {
        QJsonObject bookObj = books[i].toObject();
        ui->libraryTable->insertRow(i);

        // پر کردن ستون‌ها بر اساس چیدمان ستون‌های شما در UI:
        // ستون 0: عنوان | ستون 1: ژانر | ستون 2: نویسنده | ستون 3: تاریخ خرید (در صورت وجود در سمت سرور)
        ui->libraryTable->setItem(i, 0, new QTableWidgetItem(bookObj["title"].toString()));
        ui->libraryTable->setItem(i, 1, new QTableWidgetItem(bookObj["genre"].toString()));
        ui->libraryTable->setItem(i, 2, new QTableWidgetItem(bookObj["author"].toString()));

        // اگر سرور تاریخ خرید کتاب خاصی را نفرستد، سلول را خالی یا پیش‌فرض می‌گذاریم
        QString purchaseDate = bookObj.contains("purchaseDate") ? bookObj["purchaseDate"].toString() : "-";
        ui->libraryTable->setItem(i, 3, new QTableWidgetItem(purchaseDate));
    }
}

void LibraryPage::onPurchaseHistoryReceived(const QJsonArray &history)
{
    ui->historyTable->setRowCount(0); // پاک کردن داده‌های قبلی جدول تراکنش‌ها

    for (int i = 0; i < history.size(); ++i) {
        QJsonObject historyObj = history[i].toObject();
        ui->historyTable->insertRow(i);

        // پر کردن ستون‌ها بر اساس چیدمان ستون‌های شما در UI:
        // ستون 0: کد تراکنش | ستون 1: مبلغ کل | ستون 2: تخفیف | ستون 3: مبلغ پرداخت شده | ستون 4: تاریخ

        // از آنجایی که در سیستم شما کد تراکنش مستقیم ذخیره نمی‌شود، می‌توانیم از شماره ردیف (یا شناسه داخلی) استفاده کنیم:
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
