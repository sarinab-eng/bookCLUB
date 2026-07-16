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
    QString filePath;
    double price;
    int salesCount;
    int lastReadPage;
    int m_stock;                  // موجودی انبار
    bool available;
    ReviewManager reviewManager;

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
    int getStock() const;

    // گرفتن لیست نظرات و میانگین امتیازات
    QVector<Review> getReviews() const { return reviewManager.getAllReviews(); }
    double getRating() const { return reviewManager.getAverageRating(); }

    // Setters
    void setTitle(const QString &title);
    void setPrice(double price);
    void setDescription(const QString &desc);
    void setLastReadPage(int page);
    void setIsAvailable(bool status);
    void setSalesCount(int count);
    void setStock(int stock);
    void incrementSales(int count = 1);
    void setId(int id) { this->id = id; }
    void setAuthor(const QString &author) { this->author = author; }
    void setGenre(const QString &genre) { this->genre = genre; }
    void setPublisher(const QString &publisher) { this->publisherName = publisher; }
    void setFilePath(const QString &path) { this->filePath = path; }
    void setDiscountPercentage(int discount) { /* اگر فیلدش را اضافه کردید اینجا ست کنید */ }


    // مدیریت نظرات
    void addReview(const Review &r) { reviewManager.addReview(r); }
    void deleteReview(int userId) { reviewManager.removeReview(userId); }

    // متدهای تبدیل JSON (یکبار تعریف شده)
    QJsonObject toJson() const;
    static Book fromJson(const QJsonObject &json);
};

#endif // BOOK_H
