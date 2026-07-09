#include "PersonalLibrary.h"

PersonalLibrary::PersonalLibrary() {}

// طبق PDF: کتاب‌ها بعد از خرید موفق به اینجا اضافه می‌شوند
void PersonalLibrary::addPurchasedBook(int bookId) {
    if (!purchasedBookIds.contains(bookId)) {
        purchasedBookIds.append(bookId);
    }
}

const QVector<int>& PersonalLibrary::getPurchasedBooks() const {
    return purchasedBookIds;
}

// اصلاح خطای خط ۱۶ اسکرین‌شات (تطابق کامل با هدر)
bool PersonalLibrary::hasPurchased(int bookId) const {
    return purchasedBookIds.contains(bookId);
}

void PersonalLibrary::addToFavorites(int bookId) {
    // منطق: فقط کتابی که خریده شده می‌تواند محبوب باشد
    if (hasPurchased(bookId) && !favoriteBookIds.contains(bookId)) {
        favoriteBookIds.append(bookId);
    }
}

void PersonalLibrary::removeFromFavorites(int bookId) {
    favoriteBookIds.removeOne(bookId);
}

const QVector<int>& PersonalLibrary::getFavorites() const {
    return favoriteBookIds;
}

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
        bookshelves[newName] = bookshelves.take(oldName);
    }
}

void PersonalLibrary::addBookToShelf(const QString &shelfName, int bookId) {
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

// --- بخش سریال‌سازی برای هماهنگی با بقیه سیستم (User Storage) ---

QJsonObject PersonalLibrary::toJson() const {
    QJsonObject json;
    QJsonArray purchasedArr, favArr;
    for (int id : purchasedBookIds) purchasedArr.append(id);
    for (int id : favoriteBookIds) favArr.append(id);

    json["purchased"] = purchasedArr;
    json["favorites"] = favArr;

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
    QJsonArray purchasedArr = json["purchased"].toArray();
    for (auto val : purchasedArr) lib.purchasedBookIds.append(val.toInt());

    QJsonArray favArr = json["favorites"].toArray();
    for (auto val : favArr) lib.favoriteBookIds.append(val.toInt());

    QJsonObject shelvesObj = json["shelves"].toObject();
    for (const QString &key : shelvesObj.keys()) {
        QVector<int> shelfBooks;
        QJsonArray shelfArr = shelvesObj[key].toArray();
        for (auto val : shelfArr) shelfBooks.append(val.toInt());
        lib.bookshelves[key] = shelfBooks;
    }
    return lib;
}
