#ifndef BOOKDETAILSWIDGET_H
#define BOOKDETAILSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QSpinBox>
#include "Book.h"

class BookDetailsWidget : public QWidget {
    Q_OBJECT
public:
    explicit BookDetailsWidget(QWidget *parent = nullptr);
    void setBook(const Book &book);

private slots:
    void onAddToCartClicked();
    void onSubmitReviewClicked();

private:
    void setupUi();
    void updateReviewsList();

    // متغیرهایی که در تصویر قرمز بودند را اینجا تعریف می‌کنیم:
    QLabel *titleLabel;
    QLabel *authorLabel;
    QLabel *publisherLabel;
    QLabel *genreLabel;
    QLabel *descriptionLabel;
    QLabel *priceLabel;

    QPushButton *addToCartButton;
    QPushButton *submitReviewButton;

    QTableWidget *reviewsTable;
    QTextEdit *reviewTextEdit;
    QSpinBox *ratingSpinBox;

    Book currentBook; // برای ذخیره کتاب فعلی
};

#endif // BOOKDETAILSWIDGET_H
