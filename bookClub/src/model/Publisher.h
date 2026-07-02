#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "User.h" // دقت به کوچک یا بزرگ بودن حرف U
#include <QString>
#include <QVector> // اضافه کردن این هدر به جای QList

class Publisher : public User {
    Q_OBJECT
public:
    // فقط معرفی سازنده (بدون بدنه {})
    explicit Publisher(QObject *parent = nullptr);

    QString getRole() const override { return "Publisher"; }

    QString brandName;             // نام تجاری انتشارات
    bool isApproved;               // وضعیت تایید توسط ادمین
    QVector<int> publishedBookIds; // لیست کتاب‌های منتشر شده توسط این ناشر

    // متدهای اختصاصی
    void publishNewBook(int bookId);
    void viewSalesStatistics();
};

#endif // PUBLISHER_H