#ifndef PERSONALLIBRARY_H
#define PERSONALLIBRARY_H

#include <QVector>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>

class PersonalLibrary {
private:
    QVector<int> purchasedBookIds;                  // لیست شناسه کتاب‌های خریداری‌شده
    QVector<int> favoriteBookIds;                   // لیست شناسه علاقه‌مندی‌ها
    QMap<QString, QVector<int>> bookshelves;        // قفسه‌ها (نام -> لیست شناسه‌ها)

public:
    PersonalLibrary(); // سازنده

    // مدیریت کتاب‌های خریداری شده
    void addPurchasedBook(int bookId);
    const QVector<int>& getPurchasedBooks() const;
    bool hasPurchased(int bookId) const;

    // مدیریت علاقه‌مندی‌ها (Favorites)
    void addToFavorites(int bookId);
    void removeFromFavorites(int bookId);
    const QVector<int>& getFavorites() const;

    // مدیریت قفسه‌ها (Shelves)
    void createShelf(const QString &shelfName);
    void deleteShelf(const QString &shelfName);
    void renameShelf(const QString &oldName, const QString &newName);
    void addBookToShelf(const QString &shelfName, int bookId);
    void removeBookFromShelf(const QString &shelfName, int bookId);

    QStringList getShelfNames() const;
    QVector<int> getBooksInShelf(const QString &shelfName) const;

    // متدهای سریال‌سازی برای ذخیره در فایل یا ارسال به سرور
    QJsonObject toJson() const;
    static PersonalLibrary fromJson(const QJsonObject &json);
};

#endif // PERSONALLIBRARY_H
