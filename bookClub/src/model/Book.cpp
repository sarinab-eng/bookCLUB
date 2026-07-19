#ifndef BOOK_H
#define BOOK_H

#include <QString>

struct Book {
    QString id;
    QString title;
    QString author;
    double  price;
    QString genre;
};

struct CartItem {
    QString bookId;
    QString title;
    QString author;
    double  price;
};

#endif
