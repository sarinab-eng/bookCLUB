#include "Review.h"

Review::Review(const QString &bookTitle, const QString &userName, const QString &text, int rating)
    : m_bookTitle(bookTitle), m_userName(userName), m_text(text), m_rating(rating)
{
    m_date = QDateTime::currentDateTime();
}

Review Review::fromJson(const QJsonObject &json) {
    Review r;
    r.m_bookTitle = json["bookTitle"].toString();
    r.m_userName = json["userName"].toString();
    r.m_text = json["text"].toString();
    r.m_rating = json["rating"].toInt();
    r.m_userId = json["userId"].toInt();
    r.m_bookId = json["bookId"].toInt();
    r.m_date = QDateTime::fromString(json["date"].toString(), Qt::ISODate);
    if (!r.m_date.isValid()) r.m_date = QDateTime::currentDateTime();
    return r;
}

QString Review::getBookTitle() const { return m_bookTitle; }
QString Review::getUserName() const { return m_userName; }
QString Review::getText() const { return m_text; }
int Review::getRating() const { return m_rating; }
QDateTime Review::getDate() const { return m_date; }

QJsonObject Review::toJson() const {
    QJsonObject json;
    json["bookTitle"] = m_bookTitle;
    json["userName"] = m_userName;
    json["text"] = m_text;
    json["rating"] = m_rating;
    json["userId"] = m_userId;
    json["bookId"] = m_bookId;
    json["date"] = m_date.toString(Qt::ISODate);
    return json;
}
