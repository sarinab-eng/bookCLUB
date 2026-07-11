#include "UserDashboard.h"
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QVBoxLayout>

UserDashboard::UserDashboard(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void UserDashboard::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // بخش جستجو و فیلتر
    QHBoxLayout *topLayout = new QHBoxLayout();
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("جستجو بر اساس نام کتاب، نویسنده یا ناشر...");
    connect(searchLineEdit, &QLineEdit::textChanged, this, &UserDashboard::onSearchTextChanged);

    filterComboBox = new QComboBox(this);
    filterComboBox->addItems({"همه کتاب‌ها",
                              "محبوب‌ترین‌ها",
                              "جدیدترین‌ها",
                              "رایگان‌ها",
                              "مطابق ژانر من"});
    connect(filterComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &UserDashboard::onFilterChanged);

    QPushButton *genreBtn = new QPushButton("تغییر ژانرهای مورد علاقه", this);
    connect(genreBtn, &QPushButton::clicked, this, &UserDashboard::onGenrePreferenceSelected);

    topLayout->addWidget(searchLineEdit);
    topLayout->addWidget(filterComboBox);
    topLayout->addWidget(genreBtn);
    mainLayout->addLayout(topLayout);

    // بخش اسکرول کارت‌های کتاب
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    booksScrollWidget = new QWidget();
    booksLayout = new QGridLayout(booksScrollWidget);
    scrollArea->setWidget(booksScrollWidget);

    mainLayout->addWidget(scrollArea);
}

void UserDashboard::refreshBooks(const QVector<Book> &allBooks)
{
    m_allBooks = allBooks;
    displayBooks(m_allBooks);
}

void UserDashboard::displayBooks(const QVector<Book> &booksToDisplay)
{
    // پاک کردن کارت‌های قدیمی از لی‌اوت
    QLayoutItem *item;
    while ((item = booksLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    int row = 0, col = 0;
    for (const auto &book : booksToDisplay) {
        QWidget *card = new QWidget(this);
        card->setStyleSheet("background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: "
                            "8px; padding: 10px;");
        QVBoxLayout *cardLayout = new QVBoxLayout(card);

        QLabel *titleLbl = new QLabel(book.getTitle(), card);
        titleLbl->setStyleSheet("font-weight: bold; font-size: 16px;");
        QLabel *authorLbl = new QLabel("نویسنده: " + book.getAuthor(), card);
        QLabel *priceLbl = new QLabel(book.getPrice() == 0 ? "رایگان"
                                                           : QString("%1 $").arg(book.getPrice()),
                                      card);
        priceLbl->setStyleSheet("color: #28a745; font-weight: bold;");

        QPushButton *viewBtn = new QPushButton("مشاهده جزئیات", card);
        connect(viewBtn, &QPushButton::clicked, this, [this, book]() { emit bookSelected(book); });

        cardLayout->addWidget(titleLbl);
        cardLayout->addWidget(authorLbl);
        cardLayout->addWidget(priceLbl);
        cardLayout->addWidget(viewBtn);

        booksLayout->addWidget(card, row, col);
        col++;
        if (col >= 3) { // نمایش در ۳ ستون
            col = 0;
            row++;
        }
    }
}

void UserDashboard::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        displayBooks(m_allBooks);
        return;
    }

    QVector<Book> filtered;
    for (const auto &book : m_allBooks) {
        if (book.getTitle().contains(text, Qt::CaseInsensitive)
            || book.getAuthor().contains(text, Qt::CaseInsensitive)
            || book.getPublisher().contains(text, Qt::CaseInsensitive)) {
            filtered.append(book);
        }
    }
    displayBooks(filtered);
}

void UserDashboard::onFilterChanged(int index)
{
    QVector<Book> filtered = m_allBooks;
    if (index
        == 1) { // محبوب‌ترین‌ها (بر اساس امتیاز بالاتر از ۴)
        filtered.erase(std::remove_if(filtered.begin(),
                                      filtered.end(),
                                      [](const Book &b) { return b.getRating() < 4.0; }),
                       filtered.end());
    } else if (index == 3) { // رایگان‌ها
        filtered.erase(std::remove_if(filtered.begin(),
                                      filtered.end(),
                                      [](const Book &b) { return b.getPrice() > 0.0; }),
                       filtered.end());
    } else if (index == 4) { // مطابق با ژانرهای مورد علاقه کاربر
        filtered.erase(std::remove_if(filtered.begin(),
                                      filtered.end(),
                                      [this](const Book &b) {
                                          return !m_preferredGenres.contains(b.getGenre());
                                      }),
                       filtered.end());
    }
    displayBooks(filtered);
}

void UserDashboard::onGenrePreferenceSelected()
{
    bool ok;
    QString genres
        = QInputDialog::getText(this,
                                "ژانرهای مورد علاقه",
                                "ژانرهای خود را وارد کنید (با کاما جدا کنید - حداکثر ۳ مورد):",
                                QLineEdit::Normal,
                                m_preferredGenres.join(", "),
                                &ok);
    if (ok && !genres.isEmpty()) {
        m_preferredGenres = genres.split(",", Qt::SkipEmptyParts);
        for (auto &g : m_preferredGenres)
            g = g.trimmed();

        if (m_preferredGenres.size() > 3) {
            m_preferredGenres = m_preferredGenres.mid(0, 3);
            QMessageBox::information(this, "پیام", "فقط ۳ ژانر اول ذخیره شد.");
        }
        onFilterChanged(filterComboBox->currentIndex());
    }
}
