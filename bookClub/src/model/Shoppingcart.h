#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include "book.h"
#include <QVector>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

// ساختار کمکی برای مدیریت تخفیف‌های زمان‌دار (Bonus - صفحه 18)
struct DiscountInfo {
    QString couponCode;    // کد کوپن تخفیف برای بررسی صحت در سمت سرور
    double value;          // مقدار درصد یا مبلغ ثابت
    bool isPercentage;     // آیا درصدی است؟
    QDateTime expiryDate;  // تاریخ انقضا

    bool isValid() const {
        return QDateTime::currentDateTime() < expiryDate;
    }
};

class ShoppingCart {
private:
    QVector<Book> items;
    DiscountInfo activeDiscount; // تخفیف کلی اعمال شده روی سبد

public:
    ShoppingCart();

    void addItem(const Book &book);
    void removeItem(int bookId);
    bool hasBook(int bookId) const; // متد کمکی برای مدیریت دکمه‌های خرید در UI
    void clear();

    // اعمال تخفیف زمان‌دار (صفحه 18)
    void applyCoupon(const QString &code, double value, bool isPercentage, QDateTime expiry);

    // محاسبه‌های مالی (صفحه 9)
    double calculateSubtotal() const;
    double calculateDiscountAmount() const;
    double calculateTotal() const;

    // متدهای کمکی و سریالایزیشن
    const QVector<Book>& getItems() const { return items; }
    QString generateReceipt() const;
    QJsonObject toJson() const;
    static ShoppingCart fromJson(const QJsonObject &json);
};

#endif // SHOPPINGCART_H
