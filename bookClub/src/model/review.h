#ifndef REVIEW_H
#define REVIEW_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Review {
public:
    Review() = default; // سازنده پیش‌فرض برای استفاده در شبکه
    Review(const QString &bookTitle, const QString &userName, const QString &text, int rating);
    void setText(const QString &text) { m_text = text; }
    void setRating(int rating) { m_rating = rating; }
    void setBookTitle(const QString &title) { m_bookTitle = title; }

    // متد جدید برای تبدیل JSON به شیء Review
    static Review fromJson(const QJsonObject &json);

    // Getters
    QString getBookTitle() const;
    QString getUserName() const;
    QString getText() const;
    int getRating() const;
    QDateTime getDate() const;

private:
    QString m_bookTitle;
    QString m_userName;
    QString m_text;
    int m_rating;
    QDateTime m_date;

};

#endif // REVIEW_H
