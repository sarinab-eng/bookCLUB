#include "bookdetailpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonArray>

BookDetailPage::BookDetailPage(AuthManager *authManager, QWidget *parent)
    : QWidget(parent), m_authManager(authManager)
{
    setupUi();

    // اتصال سیگنال دریافت نظرات از AuthManager به اسلات محلی صفحه
    if (m_authManager) {
        connect(m_authManager, &AuthManager::reviewsReceived,
                this, &BookDetailPage::onReviewsReceived);
    }
}

void BookDetailPage::setCurrentUsername(const QString &username)
{
    m_currentUsername = username;
}

void BookDetailPage::setBookData(const QJsonObject &book)
{
    m_currentBook = book;

    m_titleLabel->setText("Title: " + book.value("title").toString());
    m_authorLabel->setText("Author: " + book.value("author").toString());
    m_genreLabel->setText("Genre: " + book.value("genre").toString());
    m_priceLabel->setText(QString("Price: $%1").arg(book.value("price").toDouble()));
    m_descriptionLabel->setText("Description: " + book.value("description").toString());

    // تبدیل شناسه کتاب به عدد و ارسال درخواست دریافت نقدها از سرور
    // اگر شناسه به صورت string ذخیره شده، ابتدا آن را به int تبدیل می‌کنیم
    int bookId = 0;
    if (book.value("id").isDouble()) {
        bookId = book.value("id").toInt();
    } else {
        bookId = book.value("id").toString().toInt();
    }

    if (m_authManager && bookId > 0) {
        m_authManager->getReviews(bookId);
    }
}

// ساخت و چیدمان المان‌های صفحه به همراه بخش نقدها
void BookDetailPage::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ۱. بخش اطلاعات کتاب
    m_titleLabel = new QLabel(this);
    m_authorLabel = new QLabel(this);
    m_genreLabel = new QLabel(this);
    m_priceLabel = new QLabel(this);
    m_descriptionLabel = new QLabel(this);

    mainLayout->addWidget(m_titleLabel);
    mainLayout->addWidget(m_authorLabel);
    mainLayout->addWidget(m_genreLabel);
    mainLayout->addWidget(m_priceLabel);
    mainLayout->addWidget(m_descriptionLabel);

    // ۲. بخش آمار امتیازدهی کتاب
    m_ratingStatsLabel = new QLabel("Rating: N/A (0 reviews)", this);
    m_ratingStatsLabel->setStyleSheet("font-weight: bold; color: #D81B60;");
    mainLayout->addWidget(m_ratingStatsLabel);

    // ۳. لیست کامنت‌ها
    m_reviewsListWidget = new QListWidget(this);
    m_reviewsListWidget->setStyleSheet(
        "QListWidget { border: 1px solid #FF8DA1; background-color: #FFF0F2; border-radius: 5px; }"
        );
    mainLayout->addWidget(m_reviewsListWidget);

    // ۴. فرم ارسال نظر جدید (امتیاز و متن نظر)
    QHBoxLayout *formLayout = new QHBoxLayout();
    m_ratingCombo = new QComboBox(this);
    for (int i = 1; i <= 5; ++i) {
        m_ratingCombo->addItem(QString::number(i), i);
    }

    m_commentEdit = new QTextEdit(this);
    m_commentEdit->setPlaceholderText("Write your review here...");
    m_commentEdit->setMaximumHeight(60);
    m_commentEdit->setStyleSheet("border: 1px solid #FF8DA1; border-radius: 5px;");

    m_submitReviewBtn = new QPushButton("Submit Review", this);
    m_submitReviewBtn->setStyleSheet(
        "QPushButton { background-color: #FF8DA1; color: white; border-radius: 5px; padding: 5px 15px; font-weight: bold; }"
        "QPushButton:hover { background-color: #FF6F88; }"
        );

    formLayout->addWidget(new QLabel("Rating:"));
    formLayout->addWidget(m_ratingCombo);
    formLayout->addWidget(m_commentEdit);
    formLayout->addWidget(m_submitReviewBtn);
    mainLayout->addLayout(formLayout);

    // ۵. دکمه‌های بازگشت و خرید
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_backButton = new QPushButton("Back", this);
    m_addToCartButton = new QPushButton("Add to Cart", this);

    // استایل صورتی برای دکمه‌ها
    QString btnStyle = "QPushButton { background-color: #FF8DA1; color: white; border-radius: 5px; padding: 8px; font-weight: bold; } "
                       "QPushButton:hover { background-color: #FF6F88; }";
    m_backButton->setStyleSheet(btnStyle);
    m_addToCartButton->setStyleSheet(btnStyle);

    buttonLayout->addWidget(m_backButton);
    buttonLayout->addWidget(m_addToCartButton);
    mainLayout->addLayout(buttonLayout);

    // سیگنال‌های کلیک
    connect(m_backButton, &QPushButton::clicked, this, &BookDetailPage::backRequested);
    connect(m_addToCartButton, &QPushButton::clicked, [this]() {
        emit addToCartRequested(m_currentBook);
    });
    connect(m_submitReviewBtn, &QPushButton::clicked, this, &BookDetailPage::onPostReviewClicked);
}

// ثبت نظر جدید
void BookDetailPage::onPostReviewClicked()
{
    if (m_currentUsername.isEmpty()) {
        // نام کاربری تنظیم نشده است
        return;
    }

    int bookId = 0;
    if (m_currentBook.value("id").isDouble()) {
        bookId = m_currentBook.value("id").toInt();
    } else {
        bookId = m_currentBook.value("id").toString().toInt();
    }

    int rating = m_ratingCombo->currentData().toInt();
    QString comment = m_commentEdit->toPlainText().trimmed();

    if (m_authManager && bookId > 0 && !comment.isEmpty()) {
        m_authManager->postReview(m_currentUsername, bookId, rating, comment);
        m_commentEdit->clear();
    }
}

// اسلات دریافت لیست نقدها و آمار امتیاز کتاب
void BookDetailPage::onReviewsReceived(const QJsonArray &reviews, double averageRating, int reviewCount)
{
    m_reviewsListWidget->clear();

    // به‌روزرسانی هدر میانگین امتیاز
    m_ratingStatsLabel->setText(QString("Rating: %1 / 5.0 (%2 reviews)")
                                    .arg(QString::number(averageRating, 'f', 1))
                                    .arg(reviewCount));

    for (const QJsonValue &val : reviews) {
        QJsonObject reviewObj = val.toObject();
        QString username = reviewObj.value("username").toString();
        int rating = reviewObj.value("rating").toInt();
        QString comment = reviewObj.value("comment").toString();

        QString displayFormat = QString("[%1] - (Rating: %2/5)\n%3")
                                    .arg(username)
                                    .arg(rating)
                                    .arg(comment);

        m_reviewsListWidget->addItem(displayFormat);
    }
}
