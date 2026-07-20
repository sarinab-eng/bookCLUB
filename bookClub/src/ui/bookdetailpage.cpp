#include "bookdetailpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QMessageBox>
#include <QPixmap>

BookDetailPage::BookDetailPage(AuthManager *authManager, QWidget *parent)
    : QWidget(parent), m_authManager(authManager)
{
    setupUi();

    // اتصال سیگنال دریافت نظرات از AuthManager به اسلات محلی صفحه
    if (m_authManager) {
        connect(m_authManager, &AuthManager::reviewsReceived,
                this, &BookDetailPage::onReviewsReceived);
        connect(m_authManager, &AuthManager::reviewPosted,
                this, &BookDetailPage::onReviewPosted);
        connect(m_authManager, &AuthManager::reviewEdited,
                this, &BookDetailPage::onReviewEdited);
        connect(m_authManager, &AuthManager::reviewDeleted,
                this, &BookDetailPage::onReviewDeleted);
    }
}

void BookDetailPage::setCurrentUsername(const QString &username)
{
    m_currentUsername = username;
}

void BookDetailPage::setBookData(const QJsonObject &book)
{
    m_currentBook = book;
    resetReviewForm();

    QString coverPath = book.value("coverImage").toString();
    QPixmap pixmap(coverPath);
    if (!coverPath.isEmpty() && !pixmap.isNull()) {
        m_coverLabel->setPixmap(pixmap.scaled(m_coverLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_coverLabel->setText(QString());
        m_coverLabel->setStyleSheet("border: none;");
    } else {
        m_coverLabel->setPixmap(QPixmap());
        m_coverLabel->setText("📖");
        m_coverLabel->setStyleSheet("border: none; background-color: #FFF0F5; border-radius: 8px; font-size: 60px;");
    }

    m_titleLabel->setText("Title: " + book.value("title").toString());
    m_authorLabel->setText("Author: " + book.value("author").toString());
    m_genreLabel->setText("Genre: " + book.value("genre").toString());
    m_priceLabel->setText(QString("Price: $%1").arg(book.value("price").toDouble()));
    m_descriptionLabel->setText("Description: " + book.value("description").toString());

    // شناسه‌ی کتاب‌ها رشته‌ای است (مثل "b001")، نه عددی
    QString bookId = book.value("id").toString();

    if (m_authManager && !bookId.isEmpty()) {
        m_authManager->getReviews(bookId);
    }
}

void BookDetailPage::resetReviewForm()
{
    m_myReviewId.clear();
    m_ratingCombo->setCurrentIndex(0);
    m_commentEdit->clear();
    m_submitReviewBtn->setText("ثبت نظر");
    m_deleteReviewBtn->hide();
}

// ساخت و چیدمان المان‌های صفحه به همراه بخش نقدها
void BookDetailPage::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ۰. عکس روی جلد
    m_coverLabel = new QLabel(this);
    m_coverLabel->setFixedSize(180, 220);
    m_coverLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_coverLabel, 0, Qt::AlignHCenter);

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

    // ۴. فرم ارسال/ویرایش نظر (امتیاز و متن نظر)
    QHBoxLayout *formLayout = new QHBoxLayout();
    m_ratingCombo = new QComboBox(this);
    for (int i = 1; i <= 5; ++i) {
        m_ratingCombo->addItem(QString::number(i), i);
    }

    m_commentEdit = new QTextEdit(this);
    m_commentEdit->setPlaceholderText("Write your review here...");
    m_commentEdit->setMaximumHeight(60);
    m_commentEdit->setStyleSheet("border: 1px solid #FF8DA1; border-radius: 5px;");

    m_submitReviewBtn = new QPushButton("ثبت نظر", this);
    m_submitReviewBtn->setStyleSheet(
        "QPushButton { background-color: #FF8DA1; color: white; border-radius: 5px; padding: 5px 15px; font-weight: bold; }"
        "QPushButton:hover { background-color: #FF6F88; }"
        );

    m_deleteReviewBtn = new QPushButton("حذف نظر", this);
    m_deleteReviewBtn->setStyleSheet(
        "QPushButton { background-color: #841C26; color: white; border-radius: 5px; padding: 5px 15px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A8201A; }"
        );
    m_deleteReviewBtn->hide();

    formLayout->addWidget(new QLabel("Rating:"));
    formLayout->addWidget(m_ratingCombo);
    formLayout->addWidget(m_commentEdit);
    formLayout->addWidget(m_submitReviewBtn);
    formLayout->addWidget(m_deleteReviewBtn);
    mainLayout->addLayout(formLayout);

    // ۵. دکمه‌های بازگشت، خرید و ذخیره برای بعد
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_backButton = new QPushButton("Back", this);
    m_addToCartButton = new QPushButton("Add to Cart", this);
    m_saveForLaterButton = new QPushButton("ذخیره برای بعد", this);

    // استایل صورتی برای دکمه‌ها
    QString btnStyle = "QPushButton { background-color: #FF8DA1; color: white; border-radius: 5px; padding: 8px; font-weight: bold; } "
                       "QPushButton:hover { background-color: #FF6F88; }";
    m_backButton->setStyleSheet(btnStyle);
    m_addToCartButton->setStyleSheet(btnStyle);
    m_saveForLaterButton->setStyleSheet(btnStyle);

    buttonLayout->addWidget(m_backButton);
    buttonLayout->addWidget(m_addToCartButton);
    buttonLayout->addWidget(m_saveForLaterButton);
    mainLayout->addLayout(buttonLayout);

    // سیگنال‌های کلیک
    connect(m_backButton, &QPushButton::clicked, this, &BookDetailPage::backRequested);
    connect(m_addToCartButton, &QPushButton::clicked, [this]() {
        emit addToCartRequested(m_currentBook);
    });
    connect(m_saveForLaterButton, &QPushButton::clicked, [this]() {
        emit saveForLaterRequested(m_currentBook);
    });
    connect(m_submitReviewBtn, &QPushButton::clicked, this, &BookDetailPage::onPostReviewClicked);
    connect(m_deleteReviewBtn, &QPushButton::clicked, this, &BookDetailPage::onDeleteReviewClicked);
}

// ثبت نظر جدید یا ویرایش نظر قبلیِ همین کاربر برای این کتاب
void BookDetailPage::onPostReviewClicked()
{
    if (m_currentUsername.isEmpty()) {
        // نام کاربری تنظیم نشده است
        return;
    }

    QString bookId = m_currentBook.value("id").toString();
    int rating = m_ratingCombo->currentData().toInt();
    QString comment = m_commentEdit->toPlainText().trimmed();

    if (!m_authManager || bookId.isEmpty() || comment.isEmpty()) return;

    if (m_myReviewId.isEmpty())
        m_authManager->postReview(m_currentUsername, bookId, rating, comment);
    else
        m_authManager->editReview(m_currentUsername, m_myReviewId, rating, comment);
}

void BookDetailPage::onDeleteReviewClicked()
{
    if (m_myReviewId.isEmpty() || m_currentUsername.isEmpty()) return;

    if (QMessageBox::question(this, "حذف نظر", "آیا از حذف نظر خود مطمئن هستید؟")
        != QMessageBox::Yes) return;

    m_authManager->deleteReview(m_currentUsername, m_myReviewId);
}

void BookDetailPage::onReviewPosted(bool success, const QString &message)
{
    if (!success) QMessageBox::warning(this, "خطا", message);
    // نمایش نظر تازه‌ثبت‌شده از طریق broadcast سرور (reviews_list) روی همه‌ی کلاینت‌ها انجام می‌شود
}

void BookDetailPage::onReviewEdited(bool success, const QString &message)
{
    if (!success) QMessageBox::warning(this, "خطا", message);
}

void BookDetailPage::onReviewDeleted(bool success, const QString &message)
{
    if (success) resetReviewForm();
    else QMessageBox::warning(this, "خطا", message);
}

// اسلات دریافت لیست نقدها؛ چون سیگنال AuthManager::reviewsReceived فقط آرایه نقدها رو
// می‌فرسته (نه میانگین امتیاز و تعدادشون)، این‌ها رو همینجا از روی آرایه محاسبه می‌کنیم.
// این پاسخ ممکن است broadcast سرور برای کتاب دیگری باشد، پس اول شناسه رو چک می‌کنیم.
void BookDetailPage::onReviewsReceived(const QString &bookId, const QJsonArray &reviews)
{
    if (bookId != m_currentBook.value("id").toString()) return;

    m_reviewsListWidget->clear();

    double totalRating = 0;
    QString myReviewId;
    int myRating = 0;
    QString myComment;

    for (const QJsonValue &val : reviews) {
        QJsonObject reviewObj = val.toObject();
        totalRating += reviewObj.value("rating").toInt();

        if (!m_currentUsername.isEmpty() && reviewObj.value("username").toString() == m_currentUsername) {
            myReviewId = reviewObj.value("review_id").toString();
            myRating = reviewObj.value("rating").toInt();
            myComment = reviewObj.value("comment").toString();
        }
    }
    int reviewCount = reviews.size();
    double averageRating = reviewCount > 0 ? totalRating / reviewCount : 0.0;

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

    // اگر کاربر فعلی قبلاً برای این کتاب نظر ثبت کرده، فرم رو به حالت ویرایش ببر
    m_myReviewId = myReviewId;
    if (!m_myReviewId.isEmpty()) {
        m_ratingCombo->setCurrentIndex(myRating - 1);
        m_commentEdit->setPlainText(myComment);
        m_submitReviewBtn->setText("ویرایش نظر");
        m_deleteReviewBtn->show();
    } else {
        m_submitReviewBtn->setText("ثبت نظر");
        m_deleteReviewBtn->hide();
    }
}
