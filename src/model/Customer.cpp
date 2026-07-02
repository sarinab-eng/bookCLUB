#include "Customer.h"

Customer::Customer(QObject *parent) : User(parent), balance(0.0) {
}

void Customer::buyBook(int bookId, double price) {
    if (balance >= price) {
        balance -= price;
        addToLibrary(bookId);
    }
}

void Customer::addToLibrary(int bookId) {
    if (!purchasedBookIds.contains(bookId)) {
        purchasedBookIds.append(bookId);
    }
}