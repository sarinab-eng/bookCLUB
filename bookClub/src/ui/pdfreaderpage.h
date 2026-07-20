#ifndef PDFREADERPAGE_H
#define PDFREADERPAGE_H

#include <QWidget>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QPdfDocument>
#include <QPdfView>
#include <QPdfPageNavigation>
#include "Authmanager.h"

// ماژول مطالعه‌ی داخلی کتاب (PDF Reader) 
class PdfReaderPage : public QWidget
{
    Q_OBJECT
public:
    explicit PdfReaderPage(AuthManager *authManager, QWidget *parent = nullptr);

    void setCurrentUsername(const QString &username);
    void openBook(const QJsonObject &book);

signals:
    void backRequested();

private slots:
    void onPreviousPageClicked();
    void onNextPageClicked();
    void onGoToPageClicked();
    void onZoomInClicked();
    void onZoomOutClicked();
    void onCurrentPageChanged(int page);
    void onReadingProgressReceived(const QString &bookId, int page);

private:
    void setupUi();
    void updatePageLabel();

    AuthManager *m_authManager;
    QString m_currentUsername;
    QString m_currentBookId;
    double m_zoomFactor = 1.0;

    QPdfDocument *m_document;
    QPdfView *m_pdfView;

    QLabel *m_titleLabel;
    QLabel *m_pageLabel;
    QSpinBox *m_pageSpinBox;
    QPushButton *m_prevButton;
    QPushButton *m_nextButton;
    QPushButton *m_goToPageButton;
    QPushButton *m_zoomInButton;
    QPushButton *m_zoomOutButton;
    QPushButton *m_backButton;
};

#endif // PDFREADERPAGE_H
