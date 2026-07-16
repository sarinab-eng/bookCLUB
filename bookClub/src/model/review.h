#ifndef REVIEW_H
#define REVIEW_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Review {
public:
    Review() = default;
    Review(const QString &bookTitle, const QString &userName, const QString &text, int rating);

    // Setters
    void setText(const QString &text) { m_text = text; }
    void setRating(int rating) { m_rating = rating; }
    void setBookTitle(const QString &title) { m_bookTitle = title; }
    void setUserId(int id) { m_userId = id; } // اضافه شده برای هماهنگی با Manager
    void setBookId(int id) { m_bookId = id; } // اضافه شده برای هماهنگی با Manager

    static Review fromJson(const QJsonObject &json);

    // Getters
    QString getBookTitle() const;
    QString getUserName() const;
    QString getText() const;
    int getRating() const;
    int getUserId() const { return m_userId; } // اضافه شده برای هماهنگی
    int getBookId() const { return m_bookId; } // اضافه شده برای هماهنگی
    QDateTime getDate() const;
    QJsonObject toJson() const;

private:
    QString m_bookTitle;
    QString m_userName;
    QString m_text;
    int m_rating;
    int m_userId = 0;
    int m_bookId = 0;
    QDateTime m_date;
};

#endif // REVIEW_H
