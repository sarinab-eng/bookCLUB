#include "Publisher.h"

// پیاده‌سازی سازنده با مقداردهی اولیه در فایل cpp
Publisher::Publisher(QObject *parent) : User(parent), isApproved(false) {
}

void Publisher::publishNewBook(int bookId) {
    if (!publishedBookIds.contains(bookId)) {
        publishedBookIds.append(bookId);
    }
}

void Publisher::viewSalesStatistics() {
}