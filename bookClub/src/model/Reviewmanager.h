#ifndef REVIEWMANAGER_H
#define REVIEWMANAGER_H

#include "Review.h"
#include <QVector>
#include <QJsonArray>

class ReviewManager {
private:
    QVector<Review> reviews;

public:
    ReviewManager();

    // اضافه کردن نظر جدید
    void addReview(const Review &review);

    // حذف نظر توسط ID کاربر
    void removeReview(int userId);

    // محاسبه میانگین امتیازات
    double getAverageRating() const;

    int getReviewsCount() const;
    const QVector<Review>& getAllReviews() const;

    // تبدیل به JSON و برعکس
    QJsonArray toJsonArray() const;
    void fromJsonArray(const QJsonArray &arr);
};

#endif // REVIEWMANAGER_H
