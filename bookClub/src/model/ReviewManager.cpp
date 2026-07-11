#include "GlobalReviewManager.h"

GlobalReviewManager* GlobalReviewManager::instance = nullptr;

GlobalReviewManager* GlobalReviewManager::getInstance() {
    if (!instance) {
        instance = new GlobalReviewManager();
    }
    return instance;
}

GlobalReviewManager::GlobalReviewManager(QObject *parent) : QObject(parent) {}

void GlobalReviewManager::addOrUpdateReview(const Review &review) {
    int bookId = review.getBookId();
    int userId = review.getUserId();

    QVector<Review> &reviews = bookReviews[bookId];
    bool updated = false;

    for (int i = 0; i < reviews.size(); ++i) {
        if (reviews[i].getUserId() == userId) {
            reviews[i] = review;
            updated = true;
            break;
        }
    }

    if (!updated) {
        reviews.append(review);
    }

    emit reviewsUpdated(bookId);
}

QVector<Review> GlobalReviewManager::getReviewsForBook(int bookId) const {
    return bookReviews.value(bookId, QVector<Review>());
}

double GlobalReviewManager::calculateAverageRating(int bookId) const {
    if (!bookReviews.contains(bookId) || bookReviews[bookId].isEmpty()) {
        return 0.0;
    }

    const QVector<Review> &reviews = bookReviews[bookId];
    double sum = 0.0;
    for (const auto &review : reviews) {
        sum += review.getRating();
    }

    return sum / reviews.size();
}

void GlobalReviewManager::clear() {
    bookReviews.clear();
}
