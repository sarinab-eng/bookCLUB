#include "Book.h"

Book::Book()
    : id(0)
    , title("")
    , author("")
    , publisherName("")
    , genre("")
    , description("")
    , filePath("")
    , price(0.0)
    , salesCount(0)
    , lastReadPage(1)
    , available(true)
{}

Book::Book(int id, const QString &title, const QString &author, const QString &publisher,
           const QString &genre, const QString &description, const QString &filePath, double price)
    : id(id)
    , title(title)
    , author(author)
    , publisherName(publisher)
    , genre(genre)
    , description(description)
    , filePath(filePath)
    , price(price)
    , salesCount(0)
    , lastReadPage(1)
    , available(true)
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

// Setters
void Book::setTitle(const QString &title) { this->title = title; }
void Book::setPrice(double price) { this->price = price; }
void Book::setDescription(const QString &desc) { this->description = desc; }
void Book::setLastReadPage(int page) { if (page > 0) lastReadPage = page; }
void Book::setIsAvailable(bool status) { available = status; }
void Book::setSalesCount(int count) { salesCount = count; }
void Book::incrementSales(int count) { salesCount += count; }

// سریال‌سازی به فرمت JSON برای شبکه و ذخیره‌سازی کلاینت-سرور
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

    // استفاده مستقیم از متد تبدیل آرایه JSON کلاس ReviewManager
    json["reviews"] = reviewManager.toJsonArray();
    return json;
}

// ساخت شیء Book از روی داده‌های JSON دریافتی
Book Book::fromJson(const QJsonObject &json) {
    Book b(json["id"].toInt(),
           json["title"].toString(),
           json["author"].toString(),
           json["publisher"].toString(),
           json["genre"].toString(),
           json["description"].toString(),
           json["filePath"].toString(),
           json["price"].toDouble());

    b.setSalesCount(json["salesCount"].toInt());
    b.setLastReadPage(json["lastReadPage"].toInt());
    b.setIsAvailable(json["available"].toBool());

    // بارگذاری نظرات از آرایه JSON داخل شیء ReviewManager همان کتاب
    QJsonArray reviewsArray = json["reviews"].toArray();
    b.reviewManager.fromJsonArray(reviewsArray);

    return b;
}
