#include "GlobalReviewManager.h"

GlobalReviewManager* GlobalReviewManager::instance = nullptr;

GlobalReviewManager::GlobalReviewManager(QObject *parent) : QObject(parent)
{
}

GlobalReviewManager* GlobalReviewManager::getInstance()
{
    if (instance == nullptr) {
        instance = new GlobalReviewManager();
    }
    return instance;
}

void GlobalReviewManager::addReview(const Review& review)
{
    m_allReviews.append(review);
}

QList<Review> GlobalReviewManager::getReviewsForBook(const QString &bookTitle)
{
    QList<Review> bookReviews;
    for (const Review& review : m_allReviews) {
        if (review.getBookTitle() == bookTitle) {
            bookReviews.append(review);
        }
    }
    return bookReviews;
}

double GlobalReviewManager::getAverageRating(const QString &bookTitle)
{
    QList<Review> reviews = getReviewsForBook(bookTitle);
    if (reviews.isEmpty()) return 0.0;

    double sum = 0;
    for (const Review& review : reviews) {
        sum += review.getRating();
    }
    return sum / (double)reviews.size();
}
