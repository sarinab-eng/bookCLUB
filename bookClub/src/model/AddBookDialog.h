#ifndef ADDBOOKDIALOG_H
#define ADDBOOKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include "Book.h"

class AddBookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddBookDialog(QWidget *parent = nullptr);
    Book getFinalBook() const;
    void setInitialData(const Book &book);

private slots:
    void on_selectCoverImageButton_clicked();
    void on_selectBookPdfButton_clicked();
    void on_addButton_clicked();

private:
    // تعریف ویجت‌ها به صورت مستقیم (جایگزین فایل UI)
    QLineEdit *titleLineEdit;
    QLineEdit *authorLineEdit;
    QComboBox *genreComboBox;
    QTextEdit *descriptionTextEdit;
    QSpinBox *priceSpinBox;
    QSpinBox *discountSpinBox;
    QLabel *coverImageLabel;
    QLineEdit *bookPdfPathLineEdit;

    QString m_coverImagePath;
    QString m_bookPdfPath;

    void setupManualUI(); // تابع ساخت ظاهر برنامه با کد
    QString copyFileToProjectDir(const QString &sourcePath, const QString &destSubDir);
};

#endif
