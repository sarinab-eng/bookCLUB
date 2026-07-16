#include "Book.h"

Book::Book()
    : id(0), title(""), author(""), publisherName(""), genre(""), description("")
    , filePath(""), price(0.0), salesCount(0), lastReadPage(1), m_stock(0), available(true)
{}

Book::Book(int id, const QString &title, const QString &author, const QString &publisher,
           const QString &genre, const QString &description, const QString &filePath, double price)
    : id(id), title(title), author(author), publisherName(publisher), genre(genre)
    , description(description), filePath(filePath), price(price), salesCount(0)
    , lastReadPage(1), m_stock(0), available(true)
{}

// Getters
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
bool Book::getIsAvailable() const { return available; }
int Book::getStock() const { return m_stock; }

// Setters
void Book::setTitle(const QString &title) { this->title = title; }
void Book::setPrice(double price) { this->price = price; }
void Book::setDescription(const QString &desc) { this->description = desc; }
void Book::setLastReadPage(int page) { if (page > 0) lastReadPage = page; }
void Book::setIsAvailable(bool status) { available = status; }
void Book::setSalesCount(int count) { salesCount = count; }
void Book::setStock(int stock) { m_stock = stock; }
void Book::incrementSales(int count) { salesCount += count; }

// پیاده‌سازی یکپارچه toJson
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
    json["available"] = available;
    json["stock"] = m_stock;
    json["reviews"] = reviewManager.toJsonArray();
    return json;
}

// پیاده‌سازی یکپارچه fromJson
Book Book::fromJson(const QJsonObject &json) {
    Book b;
    b.id = json["id"].toInt();
    b.title = json["title"].toString();
    b.author = json["author"].toString();
    b.publisherName = json["publisher"].toString();
    b.genre = json["genre"].toString();
    b.description = json["description"].toString();
    b.filePath = json["filePath"].toString();
    b.price = json["price"].toDouble();
    b.salesCount = json["salesCount"].toInt();
    b.lastReadPage = json["lastReadPage"].toInt();
    b.available = json["available"].toBool();
    b.m_stock = json["stock"].toInt();

    if (json.contains("reviews")) {
        QJsonArray reviewsArray = json["reviews"].toArray();
        b.reviewManager.fromJsonArray(reviewsArray);
    }

    return b;
}
