#ifndef SHOPPINGCART_H
#define SHOPPINGCART_H

#include "book.h"
#include <QVector>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

// ساختار کمکی برای مدیریت تخفیف‌های زمان‌دار (Bonus - صفحه 18)
struct DiscountInfo {
    double value;          // مقدار (درصد یا مبلغ ثابت)
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
    ShoppingCart() {
        activeDiscount = {0.0, true, QDateTime::currentDateTime().addYears(10)};
    }

    void addItem(const Book &book) {
        for (const auto &item : items) {
            if (item.getId() == book.getId()) return;
        }
        items.append(book);
    }

    void removeItem(int bookId) {
        for (int i = 0; i < items.size(); ++i) {
            if (items[i].getId() == bookId) {
                items.removeAt(i);
                break;
            }
        }
    }

    void clear() {
        items.clear();
        activeDiscount.value = 0.0;
    }

    // اعمال تخفیف زمان‌دار (صفحه 18)
    void applyCoupon(double value, bool isPercentage, QDateTime expiry) {
        activeDiscount = {value, isPercentage, expiry};
    }

    // محاسبه مجموع قیمت قبل از تخفیف
    double calculateSubtotal() const {
        double total = 0.0;
        for (const auto &item : items) {
            total += item.getPrice();
        }
        return total;
    }

    // محاسبه دقیق میزان سود کاربر از تخفیف (صفحه 9)
    double calculateDiscountAmount() const {
        if (!activeDiscount.isValid()) return 0.0;

        double subtotal = calculateSubtotal();
        if (activeDiscount.isPercentage) {
            return subtotal * (activeDiscount.value / 100.0);
        } else {
            // تخفیف مبلغ ثابت نباید بیشتر از قیمت کل باشد
            return qMin(activeDiscount.value, subtotal);
        }
    }

    // مبلغ نهایی قابل پرداخت (صفحه 9)
    double calculateTotal() const {
        return calculateSubtotal() - calculateDiscountAmount();
    }

    // صدور فاکتور متنی برای نمایش در UI (الزام گزارش‌گیری)
    QString generateReceipt() const {
        QString receipt = "--- Invoice ---\n";
        for (const auto &item : items) {
            receipt += QString("%1: %2$\n").arg(item.getTitle()).arg(item.getPrice());
        }
        receipt += "----------------\n";
        receipt += QString("Subtotal: %1$\n").arg(calculateSubtotal());
        receipt += QString("Discount: -%1$\n").arg(calculateDiscountAmount());
        receipt += QString("Total Payable: %1$\n").arg(calculateTotal());
        return receipt;
    }

    // تبدیل به JSON برای ارسال به سمت هم‌تیمی (بخش سرور و خرید نهایی)
    QJsonObject toJson() const {
        QJsonObject json;
        QJsonArray itemsArray;
        for (const auto &book : items) {
            itemsArray.append(book.toJson());
        }
        json["items"] = itemsArray;
        json["discountValue"] = activeDiscount.value;
        json["isPercentage"] = activeDiscount.isPercentage;
        json["expiryDate"] = activeDiscount.expiryDate.toString(Qt::ISODate);
        return json;
    }

    static ShoppingCart fromJson(const QJsonObject &json) {
        ShoppingCart cart;
        QJsonArray itemsArray = json["items"].toArray();
        for (auto ref : itemsArray) {
            cart.items.append(Book::fromJson(ref.toObject()));
        }
        cart.activeDiscount.value = json["discountValue"].toDouble();
        cart.activeDiscount.isPercentage = json["isPercentage"].toBool();
        cart.activeDiscount.expiryDate = QDateTime::fromString(json["expiryDate"].toString(), Qt::ISODate);
        return cart;
    }
};

#endif // SHOPPINGCART_H
