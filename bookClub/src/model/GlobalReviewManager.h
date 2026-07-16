#ifndef GLOBALREVIEWMANAGER_H
#define GLOBALREVIEWMANAGER_H

#include <QObject>
#include <QList>
#include "Review.h"

class GlobalReviewManager : public QObject
{
    Q_OBJECT
public:
    static GlobalReviewManager* getInstance();

    void addReview(const Review& review);
    QList<Review> getReviewsForBook(const QString &bookTitle);
    double getAverageRating(const QString &bookTitle);

private:
    explicit GlobalReviewManager(QObject *parent = nullptr);
    static GlobalReviewManager* instance;
    QList<Review> m_allReviews;
};

#endif // GLOBALREVIEWMANAGER_H
