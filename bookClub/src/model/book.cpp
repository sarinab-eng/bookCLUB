#include "Book.h"

Book::Book() : id(0), price(0.0), salesCount(0), lastReadPage(1) {}

Book::Book(int id, QString title, QString author, QString publisher, QString genre,
           QString description, QString filePath, double price)
    : id(id), title(title), author(author), publisherName(publisher), genre(genre),
    description(description), filePath(filePath), price(price), salesCount(0), lastReadPage(1) {}

int Book::getId() const { return id; }
QString Book::getTitle() const { return title; }
QString Book::getAuthor() const { return author; }
QString Book::getPublisher() const { return publisherName; }
QString Book::getGenre() const { return genre; }
QString Book::getDescription() const { return description; }
QString Book::getFilePath() const { return filePath; }
double Book::getPrice() const { return price; }
int Book::getSalesCount() const { return salesCount; }
int Book::getLastReadPage() const { return lastReadPage; }
QVector<Review> Book::getReviews() const { return reviews; }

void Book::setLastReadPage(int page) { if (page > 0) lastReadPage = page; }
void Book::incrementSales(int count) { salesCount += count; }

void Book::addOrUpdateReview(const QString &user, const QString &text, int score) {
    if (score < 1) score = 1;
    if (score > 5) score = 5;

    for (auto &review : reviews) {
        if (review.username == user) {
            review.comment = text;
            review.rating = score;
            return;
        }
    }
    reviews.append({user, text, score});
}

void Book::deleteReview(const QString &user) {
    for (int i = 0; i < reviews.size(); ++i) {
        if (reviews[i].username == user) {
            reviews.removeAt(i);
            break;
        }
    }
}
void Book::setTitle(const QString &title) {
    this->title = title;
}

void Book::setPrice(double price) {
    this->price = price;
}

void Book::setDescription(const QString &desc) {
    this->description = desc;
}


double Book::getAverageRating() const {
    if (reviews.isEmpty()) return 0.0;
    double sum = 0.0;
    for (const auto &r : reviews) sum += r.rating;
    return sum / reviews.size();
}

QJsonObject Book::toJson() const {
    QJsonObject json;
    json["id"] = id;
    json["title"] = title;
    json["author"] = author;
    json["publisher"] = publisherName;
    json["genre"] = genre;
    json["description"] = description;
    json["filePath"] = filePath;
    json["price"] = price;
    json["salesCount"] = salesCount;
    json["lastReadPage"] = lastReadPage;

    QJsonArray reviewsArray;
    for (const auto &r : reviews) {
        reviewsArray.append(r.toJson());
    }
    json["reviews"] = reviewsArray;
    return json;
}

Book Book::fromJson(const QJsonObject &json) {
    Book b(json["id"].toInt(),
           json["title"].toString(),
           json["author"].toString(),
           json["publisher"].toString(),
           json["genre"].toString(),
           json["description"].toString(),
           json["filePath"].toString(),
           json["price"].toDouble());

    b.salesCount = json["salesCount"].toInt();
    b.lastReadPage = json["lastReadPage"].toInt();

    QJsonArray reviewsArray = json["reviews"].toArray();
    for (const auto &val : reviewsArray) {
        b.reviews.append(Review::fromJson(val.toObject()));
    }
    return b;
}
