#include "ReviewManager.h"

ReviewManager::ReviewManager() {
    // سازنده پیش‌فرض
}

void ReviewManager::addReview(const Review &review) {
    // اگر کاربر قبلاً نظر داده، نظر قبلی حذف و نظر جدید جایگزین شود
    for (int i = 0; i < reviews.size(); ++i) {
        if (reviews[i].getUserId() == review.getUserId()) {
            reviews.removeAt(i);
            break;
        }
    }
    reviews.append(review);
}

void ReviewManager::removeReview(int userId) {
    for (int i = 0; i < reviews.size(); ++i) {
        if (reviews[i].getUserId() == userId) {
            reviews.removeAt(i);
            break;
        }
    }
}

double ReviewManager::getAverageRating() const {
    if (reviews.isEmpty()) return 0.0;
    double sum = 0;
    for (const auto &r : reviews) {
        sum += r.getRating();
    }
    return sum / static_cast<double>(reviews.size());
}

int ReviewManager::getReviewsCount() const {
    return reviews.size();
}

const QVector<Review>& ReviewManager::getAllReviews() const {
    return reviews;
}

QJsonArray ReviewManager::toJsonArray() const {
    QJsonArray arr;
    for (const auto &r : reviews) {
        arr.append(r.toJson());
    }
    return arr;
}

void ReviewManager::fromJsonArray(const QJsonArray &arr) {
    reviews.clear();
    for (auto ref : arr) {
        reviews.append(Review::fromJson(ref.toObject()));
    }
}
