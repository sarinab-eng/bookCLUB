#ifndef BOOKREADER_H
#define BOOKREADER_H

#include "Book.h"
#include <QObject>
#include <QString>

class BookReader : public QObject
{
    Q_OBJECT

private:
    Book* currentBook;    // اشاره‌گر به کتابی که در حال مطالعه است
    int currentPage;      // شماره صفحه جاری
    int totalPages;       // تعداد کل صفحات کتاب
    double zoomLevel;     // درصد بزرگ‌نمایی (مثلاً 1.0 یعنی 100٪)

    // مقادیر ثابت برای محدوده بزرگ‌نمایی
    const double MAX_ZOOM = 2.5;
    const double MIN_ZOOM = 0.5;
    const double ZOOM_STEP = 0.1; // هر بار ۱۰ درصد زوم تغییر کند

public:
    explicit BookReader(QObject* parent = nullptr);
    ~BookReader();

    // باز کردن و بستن کتاب
    bool openBook(Book* book);
    void closeBook();

    // دکمه‌ها و فرمان‌های ناوبری (مطابق الزامات صفحه ۱۰)
    bool goToNextPage();
    bool goToPrevPage();
    bool goToPage(int pageNumber);

    // فرمان‌های بزرگ‌نمایی و کوچک‌نمایی
    void zoomIn();
    void zoomOut();
    void resetZoom();

    // متدهای دریافت اطلاعات (Getters)
    int getCurrentPage() const;
    int getTotalPages() const;
    double getZoomLevel() const;
    bool isBookOpen() const;
    QString getBookTitle() const;

signals:
    // این سیگنال‌ها تغییرات را به بخش ظاهری (UI) خبر می‌دهند تا صفحه آپدیت شود
    void pageChanged(int currentPage, int totalPages);
    void zoomChanged(double zoomLevel);
    void bookOpened(const QString& bookTitle);
    void bookClosed();
    void progressSaved(int lastPage);
};

#endif // BOOKREADER_H
