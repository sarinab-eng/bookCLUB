// bookdetailpage.h
#pragma once
#include <QWidget>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QListWidget>
#include "../network/Authmanager.h" // مسیر نسبی هدر AuthManager

class BookDetailPage : public QWidget
{
    Q_OBJECT
public:
    // تغییر سازنده برای دریافت AuthManager
    explicit BookDetailPage(AuthManager *authManager, QWidget *parent = nullptr);

    // دریافت اطلاعات کتاب (یک آرگومان)
    void setBookData(const QJsonObject &book);

    // تعیین نام کاربری فعال برای ثبت نقد
    void setCurrentUsername(const QString &username);

signals:
    void backRequested();
    void addToCartRequested(const QJsonObject &book);

private slots:
    void onPostReviewClicked();
    void onReviewsReceived(const QJsonArray &reviews, double averageRating, int reviewCount);

private:
    void setupUi();

    AuthManager *m_authManager;
    QString m_currentUsername;
    QJsonObject m_currentBook;

    // کنترل‌های اطلاعات کتاب
    QLabel *m_titleLabel;
    QLabel *m_authorLabel;
    QLabel *m_genreLabel;
    QLabel *m_priceLabel;
    QLabel *m_descriptionLabel;
    QPushButton *m_addToCartButton;
    QPushButton *m_backButton;

    // کنترل‌های سیستم نقد و امتیازدهی (تم صورتی QSS)
    QLabel *m_ratingStatsLabel;      // برای نمایش میانگین امتیاز و تعداد نظرات
    QListWidget *m_reviewsListWidget;// نمایش لیست کامنت‌ها
    QComboBox *m_ratingCombo;        // انتخاب امتیاز ۱ تا ۵
    QTextEdit *m_commentEdit;        // نوشتن متن نظر
    QPushButton *m_submitReviewBtn;  // دکمه ثبت نظر
};
