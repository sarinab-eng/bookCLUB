#include "Review.h"

Review::Review() : userId(0), username(""), bookId(0), rating(0), comment("") {}

Review::Review(int userId, const QString &username, int bookId, int rating, const QString &comment)
    : userId(userId), username(username), bookId(bookId), rating(rating), comment(comment) {
    // اطمینان از بازه مجاز امتیاز (1 تا 5 ستاره) طبق مستندات
    if (this->rating < 1) this->rating = 1;
    if (this->rating > 5) this->rating = 5;
}

void Review::setRating(int newRating) {
    if (newRating >= 1 && newRating <= 5) {
        rating = newRating;
    }
}

void Review::setComment(const QString &newComment) {
    comment = newComment;
}

QJsonObject Review::toJson() const {
    QJsonObject json;
    json["userId"] = userId;
    json["username"] = username;
    json["bookId"] = bookId;
    json["rating"] = rating;
    json["comment"] = comment;
    return json;
}

Review Review::fromJson(const QJsonObject &json) {
    return Review(
        json["userId"].toInt(),
        json["username"].toString(),
        json["bookId"].toInt(),
        json["rating"].toInt(),
        json["comment"].toString()
        );
}
