#include "Shoppingcart.h"

// مقداردهی اولیه متغیر استاتیک
ShoppingCart* ShoppingCart::instance = nullptr;

// پیاده‌سازی متد Singleton
ShoppingCart* ShoppingCart::getInstance() {
    if (!instance) {
        instance = new ShoppingCart();
    }
    return instance;
}

ShoppingCart::ShoppingCart() {
    activeDiscount = {"", 0.0, true, QDateTime::currentDateTime().addYears(10)};
}

void ShoppingCart::addItem(const Book &book) {
    for (const auto &item : items) {
        if (item.getId() == book.getId()) return;
    }
    items.append(book);
}

void ShoppingCart::removeItem(int bookId) {
    for (int i = 0; i < items.size(); ++i) {
        if (items[i].getId() == bookId) {
            items.removeAt(i);
            break;
        }
    }
}

bool ShoppingCart::hasBook(int bookId) const {
    for (const auto &item : items) {
        if (item.getId() == bookId) {
            return true;
        }
    }
    return false;
}

void ShoppingCart::clear() {
    items.clear();
    activeDiscount.couponCode = "";
    activeDiscount.value = 0.0;
}

void ShoppingCart::applyCoupon(const QString &code, double value, bool isPercentage, QDateTime expiry) {
    activeDiscount = {code, value, isPercentage, expiry};
}

double ShoppingCart::calculateSubtotal() const {
    double total = 0.0;
    for (const auto &item : items) {
        total += item.getPrice();
    }
    return total;
}

double ShoppingCart::calculateDiscountAmount() const {
    if (!activeDiscount.isValid() || activeDiscount.couponCode.isEmpty()) return 0.0;

    double subtotal = calculateSubtotal();
    if (activeDiscount.isPercentage) {
        return subtotal * (activeDiscount.value / 100.0);
    } else {
        return qMin(activeDiscount.value, subtotal);
    }
}

double ShoppingCart::calculateTotal() const {
    return calculateSubtotal() - calculateDiscountAmount();
}

QString ShoppingCart::generateReceipt() const {
    QString receipt = "--- Invoice ---\n";
    for (const auto &item : items) {
        receipt += QString("%1: %2$\n").arg(item.getTitle()).arg(item.getPrice());
    }
    receipt += "----------------\n";
    receipt += QString("Subtotal: %1$\n").arg(calculateSubtotal());
    if (!activeDiscount.couponCode.isEmpty() && activeDiscount.isValid()) {
        receipt += QString("Coupon (%1): -%2$\n").arg(activeDiscount.couponCode).arg(calculateDiscountAmount());
    }
    receipt += QString("Total Payable: %1$\n").arg(calculateTotal());
    return receipt;
}

QJsonObject ShoppingCart::toJson() const {
    QJsonObject json;
    QJsonArray itemsArray;
    for (const auto &book : items) {
        itemsArray.append(book.toJson());
    }
    json["items"] = itemsArray;
    json["couponCode"] = activeDiscount.couponCode;
    json["discountValue"] = activeDiscount.value;
    json["isPercentage"] = activeDiscount.isPercentage;
    json["expiryDate"] = activeDiscount.expiryDate.toString(Qt::ISODate);
    return json;
}

// نکته: این متد یک شیء جدید برمی‌گرداند، اگر می‌خواهید Singleton را آپدیت کنید
// باید منطق آن را به کپی کردن داده‌ها در getInstance تغییر دهید.
ShoppingCart ShoppingCart::fromJson(const QJsonObject &json) {
    ShoppingCart cart;
    QJsonArray itemsArray = json["items"].toArray();
    for (auto ref : itemsArray) {
        cart.items.append(Book::fromJson(ref.toObject()));
    }
    cart.activeDiscount.couponCode = json["couponCode"].toString();
    cart.activeDiscount.value = json["discountValue"].toDouble();
    cart.activeDiscount.isPercentage = json["isPercentage"].toBool();
    cart.activeDiscount.expiryDate = QDateTime::fromString(json["expiryDate"].toString(), Qt::ISODate);
    return cart;
}
