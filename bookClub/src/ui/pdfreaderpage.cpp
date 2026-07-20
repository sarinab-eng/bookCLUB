#include "pdfreaderpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

PdfReaderPage::PdfReaderPage(AuthManager *authManager, QWidget *parent)
    : QWidget(parent), m_authManager(authManager)
{
    m_document = new QPdfDocument(this);
    setupUi();

    connect(m_pdfView->pageNavigation(), &QPdfPageNavigation::currentPageChanged,
            this, &PdfReaderPage::onCurrentPageChanged);

    if (m_authManager)
        connect(m_authManager, &AuthManager::readingProgressReceived,
                this, &PdfReaderPage::onReadingProgressReceived);
}

void PdfReaderPage::setCurrentUsername(const QString &username)
{
    m_currentUsername = username;
}

void PdfReaderPage::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(m_titleLabel);

    m_pdfView = new QPdfView(this);
    m_pdfView->setDocument(m_document);
    m_pdfView->setPageMode(QPdfView::PageMode::SinglePage);
    m_pdfView->setZoomMode(QPdfView::ZoomMode::CustomZoom);
    m_pdfView->setZoomFactor(m_zoomFactor);
    mainLayout->addWidget(m_pdfView, 1);

    QHBoxLayout *controlsLayout = new QHBoxLayout;
    m_backButton = new QPushButton("بازگشت");
    m_prevButton = new QPushButton("◀ صفحه قبل");
    m_pageLabel = new QLabel("صفحه ۰ از ۰");
    m_pageSpinBox = new QSpinBox;
    m_pageSpinBox->setMinimum(1);
    m_goToPageButton = new QPushButton("برو به صفحه");
    m_nextButton = new QPushButton("صفحه بعد ▶");
    m_zoomOutButton = new QPushButton("−");
    m_zoomInButton = new QPushButton("+");

    controlsLayout->addWidget(m_backButton);
    controlsLayout->addWidget(m_prevButton);
    controlsLayout->addWidget(m_pageLabel);
    controlsLayout->addWidget(m_pageSpinBox);
    controlsLayout->addWidget(m_goToPageButton);
    controlsLayout->addWidget(m_nextButton);
    controlsLayout->addWidget(m_zoomOutButton);
    controlsLayout->addWidget(m_zoomInButton);
    mainLayout->addLayout(controlsLayout);

    connect(m_backButton, &QPushButton::clicked, this, &PdfReaderPage::backRequested);
    connect(m_prevButton, &QPushButton::clicked, this, &PdfReaderPage::onPreviousPageClicked);
    connect(m_nextButton, &QPushButton::clicked, this, &PdfReaderPage::onNextPageClicked);
    connect(m_goToPageButton, &QPushButton::clicked, this, &PdfReaderPage::onGoToPageClicked);
    connect(m_zoomInButton, &QPushButton::clicked, this, &PdfReaderPage::onZoomInClicked);
    connect(m_zoomOutButton, &QPushButton::clicked, this, &PdfReaderPage::onZoomOutClicked);
}

void PdfReaderPage::openBook(const QJsonObject &book)
{
    m_currentBookId = book.value("id").toString();
    m_titleLabel->setText(book.value("title").toString());

    QString fileUrl = book.value("fileURL").toString();
    if (fileUrl.isEmpty()) {
        QMessageBox::information(this, "کتاب", "فایل PDF برای این کتاب تعریف نشده است.");
        return;
    }

    QPdfDocument::DocumentError err = m_document->load(fileUrl);
    if (err != QPdfDocument::DocumentError::NoError) {
        QMessageBox::warning(this, "خطا", "باز کردن فایل کتاب ممکن نشد: " + fileUrl);
        return;
    }

    m_pageSpinBox->setMaximum(qMax(1, m_document->pageCount()));
    updatePageLabel();

    // آخرین صفحه‌ی مطالعه‌شده رو از سرور می‌گیریم تا مطالعه از همان صفحه ادامه پیدا کند
    if (m_authManager && !m_currentUsername.isEmpty())
        m_authManager->requestReadingProgress(m_currentUsername, m_currentBookId);
}

void PdfReaderPage::onReadingProgressReceived(const QString &bookId, int page)
{
    if (bookId != m_currentBookId) return;
    if (page > 0 && page < m_document->pageCount())
        m_pdfView->pageNavigation()->setCurrentPage(page);
}

void PdfReaderPage::onCurrentPageChanged(int page)
{
    updatePageLabel();

    // ذخیره‌ی خودکار آخرین صفحه‌ی مطالعه‌شده برای این کاربر و این کتاب
    if (m_authManager && !m_currentUsername.isEmpty() && !m_currentBookId.isEmpty())
        m_authManager->saveReadingProgress(m_currentUsername, m_currentBookId, page);
}

void PdfReaderPage::updatePageLabel()
{
    int current = m_pdfView->pageNavigation()->currentPage() + 1;
    int total = m_document->pageCount();
    m_pageLabel->setText(QString("صفحه %1 از %2").arg(current).arg(total));
    m_pageSpinBox->blockSignals(true);
    m_pageSpinBox->setValue(current);
    m_pageSpinBox->blockSignals(false);
}

void PdfReaderPage::onPreviousPageClicked()
{
    m_pdfView->pageNavigation()->goToPreviousPage();
}

void PdfReaderPage::onNextPageClicked()
{
    m_pdfView->pageNavigation()->goToNextPage();
}

void PdfReaderPage::onGoToPageClicked()
{
    int target = m_pageSpinBox->value() - 1;
    if (target < 0 || target >= m_document->pageCount()) return;
    m_pdfView->pageNavigation()->setCurrentPage(target);
}

void PdfReaderPage::onZoomInClicked()
{
    m_zoomFactor = qMin(m_zoomFactor * 1.25, 5.0);
    m_pdfView->setZoomFactor(m_zoomFactor);
}

void PdfReaderPage::onZoomOutClicked()
{
    m_zoomFactor = qMax(m_zoomFactor / 1.25, 0.2);
    m_pdfView->setZoomFactor(m_zoomFactor);
}
