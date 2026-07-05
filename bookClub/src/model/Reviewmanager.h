#ifndef REVIEWMANAGER_H
#define REVIEWMANAGER_H

#include "Review.h"
#include <QVector>
#include <QJsonArray>

class ReviewManager {
private:
    QVector<Review> reviews;

public:
    ReviewManager() {}

    // اضافه کردن نظر جدید (صفحه 12: امکان ثبت نظر توسط خریداران)
    void addReview(const Review &review) {
        // اگر کاربر قبلاً نظر داده، نظر قبلی حذف و نظر جدید جایگزین شود (اختیاری)
        for (int i = 0; i < reviews.size(); ++i) {
            if (reviews[i].getUserId() == review.getUserId()) {
                reviews.removeAt(i);
                break;
            }
        }
        reviews.append(review);
    }

    // حذف نظر (توسط ادمین یا خود کاربر - صفحه 12)
    void removeReview(int userId) {
        for (int i = 0; i < reviews.size(); ++i) {
            if (reviews[i].getUserId() == userId) {
                reviews.removeAt(i);
                break;
            }
        }
    }

    // محاسبه میانگین امتیازات کتاب (برای نمایش در فروشگاه)
    double getAverageRating() const {
        if (reviews.isEmpty()) return 0.0;
        double sum = 0;
        for (const auto &r : reviews) sum += r.getRating();
        return sum / reviews.size();
    }

    int getReviewsCount() const { return reviews.size(); }
    const QVector<Review>& getAllReviews() const { return reviews; }

    // تبدیل لیست نظرات به JSON
    QJsonArray toJsonArray() const {
        QJsonArray arr;
        for (const auto &r : reviews) arr.append(r.toJson());
        return arr;
    }

    void fromJsonArray(const QJsonArray &arr) {
        reviews.clear();
        for (auto ref : arr) {
            reviews.append(Review::fromJson(ref.toObject()));
        }
    }
};

#endif // REVIEWMANAGER_H
