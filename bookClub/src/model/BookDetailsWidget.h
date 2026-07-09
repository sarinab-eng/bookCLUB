#ifndef BOOKDETAILSWIDGET_H
#define BOOKDETAILSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QListWidget>
#include <QSpinBox>
#include "Book.h"

class BookDetailsWidget : public QWidget {
    Q_OBJECT

private:
    Book currentBook;

    // المان‌های گرافیکی (UI Elements)
    QLabel *titleLabel;
    QLabel *authorLabel;
    QLabel *publisherLabel;
    QLabel *ratingLabel;
    QLabel *priceLabel;
    QTextBrowser *descriptionBrowser;
    QListWidget *reviewsListWidget;

    QPushButton *addToCartButton;
    QPushButton *addReviewButton;
    QSpinBox *ratingSpinBox; // برای ثبت امتیاز ۱ تا ۵ ستاره

public:
    explicit BookDetailsWidget(QWidget *parent = nullptr);

    // متد برای بارگذاری داده‌های یک کتاب در UI
    void setBook(const Book &book);

signals:
    // سیگنالی که وقتی کاربر روی خرید کلیک کرد به سبد خرید خبر می‌دهد
    void addToCartRequested(int bookId);
    // سیگنالی برای ثبت نظر جدید به سرور
    void addReviewRequested(int bookId, int rating, const QString &comment);

private slots:
    void onAddToCartClicked();
    void onAddReviewClicked();
};

#endif // BOOKDETAILSWIDGET_H
