#ifndef ADDEDITBOOKDIALOG_H
#define ADDEDITBOOKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QJsonObject>

// فرم مشترک افزودن کتاب جدید و ویرایش کتاب موجود در پنل ناشر
class AddEditBookDialog : public QDialog
{
    Q_OBJECT
public:
    // اگر existingBook خالی باشد یعنی حالت «افزودن کتاب جدید»، وگرنه فرم با
    // اطلاعات همان کتاب برای ویرایش پر می‌شود
    explicit AddEditBookDialog(const QJsonObject &existingBook = QJsonObject(), QWidget *parent = nullptr);

    QJsonObject bookData() const;

private slots:
    void onChooseCoverClicked();
    void onChoosePdfClicked();

private:
    void setupUi(const QJsonObject &existingBook);

    QLineEdit *m_titleEdit;
    QLineEdit *m_authorEdit;
    QComboBox *m_genreCombo;
    QTextEdit *m_descriptionEdit;
    QDoubleSpinBox *m_priceSpin;
    QSpinBox *m_discountSpin;
    QLineEdit *m_coverPathEdit;
    QLineEdit *m_pdfPathEdit;
};

#endif // ADDEDITBOOKDIALOG_H
