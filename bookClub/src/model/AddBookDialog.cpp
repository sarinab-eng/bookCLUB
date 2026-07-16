#include "addbookdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QApplication>

AddBookDialog::AddBookDialog(QWidget *parent) : QDialog(parent)
{
    setupManualUI();
    setWindowTitle("افزودن کتاب جدید");
    resize(450, 500);
}

void AddBookDialog::setupManualUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // ساخت ویجت‌ها
    titleLineEdit = new QLineEdit(this);
    authorLineEdit = new QLineEdit(this);
    genreComboBox = new QComboBox(this);
    genreComboBox->addItems({"داستانی", "علمی", "آموزشی", "تاریخی", "کودک", "روانشناسی"});

    descriptionTextEdit = new QTextEdit(this);
    priceSpinBox = new QSpinBox(this);
    priceSpinBox->setRange(0, 10000000);
    priceSpinBox->setSuffix(" تومان");

    discountSpinBox = new QSpinBox(this);
    discountSpinBox->setRange(0, 100);
    discountSpinBox->setSuffix(" %");

    coverImageLabel = new QLabel("تصویر انتخاب نشده", this);
    coverImageLabel->setFixedSize(120, 160);
    coverImageLabel->setStyleSheet("border: 2px dashed gray;");
    coverImageLabel->setAlignment(Qt::AlignCenter);

    QPushButton *btnSelectCover = new QPushButton("انتخاب تصویر جلد", this);
    bookPdfPathLineEdit = new QLineEdit(this);
    bookPdfPathLineEdit->setPlaceholderText("مسیری انتخاب نشده...");
    bookPdfPathLineEdit->setReadOnly(true);
    QPushButton *btnSelectPdf = new QPushButton("انتخاب فایل PDF", this);

    // چیدمان در فرم
    formLayout->addRow("عنوان کتاب:", titleLineEdit);
    formLayout->addRow("نویسنده:", authorLineEdit);
    formLayout->addRow("ژانر:", genreComboBox);
    formLayout->addRow("توضیحات:", descriptionTextEdit);
    formLayout->addRow("قیمت فروش:", priceSpinBox);
    formLayout->addRow("تخفیف:", discountSpinBox);
    formLayout->addRow("فایل جلد:", btnSelectCover);
    formLayout->addRow("", coverImageLabel);
    formLayout->addRow("فایل اصلی PDF:", btnSelectPdf);
    formLayout->addRow("", bookPdfPathLineEdit);

    // دکمه‌های عملیاتی
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton("ثبت کتاب", this);
    addBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;");
    QPushButton *cancelBtn = new QPushButton("انصراف", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(addBtn);
    buttonLayout->addWidget(cancelBtn);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    // اتصال سیگنال‌ها (چون دیگر فایل UI نداریم)
    connect(btnSelectCover, &QPushButton::clicked, this, &AddBookDialog::on_selectCoverImageButton_clicked);
    connect(btnSelectPdf, &QPushButton::clicked, this, &AddBookDialog::on_selectBookPdfButton_clicked);
    connect(addBtn, &QPushButton::clicked, this, &AddBookDialog::on_addButton_clicked);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

QString AddBookDialog::copyFileToProjectDir(const QString &sourcePath, const QString &destSubDir) {
    if (sourcePath.isEmpty()) return "";
    QString destDir = qApp->applicationDirPath() + "/" + destSubDir;
    QDir().mkpath(destDir);
    QString destPath = destDir + "/" + QFileInfo(sourcePath).fileName();
    if (QFile::exists(destPath)) QFile::remove(destPath);
    QFile::copy(sourcePath, destPath);
    return destPath;
}

Book AddBookDialog::getFinalBook() const {
    Book b;
    b.setTitle(titleLineEdit->text().trimmed());
    b.setAuthor(authorLineEdit->text().trimmed());
    b.setGenre(genreComboBox->currentText());
    b.setDescription(descriptionTextEdit->toPlainText().trimmed());
    b.setPrice(priceSpinBox->value());
    b.setDiscountPercentage(discountSpinBox->value());
    b.setFilePath(m_bookPdfPath);
    // b.setCoverPath(m_coverImagePath);
    b.setSalesCount(0);
    b.setLastReadPage(1);
    return b;
}

void AddBookDialog::on_selectCoverImageButton_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "انتخاب کاور", "", "Images (*.png *.jpg *.jpeg)");
    if(!path.isEmpty()) {
        m_coverImagePath = copyFileToProjectDir(path, "data/covers");
        QPixmap pix(m_coverImagePath);
        coverImageLabel->setPixmap(pix.scaled(coverImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        coverImageLabel->setText("");
    }
}

void AddBookDialog::on_selectBookPdfButton_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "انتخاب PDF", "", "PDF Files (*.pdf)");
    if(!path.isEmpty()) {
        m_bookPdfPath = copyFileToProjectDir(path, "data/books");
        bookPdfPathLineEdit->setText(QFileInfo(m_bookPdfPath).fileName());
    }
}

void AddBookDialog::on_addButton_clicked() {
    if(titleLineEdit->text().isEmpty() || m_bookPdfPath.isEmpty()) {
        QMessageBox::warning(this, "خطا در فرم", "تکمیل عنوان کتاب و انتخاب فایل PDF الزامی است.");
        return;
    }
    accept();
}
