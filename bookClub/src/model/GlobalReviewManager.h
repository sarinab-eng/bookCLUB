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

    void addReview(const Review& review); // تغییر به const reference
    QList<Review> getReviewsForBook(const QString &bookTitle);
    double getAverageRating(const QString &bookTitle);

private:
    explicit GlobalReviewManager(QObject *parent = nullptr);
    static GlobalReviewManager* instance;
    QList<Review> m_allReviews; // ذخیره خود اشیاء به جای اشاره‌گر
};

#endif // GLOBALREVIEWMANAGER_H
