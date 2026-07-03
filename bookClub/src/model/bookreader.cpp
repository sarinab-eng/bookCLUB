#include "BookReader.h"
#include <QDebug>

BookReader::BookReader(QObject* parent)
    : QObject(parent), currentBook(nullptr), currentPage(1), totalPages(1), zoomLevel(1.0) {}

BookReader::~BookReader() {
    closeAndSave();
}

bool BookReader::openBook(Book* book, int totalPdfPages) {
    if (!book) return false;

    currentBook = book;
    totalPages = totalPdfPages;
    zoomLevel = 1.0;

    // ۱. قابلیت ذخیره آخرین صفحه مطالعه شده (الزام صفحه ۱۰)
    // لود کردن موقعیت قبلی از دیتای کتاب
    currentPage = currentBook->getLastReadPage();
    if (currentPage < 1 || currentPage > totalPages) currentPage = 1;

    emit statusChanged(currentPage, totalPages);
    emit requestPageRender(currentPage, zoomLevel);
    return true;
}

void BookReader::goToNextPage() {
    if (currentPage < totalPages) {
        currentPage++;
        emit statusChanged(currentPage, totalPages);
        emit requestPageRender(currentPage, zoomLevel);
    }
}

void BookReader::goToPrevPage() {
    if (currentPage > 1) {
        currentPage--;
        emit statusChanged(currentPage, totalPages);
        emit requestPageRender(currentPage, zoomLevel);
    }
}

bool BookReader::goToPage(int pageNumber) {
    if (pageNumber >= 1 && pageNumber <= totalPages) {
        currentPage = pageNumber;
        emit statusChanged(currentPage, totalPages);
        emit requestPageRender(currentPage, zoomLevel);
        return true;
    }
    return false;
}

void BookReader::zoomIn() {
    if (zoomLevel < MAX_ZOOM) {
        zoomLevel += ZOOM_STEP;
        emit zoomFactorChanged(zoomLevel);
        emit requestPageRender(currentPage, zoomLevel);
    }
}

void BookReader::zoomOut() {
    if (zoomLevel > MIN_ZOOM) {
        zoomLevel -= ZOOM_STEP;
        emit zoomFactorChanged(zoomLevel);
        emit requestPageRender(currentPage, zoomLevel);
    }
}

void BookReader::closeAndSave() {
    if (currentBook) {
        // ذخیره آخرین صفحه مطالعه شده در شیء کتاب قبل از بستن
        currentBook->setLastReadPage(currentPage);
        emit bookClosed();
        currentBook = nullptr;
    }
}
