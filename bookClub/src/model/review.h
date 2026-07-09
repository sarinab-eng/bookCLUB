#ifndef REVIEW_H
#define REVIEW_H

#include <QString>
#include <QJsonObject>

class Review {
private:
    int userId;
    QString username; // اضافه شده برای نمایش نام نویسنده نظر در UI
    int bookId;
    int rating;       // باید بین 1 تا 5 باشد
    QString comment;

public:
    Review();
    Review(int userId, const QString &username, int bookId, int rating, const QString &comment);

    // Getters
    int getUserId() const { return userId; }
    QString getUsername() const { return username; }
    int getBookId() const { return bookId; }
    int getRating() const { return rating; }
    QString getComment() const { return comment; }

    // Setters (برای امکان ویرایش نظر توسط کاربر طبق مستندات)
    void setRating(int newRating);
    void setComment(const QString &newComment);
    void setUsername(const QString &newUsername) { username = newUsername; }

    // متدهای تبدیل JSON برای ارتباط شبکه و ذخیره‌سازی
    QJsonObject toJson() const;
    static Review fromJson(const QJsonObject &json);
};

#endif // REVIEW_H
