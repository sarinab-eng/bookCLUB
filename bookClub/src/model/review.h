#ifndef REVIEW_H
#define REVIEW_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Review {
private:
    int userId;          // شناسه کاربری که نظر داده
    QString username;    // نام نمایشی کاربر
    int rating;          // امتیاز (بین 1 تا 5)
    QString comment;     // متن نظر
    QDateTime createdAt; // تاریخ و زمان ثبت نظر

public:
    Review() : userId(0), rating(0), createdAt(QDateTime::currentDateTime()) {}

    Review(int uId, QString uName, int rate, QString msg)
        : userId(uId), username(uName), rating(rate), comment(msg), createdAt(QDateTime::currentDateTime()) {
        // اطمینان از اینکه امتیاز در بازه 1 تا 5 باشد
        if (rating < 1) rating = 1;
        if (rating > 5) rating = 5;
    }

    // Getters
    int getUserId() const { return userId; }
    QString getUsername() const { return username; }
    int getRating() const { return rating; }
    QString getComment() const { return comment; }
    QDateTime getCreatedAt() const { return createdAt; } // اضافه شده برای دسترسی مستقیم به تاریخ
    QString getFormattedDate() const { return createdAt.toString("yyyy/MM/dd HH:mm"); }

    // Setters (توابع جدید اضافه شده جهت رفع خطای ویرایش نظر در کلاس کتاب)
    void setComment(const QString &msg) {
        comment = msg;
    }

    void setRating(int rate) {
        if (rate < 1) rating = 1;
        else if (rate > 5) rating = 5;
        else rating = rate;
    }

    // تبدیل به JSON برای ارسال در شبکه (مورد نیاز هم‌گروهی شما)
    QJsonObject toJson() const {
        QJsonObject json;
        json["userId"] = userId;
        json["username"] = username;
        json["rating"] = rating;
        json["comment"] = comment;
        json["createdAt"] = createdAt.toString(Qt::ISODate);
        return json;
    }

    static Review fromJson(const QJsonObject &json) {
        Review r;
        r.userId = json["userId"].toInt();
        r.username = json["username"].toString();
        r.rating = json["rating"].toInt();
        r.comment = json["comment"].toString();
        r.createdAt = QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate);
        return r;
    }
};

#endif // REVIEW_H
