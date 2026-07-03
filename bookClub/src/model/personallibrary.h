#ifndef PERSONALLIBRARY_H
#define PERSONALLIBRARY_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

class PersonalLibrary {
private:
    QVector<int> purchasedBookIds;            // شناسه کتاب‌های خریداری شده
    QVector<int> favoriteBookIds;             // لیست علاقه‌مندی‌ها
    QMap<QString, QVector<int>> bookshelves;  // قفسه‌ها (نام قفسه -> لیست شناسه کتاب‌ها)

public:
    PersonalLibrary() {}

    // افزودن کتاب خریداری شده به کتابخانه
    void addPurchasedBook(int bookId) {
        if (!purchasedBookIds.contains(bookId)) {
            purchasedBookIds.append(bookId);
        }
    }

    const QVector<int>& getPurchasedBooks() const { return purchasedBookIds; }

    // مدیریت علاقه‌مندی‌ها (بررسی خرید الزامی است)
    void addToFavorites(int bookId) {
        if (purchasedBookIds.contains(bookId) && !favoriteBookIds.contains(bookId)) {
            favoriteBookIds.append(bookId);
        }
    }

    void removeFromFavorites(int bookId) {
        favoriteBookIds.removeOne(bookId);
    }

    const QVector<int>& getFavorites() const { return favoriteBookIds; }

    // مدیریت قفسه‌ها (صفحه ۱۰ مستند)
    void createShelf(const QString &shelfName) {
        if (!shelfName.isEmpty() && !bookshelves.contains(shelfName)) {
            bookshelves[shelfName] = QVector<int>();
        }
    }

    void deleteShelf(const QString &shelfName) {
        bookshelves.remove(shelfName);
    }

    void renameShelf(const QString &oldName, const QString &newName) {
        if (!newName.isEmpty() && bookshelves.contains(oldName) && !bookshelves.contains(newName)) {
            bookshelves[newName] = bookshelves.take(oldName);
        }
    }

    // اضافه کردن کتاب به قفسه (بررسی اینکه کتاب حتماً خریداری شده باشد)
    void addBookToShelf(const QString &shelfName, int bookId) {
        if (purchasedBookIds.contains(bookId) && bookshelves.contains(shelfName)) {
            if (!bookshelves[shelfName].contains(bookId)) {
                bookshelves[shelfName].append(bookId);
            }
        }
    }

    void removeBookFromShelf(const QString &shelfName, int bookId) {
        if (bookshelves.contains(shelfName)) {
            bookshelves[shelfName].removeOne(bookId);
        }
    }

    // انتقال کتاب بین دو قفسه (الزام صریح صفحه ۱۰ مستند)
    bool moveBookBetweenShelves(int bookId, const QString &fromShelf, const QString &toShelf) {
        if (bookshelves.contains(fromShelf) && bookshelves.contains(toShelf)) {
            if (bookshelves[fromShelf].contains(bookId)) {
                bookshelves[fromShelf].removeOne(bookId);
                if (!bookshelves[toShelf].contains(bookId)) {
                    bookshelves[toShelf].append(bookId);
                }
                return true;
            }
        }
        return false;
    }

    // متدهای کمکی برای استفاده در UI
    QStringList getShelfNames() const {
        return bookshelves.keys();
    }

    QVector<int> getBooksInShelf(const QString &shelfName) const {
        return bookshelves.value(shelfName, QVector<int>());
    }

    // ذخیره و بازیابی اطلاعات به صورت JSON
    QJsonObject toJson() const {
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

    static PersonalLibrary fromJson(const QJsonObject &json) {
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
};

#endif // PERSONALLIBRARY_H
