#include "BookDetailsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>

BookDetailsWidget::BookDetailsWidget(QWidget *parent) : QWidget(parent) {
    // ایجاد لایوت اصلی صفحه (عمودی)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // بخش بالایی: اطلاعات متنی کتاب
    titleLabel = new QLabel("عنوان کتاب", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");

    authorLabel = new QLabel("نویسنده: ", this);
    publisherLabel = new QLabel("ناشر: ", this);
    ratingLabel = new QLabel("امتیاز: ", this);
    priceLabel = new QLabel("قیمت: ", this);
    priceLabel->setStyleSheet("color: green; font-weight: bold;");

    descriptionBrowser = new QTextBrowser(this);
    descriptionBrowser->setPlaceholderText("توضیحات کتاب...");

    // افزودن اطلاعات به لایوت عمودی بالا
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(authorLabel);
    mainLayout->addWidget(publisherLabel);
    mainLayout->addWidget(ratingLabel);
    mainLayout->addWidget(priceLabel);
    mainLayout->addWidget(new QLabel("توضیحات:", this));
    mainLayout->addWidget(descriptionBrowser);

    // دکمه افزودن به سبد خرید
    addToCartButton = new QPushButton("افزودن به سبد خرید", this);
    mainLayout->addWidget(addToCartButton);

    // بخش نظرات
    mainLayout->addWidget(new QLabel("نظرات کاربران:", this));
    reviewsListWidget = new QListWidget(this);
    mainLayout->addWidget(reviewsListWidget);

    // بخش ثبت نظر جدید
    QHBoxLayout *newReviewLayout = new QHBoxLayout();
    ratingSpinBox = new QSpinBox(this);
    ratingSpinBox->setRange(1, 5); // فقط امتیاز بین ۱ تا ۵ ستاره

    addReviewButton = new QPushButton("ثبت نظر و امتیاز", this);

    newReviewLayout->addWidget(new QLabel("امتیاز شما (۱-۵):"));
    newReviewLayout->addWidget(ratingSpinBox);
    newReviewLayout->addWidget(addReviewButton);
    mainLayout->addLayout(newReviewLayout);

    // اتصال سیگنال‌ها به اسلات‌ها
    connect(addToCartButton, &QPushButton::clicked, this, &BookDetailsWidget::onAddToCartClicked);
    connect(addReviewButton, &QPushButton::clicked, this, &BookDetailsWidget::onAddReviewClicked);
}

void BookDetailsWidget::setBook(const Book &book) {
    currentBook = book;

    // پر کردن فیلدهای متنی بر اساس اطلاعات کتاب
    titleLabel->setText(book.getTitle());
    authorLabel->setText("نویسنده: " + book.getAuthor());
    publisherLabel->setText("ناشر: " + book.getPublisher());
    priceLabel->setText(QString("قیمت: %1 تومان").arg(book.getPrice()));
    ratingLabel->setText(QString("امتیاز میانگین: %1 / 5").arg(book.getRating()));
    descriptionBrowser->setText(book.getDescription());

    // خالی کردن لیست نظرات قبلی و نمایش نظرات جدید کتاب
    reviewsListWidget->clear();
    for (const Review &r : book.getReviews()) {
        QString itemText = QString("کاربر %1 (امتیاز: %2/5):\n%3")
                               .arg(r.getUserId()) // یا نام کاربری در صورت وجود
                               .arg(r.getRating())
                               .arg(r.getComment());
        reviewsListWidget->addItem(itemText);
    }
}

void BookDetailsWidget::onAddToCartClicked() {
    emit addToCartRequested(currentBook.getId());
    QMessageBox::information(this, "سبد خرید", "کتاب به سبد خرید اضافه شد.");
}

void BookDetailsWidget::onAddReviewClicked() {
    bool ok;
    // باز کردن یک پنجره کوچک برای گرفتن متن نظر از کاربر
    QString comment = QInputDialog::getMultiLineText(this, "ثبت نظر", "نظر خود را بنویسید:", "", &ok);

    if (ok && !comment.trimmed().isEmpty()) {
        int rating = ratingSpinBox->value();
        emit addReviewRequested(currentBook.getId(), rating, comment);

        // به صورت موقت در کلاینت اضافه می‌کنیم (در عمل باید به سرور ارسال و رفرش شود)
        Review newReview(1, currentBook.getId(), rating, comment); // ۱ شناسه فرضی کاربر است
        currentBook.addReview(newReview);
        setBook(currentBook); // بروزرسانی صفحه نمایش داده‌ها
    }
}
