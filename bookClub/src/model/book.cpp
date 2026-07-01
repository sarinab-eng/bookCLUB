#include "Book.h"

Book::Book(QObject *parent)
    : QObject(parent), m_id(0), m_price(0.0), m_discountPercent(0.0), m_averageRating(0.0) {}

Book::Book(int id, const QString &title, const QString &author, const QString &publisher,
           double price, double discountPercent, const QString &pdfPath, const QString &coverPath,
           const QVector<int> &genres, QObject *parent)
    : QObject(parent), m_id(id), m_title(title), m_author(author), m_publisher(publisher),
    m_price(price), m_discountPercent(discountPercent), m_pdfPath(pdfPath),
    m_coverPath(coverPath), m_genres(genres), m_averageRating(0.0) {}

Book::Book(const Book &other) : QObject(other.parent()) {
    *this = other;
}

Book& Book::operator=(const Book &other) {
    if (this != &other) {
        m_id = other.m_id;
        m_title = other.m_title;
        m_author = other.m_author;
        m_publisher = other.m_publisher;
        m_price = other.m_price;
        m_discountPercent = other.m_discountPercent;
        m_pdfPath = other.m_pdfPath;
        m_coverPath = other.m_coverPath;
        m_genres = other.m_genres;
        m_averageRating = other.m_averageRating;
    }
    return *this;
}

double Book::finalPrice() const {
    return m_price * (1.0 - (m_discountPercent / 100.0));
}

// Setters با انتشار سیگنال در صورت تغییر مقدار
void Book::setId(int id) {
    if (m_id != id) {
        m_id = id;
        emit idChanged();
    }
}

void Book::setTitle(const QString &title) {
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
    }
}

void Book::setAuthor(const QString &author) {
    if (m_author != author) {
        m_author = author;
        emit authorChanged();
    }
}

void Book::setPublisher(const QString &publisher) {
    if (m_publisher != publisher) {
        m_publisher = publisher;
        emit publisherChanged();
    }
}

void Book::setPrice(double price) {
    if (m_price != price) {
        m_price = price;
        emit priceChanged();
    }
}

void Book::setDiscountPercent(double discountPercent) {
    if (m_discountPercent != discountPercent) {
        m_discountPercent = discountPercent;
        emit discountPercentChanged();
    }
}

void Book::setPdfPath(const QString &pdfPath) {
    if (m_pdfPath != pdfPath) {
        m_pdfPath = pdfPath;
        emit pdfPathChanged();
    }
}

void Book::setCoverPath(const QString &coverPath) {
    if (m_coverPath != coverPath) {
        m_coverPath = coverPath;
        emit coverPathChanged();
    }
}

void Book::setGenres(const QVector<int> &genres) {
    if (m_genres != genres) {
        m_genres = genres;
        emit genresChanged();
    }
}

void Book::setAverageRating(double rating) {
    if (m_averageRating != rating) {
        m_averageRating = rating;
        emit averageRatingChanged();
    }
}

// تبدیل آبجکت کتاب به JSON برای ارسال روی شبکه TCP
QJsonObject Book::toJson() const {
    QJsonObject json;
    json["id"] = m_id;
    json["title"] = m_title;
    json["author"] = m_author;
    json["publisher"] = m_publisher;
    json["price"] = m_price;
    json["discountPercent"] = m_discountPercent;
    json["pdfPath"] = m_pdfPath;
    json["coverPath"] = m_coverPath;
    json["averageRating"] = m_averageRating;

    QJsonArray genresArray;
    for (int genreId : m_genres) {
        genresArray.append(genreId);
    }
    json["genres"] = genresArray;

    return json;
}

// ساخت آبجکت کتاب از روی JSON دریافتی از سرور
Book* Book::fromJson(const QJsonObject &json, QObject *parent) {
    Book *book = new Book(parent);
    book->setId(json["id"].toInt());
    book->setTitle(json["title"].toString());
    book->setAuthor(json["author"].toString());
    book->setPublisher(json["publisher"].toString());
    book->setPrice(json["price"].toDouble());
    book->setDiscountPercent(json["discountPercent"].toDouble());
    book->setPdfPath(json["pdfPath"].toString());
    book->setCoverPath(json["coverPath"].toString());
    book->setAverageRating(json["averageRating"].toDouble());

    QVector<int> genres;
    QJsonArray genresArray = json["genres"].toArray();
    for (const QJsonValue &value : genresArray) {
        genres.append(value.toInt());
    }
    book->setGenres(genres);

    return book;
}
