#include "bookmanager.h"

BookManager::BookManager(QTcpSocket *socket, QObject *parent)
    : QObject(parent), socket(socket) {}

// درخواست لیست کامل کتاب‌ها از سرور
void BookManager::requestAllBooks() {
    QJsonObject request;
    request["type"] = "get_all_books";

    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(QJsonDocument(request).toJson());
    }
}

// درخواست فیلتر شده بر اساس ژانر
void BookManager::requestBooksByGenre(const QString &genre) {
    QJsonObject request;
    request["type"] = "get_books_by_genre";
    request["genre"] = genre;

    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(QJsonDocument(request).toJson());
    }
}

// متد اضافه کردن کتاب (مخصوص پنل ناشر)
void BookManager::addNewBook(Book *book) {
    if (!book) return;

    QJsonObject request;
    request["type"] = "add_new_book";
    request["data"] = book->toJson(); // متدی که در کلاس book.cpp نوشتید

    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(QJsonDocument(request).toJson());
    }
}
