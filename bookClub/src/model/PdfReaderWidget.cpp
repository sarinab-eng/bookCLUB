#include "PdfReaderWidget.h"
#include <QFileInfo>

PdfReaderWidget::PdfReaderWidget(QWidget *parent)
    : QWidget(parent), m_bookId(-1)
{
    setupUi();
}

void PdfReaderWidget::setupUi() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    // نوار بالا
    QHBoxLayout *topBarLayout = new QHBoxLayout();
    lblBookTitle = new QLabel("نام کتاب", this);
    lblBookTitle->setStyleSheet("font-weight: bold; font-size: 14px; color: #333333;");

    btnClose = new QPushButton("خروج", this);
    btnClose->setCursor(Qt::PointingHandCursor);
    btnClose->setStyleSheet(
        "QPushButton { background-color: #ff6b81; color: white; border: none; "
        "padding: 6px 12px; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background-color: #ff4757; }"
        );
    connect(btnClose, &QPushButton::clicked, this, &PdfReaderWidget::closeReader);

    topBarLayout->addWidget(lblBookTitle);
    topBarLayout->addStretch();
    topBarLayout->addWidget(btnClose);
    mainLayout->addLayout(topBarLayout);

    // ویوور وب‌سایت/پی‌دی‌اف خوان داخلی کرومیوم
    m_webView = new QWebEngineView(this);
    m_webView->setStyleSheet("border: 1px solid #e0e0e0;");
    mainLayout->addWidget(m_webView);
}

bool PdfReaderWidget::openBook(int bookId, const QString &title, const QString &filePath) {
    m_bookId = bookId;
    m_pdfPath = filePath;
    lblBookTitle->setText("در حال مطالعه: " + title);

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        return false;
    }

    // بارگذاری فایل PDF محلی در WebView (کرومیوم به صورت خودکار PDF را رندر می‌کند)
    m_webView->setUrl(QUrl::fromLocalFile(filePath));
    return true;
}

void PdfReaderWidget::closeReader() {
    // از آنجا که QWebEngineView کنترل‌های داخلی PDF Reader کروم (شامل نمایش شماره صفحه) را دارد،
    // در این حالت نیازی به مدیریت دستی شماره صفحه نیست یا می‌توان مقدار پیش‌فرض را ذخیره کرد.
    emit saveReadingProgress(m_bookId, 0);
    emit closed();
}
