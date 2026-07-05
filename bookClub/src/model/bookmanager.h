#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H

#include "book.h"
#include <QVector>
#include <QString>

class BookManager {
private:
    QVector<Book> allBooks; // شبیه‌ساز دیتابیس کتاب‌ها در سرور

public:
    BookManager();

    // مدیریت کتاب‌ها توسط ناشر و ادمین (صفحات 4، 6 و 17)
    void addBook(const Book &book);
    bool editBook(int bookId, const QString &title, double price, const QString &desc);
    bool deleteBook(int bookId); // مشترک بین ادمین و ناشر مالک اثر

    // موتور جستجو و فیلتر پیشرفته (صفحه 8)
    QVector<Book> search(const QString &query) const;
    QVector<Book> filterByGenre(const QString &genre) const;
    Book* findBookById(int bookId);

    // آمار ناشر (صفحه 12)
    QVector<Book> getPublisherBooks(const QString &publisherName) const;
    QVector<Book> getBestSellers(const QString &publisherName) const;
    QVector<Book> getWorstSellers(const QString &publisherName) const;
    double getPublisherAverageRating(const QString &publisherName) const;

    // فایلینگ و ذخیره‌سازی محلی داده‌ها
    void loadFromFile(const QString &filePath);
    void saveToFile(const QString &filePath) const;

    // فیلترهای تکمیلی (صفحه 8)
    QVector<Book> filterByPriceRange(double minPrice, double maxPrice) const;
    QVector<Book> filterByMinRating(double minRating) const;

    // مدیریت وضعیت و فروش (صفحه 12 و 17)
    bool setBookAvailability(int bookId, bool status); // تغییر وضعیت موجود/ناموجود
    void recordSale(int bookId, int quantity = 1); // ثبت فروش برای آمار

};

#endif // BOOKMANAGER_H
