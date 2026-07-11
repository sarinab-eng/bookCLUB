#include "BookReader.h"
#include <QDebug>
#include <QFileInfo>

BookReader::BookReader(QObject* parent)
    : QObject(parent), currentBook(nullptr), currentPage(1), totalPages(100), zoomLevel(1.0) {}

BookReader::~BookReader() {
    closeBook(); // مطمئن می‌شویم هنگام خروج، پیشرفت مطالعه ذخیره شود
}

bool BookReader::openBook(Book* book) {
    if (!book) {
        qWarning() << "BookReader Error: Book pointer is null.";
        return false;
    }

    // بستن کتاب قبلی در صورت باز بودن
    if (currentBook) {
        closeBook();
    }

    // بررسی اینکه آیا فایل فیزیکی کتاب وجود دارد (الزام باز کردن داخل برنامه)
    QFileInfo fileInfo(book->getFilePath());
    if (!fileInfo.exists()) {
        qWarning() << "BookReader Error: PDF file not found at path:" << book->getFilePath();
        return false;
    }

    currentBook = book;
    zoomLevel = 1.0; // شروع با زوم ۱۰۰٪

    // بازیابی آخرین صفحه مطالعه شده (الزام صفحه ۱۰)
    currentPage = currentBook->getLastReadPage();
    if (currentPage < 1) {
        currentPage = 1; // اگر بار اول است، از صفحه ۱ شروع شود
    }

    // در پیاده‌سازی واقعی، تعداد صفحات از فایل PDF استخراج می‌شود.
    // در اینجا یک مقدار پیش‌فرض منطقی برای شبیه‌سازی قرار می‌دهیم.
    totalPages = 120;

    // خبر دادن به برنامه که کتاب با موفقیت باز شد
    emit bookOpened(currentBook->getTitle());
    emit pageChanged(currentPage, totalPages);
    emit zoomChanged(zoomLevel);

    qDebug() << "Book successfully opened:" << currentBook->getTitle()
             << "| Resumed from page:" << currentPage;
    return true;
}

void BookReader::closeBook() {
    if (!currentBook) return;

    // ذخیره آخرین وضعیت مطالعه در شیء کتاب (الزام صفحه ۱۰)
    currentBook->setLastReadPage(currentPage);
    emit progressSaved(currentPage);
    emit bookClosed();

    qDebug() << "Book closed. Progress saved at page:" << currentPage;

    // ریست کردن متغیرها
    currentBook = nullptr;
    currentPage = 1;
    totalPages = 1;
    zoomLevel = 1.0;
}

bool BookReader::goToNextPage() {
    if (!currentBook) return false;

    if (currentPage < totalPages) {
        currentPage++;
        emit pageChanged(currentPage, totalPages);
        return true;
    }
    return false; // صفحه آخر است و نمی‌توان جلوتر رفت
}

bool BookReader::goToPrevPage() {
    if (!currentBook) return false;

    if (currentPage > 1) {
        currentPage--;
        emit pageChanged(currentPage, totalPages);
        return true;
    }
    return false; // صفحه اول است و نمی‌توان عقب‌تر رفت
}

bool BookReader::goToPage(int pageNumber) {
    if (!currentBook) return false;

    if (pageNumber >= 1 && pageNumber <= totalPages) {
        currentPage = pageNumber;
        emit pageChanged(currentPage, totalPages);
        return true;
    }
    return false; // شماره صفحه نامعتبر است
}

void BookReader::zoomIn() {
    if (!currentBook) return;

    if (zoomLevel + ZOOM_STEP <= MAX_ZOOM) {
        zoomLevel += ZOOM_STEP;
        emit zoomChanged(zoomLevel);
    }
}

void BookReader::zoomOut() {
    if (!currentBook) return;

    if (zoomLevel - ZOOM_STEP >= MIN_ZOOM) {
        zoomLevel -= ZOOM_STEP;
        emit zoomChanged(zoomLevel);
    }
}

void BookReader::resetZoom() {
    if (!currentBook) return;

    zoomLevel = 1.0;
    emit zoomChanged(zoomLevel);
}

// متدهای کمکی برای خواندن اطلاعات جاری
int BookReader::getCurrentPage() const { return currentPage; }
int BookReader::getTotalPages() const { return totalPages; }
double BookReader::getZoomLevel() const { return zoomLevel; }
bool BookReader::isBookOpen() const { return currentBook != nullptr; }

QString BookReader::getBookTitle() const {
    return currentBook ? currentBook->getTitle() : QString("");
}
