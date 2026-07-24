#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "User.h"
#include <QString>
#include <QVector>

class Customer : public User {
    Q_OBJECT
public:
    explicit Customer(QObject *parent = nullptr);

    QString getRole() const override { return "Customer"; }

    double balance;
    QVector<int> purchasedBookIds;
    QVector<int> favoriteGenres;

    void buyBook(int bookId, double price);
    void addToLibrary(int bookId);
};

#endif