
#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include "Review.h"
#include "ReviewManager.h"

class Book {
private:
    int id;
    QString title;
    QString author;
    QString publisherName;
    QString genre;
    QString description;
    QString filePath;            // آدرس فایل کتاب یا متن آن برای ماژول مطالعه
    double price;
    int salesCount;              // برای آمار پرفروش‌ترین‌ها
    int lastReadPage;            // ذخیره آخرین صفحه مطالعه شده
    ReviewManager reviewManager; // مدیریت نظرات از طریق ReviewManager
    bool available;              // وضعیت موجودی کتاب

public:
    Book();
    Book(int id, const QString &title, const QString &author, const QString &publisher,
         const QString &genre, const QString &description, const QString &filePath, double price);

    // Getters
    int getId() const;
    QString getTitle() const;
    QString getAuthor() const;
    QString getPublisher() const;
    QString getGenre() const;
    QString getDescription() const;
    QString getFilePath() const;
    double getPrice() const;
    int getSalesCount() const;
    int getLastReadPage() const;
    bool getIsAvailable() const;

    // گرفتن لیست نظرات و میانگین امتیازات با استفاده از ReviewManager
    QVector<Review> getReviews() const { return reviewManager.getAllReviews(); }
    double getRating() const { return reviewManager.getAverageRating(); }

    // Setters
    void setTitle(const QString &title);
    void setPrice(double price);
    void setDescription(const QString &desc);
    void setLastReadPage(int page);
    void setIsAvailable(bool status);
    void setSalesCount(int count);
    void incrementSales(int count = 1);

    // مدیریت نظرات
    void addReview(const Review &r) { reviewManager.addReview(r); }
    void deleteReview(int userId) { reviewManager.removeReview(userId); }

    // سریال‌سازی برای بستر شبکه کلاینت-سرور
    QJsonObject toJson() const;
    static Book fromJson(const QJsonObject &json);
};

#endif // BOOK_H
