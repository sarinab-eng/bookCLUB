#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>

struct Review {
    QString username;
    QString comment;
    int rating; // 1 to 5

    QJsonObject toJson() const {
        QJsonObject json;
        json["username"] = username;
        json["comment"] = comment;
        json["rating"] = rating;
        return json;
    }

    static Review fromJson(const QJsonObject &json) {
        return {json["username"].toString(), json["comment"].toString(), json["rating"].toInt()};
    }
};

class Book {
private:
    int id;
    QString title;
    QString author;
    QString publisherName;
    QString genre;
    QString description;
    QString filePath; // آدرس فایل کتاب یا متن آن برای ماژول مطالعه (صفحه 10)
    double price;
    int salesCount;   // برای آمار پرفروش‌ترین‌ها (صفحه 12)
    int lastReadPage; // ذخیره آخرین صفحه مطالعه شده (صفحه 10)

    QVector<Review> reviews;

public:
    Book();
    Book(int id, QString title, QString author, QString publisher, QString genre,
         QString description, QString filePath, double price);

    // Getters & Setters
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
    QVector<Review> getReviews() const;

    void setLastReadPage(int page);
    void incrementSales(int count = 1);

    // مدیریت نظرات و امتیازدهی (صفحه 8)
    void addOrUpdateReview(const QString &user, const QString &text, int score);
    void deleteReview(const QString &user);
    double getAverageRating() const;

    // سریال‌سازی برای بستر شبکه کلاینت-سرور
    QJsonObject toJson() const;
    static Book fromJson(const QJsonObject &json);
    void setTitle(const QString &title);
    void setPrice(double price);
    void setDescription(const QString &desc);


};

#endif // BOOK_H
