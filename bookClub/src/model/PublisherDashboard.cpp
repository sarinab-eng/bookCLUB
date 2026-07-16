#include "PublisherDashboard.h"
#include <QHeaderView>
#include <QMessageBox>

PublisherDashboard::PublisherDashboard(QWidget *parent) : QWidget(parent) {
    setupUI();

    // اتصال سیگنال شبکه به آپدیت جدول
    connect(ClientNetworkManager::getInstance(), &ClientNetworkManager::allBooksReceived,
            this, &PublisherDashboard::updateBookTable);

    connect(addBtn, &QPushButton::clicked, this, &PublisherDashboard::onAddBookClicked);
    connect(refreshBtn, &QPushButton::clicked, [this](){
        ClientNetworkManager::getInstance()->requestAllBooks(); // یا متد اختصاصی ناشر
    });
}

void PublisherDashboard::setupUI() {
    auto *mainLayout = new QVBoxLayout(this);

    auto *titleLabel = new QLabel("پنل مدیریت ناشر", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);

    // جدول کتاب‌ها
    bookTable = new QTableWidget(0, 4, this);
    bookTable->setHorizontalHeaderLabels({"شناسه", "عنوان کتاب", "قیمت", "موجودی"});
    bookTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bookTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    bookTable->setAlternatingRowColors(true);
    bookTable->setStyleSheet("QTableWidget { background-color: #ffffff; alternate-background-color: #f2f2f2; }");

    mainLayout->addWidget(bookTable);

    // دکمه‌ها
    auto *btnLayout = new QHBoxLayout();
    addBtn = new QPushButton("افزودن کتاب جدید", this);
    refreshBtn = new QPushButton("به‌روزرسانی لیست", this);
    deleteBtn = new QPushButton("حذف کتاب انتخاب شده", this);

    addBtn->setStyleSheet("background-color: #27ae60; color: white; padding: 8px;");
    deleteBtn->setStyleSheet("background-color: #c0392b; color: white; padding: 8px;");

    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(refreshBtn);

    mainLayout->addLayout(btnLayout);
}

void PublisherDashboard::updateBookTable(const QVector<Book> &books) {
    bookTable->setRowCount(0);
    for (const auto &book : books) {
        int row = bookTable->rowCount();
        bookTable->insertRow(row);
        bookTable->setItem(row, 0, new QTableWidgetItem(QString::number(book.getId())));
        bookTable->setItem(row, 1, new QTableWidgetItem(book.getTitle()));
        bookTable->setItem(row, 2, new QTableWidgetItem(QString::number(book.getPrice())));
        bookTable->setItem(row, 3, new QTableWidgetItem(QString::number(book.getStock())));
    }
}

void PublisherDashboard::onAddBookClicked() {
    // در اینجا باید یک Dialog باز کنید تا اطلاعات کتاب (نام، قیمت و...) را بگیرد
    // فعلاً یک پیغام تست:
    QMessageBox::information(this, "افزودن", "در اینجا فرم افزودن کتاب باز خواهد شد.");
}

void PublisherDashboard::onDeleteBookClicked() {
    int currentRow = bookTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "خطا", "لطفاً یک کتاب را انتخاب کنید.");
        return;
    }
    int bookId = bookTable->item(currentRow, 0)->text().toInt();
    // فراخوانی متد حذف در NetworkManager (باید پیاده‌سازی شود)
    // ClientNetworkManager::getInstance()->sendDeleteBookRequest(bookId);
}
