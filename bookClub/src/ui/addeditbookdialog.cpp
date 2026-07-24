#include "addeditbookdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QFileDialog>

static const QStringList kGenres = {
    "رمان", "علمی", "تاریخ", "کودک و نوجوان", "شعر",
    "زندگی‌نامه", "فلسفه", "روان‌شناسی", "ماجراجویی", "علمی-تخیلی", "عاشقانه"
};

AddEditBookDialog::AddEditBookDialog(const QJsonObject &existingBook, QWidget *parent)
    : QDialog(parent)
{
    setupUi(existingBook);
}

void AddEditBookDialog::setupUi(const QJsonObject &existingBook)
{
    bool isEdit = !existingBook.isEmpty();
    setWindowTitle(isEdit ? "ویرایش کتاب" : "افزودن کتاب جدید");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout;

    m_titleEdit = new QLineEdit(existingBook.value("title").toString());
    m_authorEdit = new QLineEdit(existingBook.value("author").toString());

    m_genreCombo = new QComboBox;
    m_genreCombo->addItems(kGenres);
    int genreIndex = kGenres.indexOf(existingBook.value("genre").toString());
    if (genreIndex >= 0) m_genreCombo->setCurrentIndex(genreIndex);

    m_descriptionEdit = new QTextEdit(existingBook.value("description").toString());
    m_descriptionEdit->setMaximumHeight(80);

    m_priceSpin = new QDoubleSpinBox;
    m_priceSpin->setRange(0, 100000000);
    m_priceSpin->setSuffix(" تومان");
    m_priceSpin->setValue(existingBook.value("price").toDouble());

    m_discountSpin = new QSpinBox;
    m_discountSpin->setRange(0, 100);
    m_discountSpin->setSuffix(" %");
    m_discountSpin->setValue(existingBook.value("discountPercent").toInt());

    m_coverPathEdit = new QLineEdit(existingBook.value("coverImage").toString());
    QPushButton *chooseCoverBtn = new QPushButton("انتخاب تصویر جلد...");
    QHBoxLayout *coverLayout = new QHBoxLayout;
    coverLayout->addWidget(m_coverPathEdit);
    coverLayout->addWidget(chooseCoverBtn);

    m_pdfPathEdit = new QLineEdit(existingBook.value("fileURL").toString());
    QPushButton *choosePdfBtn = new QPushButton("انتخاب فایل PDF...");
    QHBoxLayout *pdfLayout = new QHBoxLayout;
    pdfLayout->addWidget(m_pdfPathEdit);
    pdfLayout->addWidget(choosePdfBtn);

    form->addRow("نام کتاب:", m_titleEdit);
    form->addRow("نام نویسنده:", m_authorEdit);
    form->addRow("ژانر:", m_genreCombo);
    form->addRow("توضیحات:", m_descriptionEdit);
    form->addRow("قیمت:", m_priceSpin);
    form->addRow("درصد تخفیف:", m_discountSpin);
    form->addRow("عکس روی جلد:", coverLayout);
    form->addRow("فایل PDF:", pdfLayout);

    mainLayout->addLayout(form);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttons);

    connect(chooseCoverBtn, &QPushButton::clicked, this, &AddEditBookDialog::onChooseCoverClicked);
    connect(choosePdfBtn, &QPushButton::clicked, this, &AddEditBookDialog::onChoosePdfClicked);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void AddEditBookDialog::onChooseCoverClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "انتخاب تصویر جلد", QString(), "Images (*.png *.jpg *.jpeg)");
    if (!path.isEmpty()) m_coverPathEdit->setText(path);
}

void AddEditBookDialog::onChoosePdfClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "انتخاب فایل PDF", QString(), "PDF Files (*.pdf)");
    if (!path.isEmpty()) m_pdfPathEdit->setText(path);
}

QJsonObject AddEditBookDialog::bookData() const
{
    QJsonObject data;
    data["title"] = m_titleEdit->text().trimmed();
    data["author"] = m_authorEdit->text().trimmed();
    data["genre"] = m_genreCombo->currentText();
    data["description"] = m_descriptionEdit->toPlainText().trimmed();
    data["price"] = m_priceSpin->value();
    data["discountPercent"] = m_discountSpin->value();
    data["coverImage"] = m_coverPathEdit->text().trimmed();
    data["fileURL"] = m_pdfPathEdit->text().trimmed();
    return data;
}
