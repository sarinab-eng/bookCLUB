#ifndef BOOK_H
#define BOOK_H

#include <QString>

struct Book {
    QString id;
    QString title;
    QString author;
    QString publisher;   // ← اضافه شد
    QString genre;
    double price = 0.0;
    QString coverImage;
    QString description;
    int stock = 0;
};

struct CartItem {
    QString bookId;
    QString title;
    double price = 0.0;
    int quantity = 1;
    double totalPrice() const { return price * quantity; }
};

#endif // BOOK_H
