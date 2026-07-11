#ifndef PDFREADERWIDGET_H
#define PDFREADERWIDGET_H

#include <QPdfView>
#include <QPdfDocument>
#include <QWebEngineView>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QUrl>

class PdfReaderWidget : public QWidget {
    Q_OBJECT

private:
    QWebEngineView *m_webView;
    int m_bookId;
    QString m_pdfPath;
    QLabel *lblBookTitle;
    QPushButton *btnClose;

    void setupUi();

signals:
    void saveReadingProgress(int bookId, int lastPage);
    void closed();

private slots:
    void closeReader();

public:
    explicit PdfReaderWidget(QWidget *parent = nullptr);
    bool openBook(int bookId, const QString &title, const QString &filePath);
};

#endif // PDFREADERWIDGET_H
