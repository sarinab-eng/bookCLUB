#include "PersonalLibrary.h"

PersonalLibrary::PersonalLibrary() {
    // سازنده کلاس
}

// اضافه کردن کتاب به لیست خریداری شده ها
void PersonalLibrary::addPurchasedBook(int bookId) {
    if (!purchasedBookIds.contains(bookId)) {
        purchasedBookIds.append(bookId);
    }
}

const QVector<int>& PersonalLibrary::getPurchasedBooks() const {
    return purchasedBookIds;
}

// بررسی وضعیت خرید کتاب
bool PersonalLibrary::hasPurchased(int bookId) const {
    return purchasedBookIds.contains(bookId);
}

// مدیریت لیست علاقه‌مندی‌ها (Favorites/Wishlist)
void PersonalLibrary::addToFavorites(int bookId) {
    // اصلاح منطقی: کاربر می‌تواند هر کتابی را (قبل یا بعد از خرید) به لیست علاقه‌مندی‌ها اضافه کند
    if (!favoriteBookIds.contains(bookId)) {
        favoriteBookIds.append(bookId);
    }
}

void PersonalLibrary::removeFromFavorites(int bookId) {
    favoriteBookIds.removeOne(bookId);
}

const QVector<int>& PersonalLibrary::getFavorites() const {
    return favoriteBookIds;
}

// --- مدیریت قفسه‌ها (Shelves) ---

void PersonalLibrary::createShelf(const QString &shelfName) {
    if (!shelfName.isEmpty() && !bookshelves.contains(shelfName)) {
        bookshelves[shelfName] = QVector<int>();
    }
}

void PersonalLibrary::deleteShelf(const QString &shelfName) {
    bookshelves.remove(shelfName);
}

void PersonalLibrary::renameShelf(const QString &oldName, const QString &newName) {
    if (!newName.isEmpty() && bookshelves.contains(oldName) && !bookshelves.contains(newName)) {
        // انتقال داده‌ها به کلید جدید و حذف کلید قدیمی
        bookshelves[newName] = bookshelves.take(oldName);
    }
}

void PersonalLibrary::addBookToShelf(const QString &shelfName, int bookId) {
    // طبق منطق پروژه: فقط کتاب‌های خریداری‌شده می‌توانند قفسه‌بندی شوند
    if (hasPurchased(bookId) && bookshelves.contains(shelfName)) {
        if (!bookshelves[shelfName].contains(bookId)) {
            bookshelves[shelfName].append(bookId);
        }
    }
}

void PersonalLibrary::removeBookFromShelf(const QString &shelfName, int bookId) {
    if (bookshelves.contains(shelfName)) {
        bookshelves[shelfName].removeOne(bookId);
    }
}

QStringList PersonalLibrary::getShelfNames() const {
    return bookshelves.keys();
}

QVector<int> PersonalLibrary::getBooksInShelf(const QString &shelfName) const {
    return bookshelves.value(shelfName, QVector<int>());
}

// --- متدهای سریال‌سازی (JSON Serialization) ---

QJsonObject PersonalLibrary::toJson() const {
    QJsonObject json;

    // تبدیل لیست‌های int به QJsonArray
    QJsonArray purchasedArr;
    for (int id : purchasedBookIds) purchasedArr.append(id);

    QJsonArray favArr;
    for (int id : favoriteBookIds) favArr.append(id);

    json["purchased"] = purchasedArr;
    json["favorites"] = favArr;

    // تبدیل Map قفسه‌ها به Object
    QJsonObject shelvesObj;
    for (auto it = bookshelves.begin(); it != bookshelves.end(); ++it) {
        QJsonArray shelfBooks;
        for (int id : it.value()) shelfBooks.append(id);
        shelvesObj[it.key()] = shelfBooks;
    }
    json["shelves"] = shelvesObj;

    return json;
}

PersonalLibrary PersonalLibrary::fromJson(const QJsonObject &json) {
    PersonalLibrary lib;

    // بازگردانی لیست خریدها
    QJsonArray purchasedArr = json["purchased"].toArray();
    for (auto val : purchasedArr) lib.purchasedBookIds.append(val.toInt());

    // بازگردانی علاقه‌مندی‌ها
    QJsonArray favArr = json["favorites"].toArray();
    for (auto val : favArr) lib.favoriteBookIds.append(val.toInt());

    // بازگردانی قفسه‌ها
    QJsonObject shelvesObj = json["shelves"].toObject();
    for (const QString &key : shelvesObj.keys()) {
        QVector<int> shelfBooks;
        QJsonArray shelfArr = shelvesObj[key].toArray();
        for (auto val : shelfArr) shelfBooks.append(val.toInt());
        lib.bookshelves[key] = shelfBooks;
    }

    return lib;
}
