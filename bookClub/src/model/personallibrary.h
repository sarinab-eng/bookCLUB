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
    QMap<QString, QVector<int>> bookshelves;  // قفسه‌بندی (نام قفسه -> لیست شناسه کتاب‌ها)

public:
    PersonalLibrary();

    // مدیریت کتاب‌های خریداری شده (صفحه ۹ PDF)
    void addPurchasedBook(int bookId);
    const QVector<int>& getPurchasedBooks() const;
    bool hasPurchased(int bookId) const;

    // مدیریت علاقه‌مندی‌ها
    void addToFavorites(int bookId);
    void removeFromFavorites(int bookId);
    const QVector<int>& getFavorites() const;

    // مدیریت قفسه‌ها و سازماندهی (صفحه ۱۰ PDF)
    void createShelf(const QString &shelfName);
    void deleteShelf(const QString &shelfName);
    void renameShelf(const QString &oldName, const QString &newName);
    void addBookToShelf(const QString &shelfName, int bookId);
    void removeBookFromShelf(const QString &shelfName, int bookId);

    // متدهای کمکی برای UI
    QStringList getShelfNames() const;
    QVector<int> getBooksInShelf(const QString &shelfName) const;

    // متدهای تبدیل به JSON برای ذخیره در فایل کاربران
    QJsonObject toJson() const;
    static PersonalLibrary fromJson(const QJsonObject &json);
};

#endif // PERSONALLIBRARY_H
