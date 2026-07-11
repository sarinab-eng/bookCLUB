#include "BookManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>

BookManager::BookManager() {}

void BookManager::addBook(const Book &book) {
    if (findBookById(book.getId()) == nullptr) {
        allBooks.append(book);
    }
}

bool BookManager::editBook(int bookId, const QString &title, double price, const QString &desc) {
    Book* book = findBookById(bookId);
    if (book) {
        book->setTitle(title);
        book->setPrice(price);
        book->setDescription(desc);
        return true;
    }
    return false;
}

bool BookManager::deleteBook(int bookId) {
    for (int i = 0; i < allBooks.size(); ++i) {
        if (allBooks[i].getId() == bookId) {
            allBooks.removeAt(i);
            return true;
        }
    }
    return false;
}

Book* BookManager::findBookById(int bookId) {
    for (auto &book : allBooks) {
        if (book.getId() == bookId) return &book;
    }
    return nullptr;
}

QVector<Book> BookManager::search(const QString &query) const {
    QVector<Book> results;
    QString lowerQuery = query.toLower().trimmed();
    for (const auto &book : allBooks) {
        if (book.getTitle().toLower().contains(lowerQuery) ||
            book.getAuthor().toLower().contains(lowerQuery) ||
            book.getPublisher().toLower().contains(lowerQuery)) {
            results.append(book);
        }
    }
    return results;
}

QVector<Book> BookManager::filterByGenre(const QString &genre) const {
    QVector<Book> results;
    for (const auto &book : allBooks) {
        if (book.getGenre().compare(genre, Qt::CaseInsensitive) == 0) {
            results.append(book);
        }
    }
    return results;
}

QVector<Book> BookManager::getPublisherBooks(const QString &publisherName) const {
    QVector<Book> results;
    for (const auto &book : allBooks) {
        if (book.getPublisher().compare(publisherName, Qt::CaseInsensitive) == 0) {
            results.append(book);
        }
    }
    return results;
}

QVector<Book> BookManager::getBestSellers(const QString &publisherName) const {
    QVector<Book> pubBooks = getPublisherBooks(publisherName);
    std::sort(pubBooks.begin(), pubBooks.end(), [](const Book &a, const Book &b) {
        return a.getSalesCount() > b.getSalesCount();
    });
    return pubBooks;
}

QVector<Book> BookManager::getWorstSellers(const QString &publisherName) const {
    QVector<Book> pubBooks = getPublisherBooks(publisherName);
    std::sort(pubBooks.begin(), pubBooks.end(), [](const Book &a, const Book &b) {
        return a.getSalesCount() < b.getSalesCount();
    });
    return pubBooks;
}

double BookManager::getPublisherAverageRating(const QString &publisherName) const {
    QVector<Book> pubBooks = getPublisherBooks(publisherName);
    if (pubBooks.isEmpty()) return 0.0;

    double totalRatingSum = 0.0;
    int ratedBooksCount = 0;

    for (const auto &book : pubBooks) {
        double avg = book.getAverageRating();
        if (avg > 0.0) {
            totalRatingSum += avg;
            ratedBooksCount++;
        }
    }
    return ratedBooksCount > 0 ? (totalRatingSum / ratedBooksCount) : 0.0;
}

void BookManager::saveToFile(const QString &filePath) const {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonArray arr;
        for (const auto &book : allBooks) {
            arr.append(book.toJson());
        }
        QJsonDocument doc(arr);
        file.write(doc.toJson());
        file.close();
    }
}

void BookManager::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        allBooks.clear();
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray arr = doc.array();
        for (auto val : arr) {
            allBooks.append(Book::fromJson(val.toObject()));
        }
        file.close();
    }
}

QVector<Book> BookManager::filterByPriceRange(double minPrice, double maxPrice) const {
    QVector<Book> results;
    for (const auto &book : allBooks) {
        if (book.getPrice() >= minPrice && book.getPrice() <= maxPrice) {
            results.append(book);
        }
    }
    return results;
}

QVector<Book> BookManager::filterByMinRating(double minRating) const {
    QVector<Book> results;
    for (const auto &book : allBooks) {
        if (book.getAverageRating() >= minRating) {
            results.append(book);
        }
    }
    return results;
}

bool BookManager::setBookAvailability(int bookId, bool status) {
    Book* book = findBookById(bookId);
    if (book) {
        book->setIsAvailable(status);
        return true;
    }
    return false;
}

void BookManager::recordSale(int bookId, int quantity) {
    Book* book = findBookById(bookId);
    if (book) {
        book->setSalesCount(book->getSalesCount() + quantity);
    }
}
