#ifndef BOOKREADER_H
#define BOOKREADER_H

#include "Book.h"
#include <QObject>
#include <QString>

class BookReader : public QObject {
    Q_OBJECT

private:
    Book* currentBook;
    int currentPage;
    int totalPages;
    double zoomLevel;
    const double ZOOM_STEP = 0.2;
    const double MAX_ZOOM = 3.0;
    const double MIN_ZOOM = 0.5;

public:
    explicit BookReader(QObject* parent = nullptr);
    ~BookReader();

    // متدهای اصلی طبق صفحه ۱۰
    bool openBook(Book* book, int totalPdfPages);
    void closeAndSave();

    // ناوبری و کنترل (الزامات مستند)
    void goToNextPage();
    void goToPrevPage();
    bool goToPage(int pageNumber);

    void zoomIn();
    void zoomOut();
    void resetZoom();

    // Getters برای UI
    int getCurrentPage() const { return currentPage; }
    int getTotalPages() const { return totalPages; }
    double getZoomLevel() const { return zoomLevel; }
    QString getFilePath() const { return currentBook ? currentBook->getFilePath() : ""; }

signals:
    // این سیگنال‌ها به UI فرمان می‌دهند که نمایشگر را بروزرسانی کند
    void requestPageRender(int page, double zoom);
    void statusChanged(int current, int total);
    void zoomFactorChanged(double factor);
    void bookClosed();
};

#endif
