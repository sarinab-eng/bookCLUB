#include "BookDetailsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include "Review.h"
#include "GlobalReviewManager.h"
#include "Shoppingcart.h"


BookDetailsWidget::BookDetailsWidget(QWidget *parent) : QWidget(parent) {
    setupUi();
}

void BookDetailsWidget::setupUi() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // بخش اطلاعات کتاب
    titleLabel = new QLabel();
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2C3E50;");

    authorLabel = new QLabel();
    authorLabel->setStyleSheet("font-size: 18px; color: #555;");

    publisherLabel = new QLabel();
    genreLabel = new QLabel();

    descriptionLabel = new QLabel();
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setStyleSheet("margin-top: 10px; line-height: 1.5;");

    priceLabel = new QLabel();
    priceLabel->setStyleSheet("font-size: 20px; color: #E74C3C; font-weight: bold;");

    addToCartButton = new QPushButton("افزودن به سبد خرید");
    addToCartButton->setStyleSheet("background-color: #27AE60; color: white; padding: 10px; font-size: 16px;");

    // بخش نظرات
    reviewsTable = new QTableWidget(0, 3);
    reviewsTable->setHorizontalHeaderLabels({"کاربر", "امتیاز", "نظر"});
    reviewsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ratingSpinBox = new QSpinBox();
    ratingSpinBox->setRange(1, 5);
    reviewTextEdit = new QTextEdit();
    reviewTextEdit->setPlaceholderText("نظر خود را بنویسید...");
    reviewTextEdit->setMaximumHeight(80);

    submitReviewButton = new QPushButton("ثبت نظر");

    // چیدمان
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(authorLabel);
    mainLayout->addWidget(publisherLabel);
    mainLayout->addWidget(genreLabel);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addWidget(priceLabel);
    mainLayout->addWidget(addToCartButton);
    mainLayout->addWidget(new QLabel("<h3>نظرات کاربران</h3>"));
    mainLayout->addWidget(reviewsTable);
    mainLayout->addWidget(new QLabel("ثبت نظر جدید:"));
    mainLayout->addWidget(ratingSpinBox);
    mainLayout->addWidget(reviewTextEdit);
    mainLayout->addWidget(submitReviewButton);

    // اتصال سیگنال‌ها
    connect(addToCartButton, &QPushButton::clicked, this, &BookDetailsWidget::onAddToCartClicked);
    connect(submitReviewButton, &QPushButton::clicked, this, &BookDetailsWidget::onSubmitReviewClicked);
}

void BookDetailsWidget::setBook(const Book &book) {
    currentBook = book;

    titleLabel->setText(book.getTitle());
    authorLabel->setText("نویسنده: " + book.getAuthor());
    publisherLabel->setText("ناشر: " + book.getPublisher());
    genreLabel->setText("ژانر: " + book.getGenre());
    descriptionLabel->setText(book.getDescription());
    priceLabel->setText(QString("قیمت: %1 تومان").arg(book.getPrice()));

    updateReviewsList();
}

void BookDetailsWidget::updateReviewsList() {
    reviewsTable->setRowCount(0);
    // استفاده از متد getReviews() که در کلاس Book خودتان تعریف کردید
    QVector<Review> reviews = currentBook.getReviews();

    for (const auto &review : reviews) {
        int row = reviewsTable->rowCount();
        reviewsTable->insertRow(row);
        // فرض بر این است که کلاس Review متدهای getUserName, getRating و getText را دارد
        reviewsTable->setItem(row, 0, new QTableWidgetItem(review.getUserName()));
        reviewsTable->setItem(row, 1, new QTableWidgetItem(QString::number(review.getRating())));
        reviewsTable->setItem(row, 2, new QTableWidgetItem(review.getText()));
    }
}

void BookDetailsWidget::onAddToCartClicked() {
    // استفاده از متد addItem که در ShoppingCart شما تعریف شده
    ShoppingCart::getInstance()->addItem(currentBook);
    QMessageBox::information(this, "سبد خرید", "کتاب با موفقیت به سبد خرید اضافه شد.");
}

void BookDetailsWidget::onSubmitReviewClicked() {
    QString text = reviewTextEdit->toPlainText().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "خطا", "لطفاً متن نظر را وارد کنید.");
        return;
    }

    // ساخت یک شیء Review بر اساس پارامترهای کلاس شما
    // نکته: پارامترها را بر اساس سازنده کلاس Review خودتان تنظیم کنید
    Review newReview;
    newReview.setText(text);
    newReview.setRating(ratingSpinBox->value());
    newReview.setBookTitle(currentBook.getTitle());
    // newReview.setUserName("User"); // این بخش را بعداً با سیستم لاگین ست کنید

    // استفاده از GlobalReviewManager برای ثبت سراسری
    GlobalReviewManager::getInstance()->addReview(newReview);

    // اضافه کردن به مدل محلی کتاب برای نمایش سریع
    currentBook.addReview(newReview);

    reviewTextEdit->clear();
    updateReviewsList();
    QMessageBox::information(this, "موفقیت", "نظر شما ثبت شد.");
}
