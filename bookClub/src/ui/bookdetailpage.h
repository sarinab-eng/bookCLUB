// bookdetailpage.h
#pragma once
#include <QWidget>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QListWidget>
#include "../network/Authmanager.h"

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
    void saveForLaterRequested(const QJsonObject &book);

private slots:
    void onPostReviewClicked();
    void onDeleteReviewClicked();
    void onReviewsReceived(const QString &bookId, const QJsonArray &reviews);
    void onReviewPosted(bool success, const QString &message);
    void onReviewEdited(bool success, const QString &message);
    void onReviewDeleted(bool success, const QString &message);

private:
    void setupUi();
    void resetReviewForm();

    AuthManager *m_authManager;
    QString m_currentUsername;
    QJsonObject m_currentBook;
    QString m_myReviewId; // خالی یعنی کاربر فعلی هنوز نظری برای این کتاب ثبت نکرده

    // کنترل‌های اطلاعات کتاب
    QLabel *m_coverLabel;
    QLabel *m_titleLabel;
    QLabel *m_authorLabel;
    QLabel *m_genreLabel;
    QLabel *m_priceLabel;
    QLabel *m_descriptionLabel;
    QPushButton *m_addToCartButton;
    QPushButton *m_saveForLaterButton;
    QPushButton *m_backButton;

    // کنترل‌های سیستم نقد و امتیازدهی (تم صورتی QSS)
    QLabel *m_ratingStatsLabel;      // برای نمایش میانگین امتیاز و تعداد نظرات
    QListWidget *m_reviewsListWidget;// نمایش لیست کامنت‌ها
    QComboBox *m_ratingCombo;        // انتخاب امتیاز ۱ تا ۵
    QTextEdit *m_commentEdit;        // نوشتن متن نظر
    QPushButton *m_submitReviewBtn;  // دکمه ثبت/ویرایش نظر
    QPushButton *m_deleteReviewBtn;  // دکمه حذف نظر (فقط وقتی کاربر نظر قبلی دارد نمایش داده می‌شود)
};
