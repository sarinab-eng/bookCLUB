#include "customerpage.h"
#include "ui_customerpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QListWidgetItem>
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QScrollArea>
#include <functional>

class ClickableFrame : public QFrame {
public:
    std::function<void()> onClick;
protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (onClick) onClick();
        QFrame::mousePressEvent(event);
    }
};

CustomerPage::CustomerPage(AuthManager *authManager, QWidget *parent)
    : QWidget(parent), ui(new Ui::CustomerPage), m_authManager(authManager)
{
    ui->setupUi(this);

    connect(ui->logoutButton,    &QPushButton::clicked, this, &CustomerPage::onLogout);

    QWidget *homePage = new QWidget(this);
    QVBoxLayout *homeLayout = new QVBoxLayout(homePage);
    homeLayout->setContentsMargins(0, 0, 0, 0);

    homeLayout->addWidget(ui->recommendedLabel);
    homeLayout->addWidget(ui->recommendedScrollArea);
    homeLayout->addWidget(ui->genreFilterLabel);
    ui->genreFilterComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui->genreFilterComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    homeLayout->addWidget(ui->genreFilterComboBox, 0, Qt::AlignLeft);
    homeLayout->addWidget(ui->genreFilterScrollArea);
    homeLayout->addWidget(ui->popularLabel);
    homeLayout->addWidget(ui->popularScrollArea);
    homeLayout->addWidget(ui->newBooksLabel);
    homeLayout->addWidget(ui->newBooksScrollArea);
    homeLayout->addWidget(ui->bestSellerLabel);
    homeLayout->addWidget(ui->bestSellerScrollArea);
    homeLayout->addWidget(ui->freeBooksLabel);
    homeLayout->addWidget(ui->freeBooksScrollArea);

    for (QScrollArea *sectionArea : {ui->recommendedScrollArea, ui->genreFilterScrollArea,
                                      ui->popularScrollArea, ui->newBooksScrollArea,
                                      ui->bestSellerScrollArea, ui->freeBooksScrollArea}) {
        sectionArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        sectionArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        sectionArea->setFixedHeight(210);
    }

    m_recommendedLayout = new QHBoxLayout(ui->recommendedContainer);
    m_genreFilterLayout = new QHBoxLayout(ui->genreFilterContainer);
    m_popularLayout     = new QHBoxLayout(ui->popularContainer);
    m_newBooksLayout    = new QHBoxLayout(ui->newBooksContainer);
    m_bestSellerLayout  = new QHBoxLayout(ui->bestSellerContainer);
    m_freeBooksLayout   = new QHBoxLayout(ui->freeBooksContainer);
    for (QHBoxLayout *l : {m_recommendedLayout, m_genreFilterLayout, m_popularLayout, m_newBooksLayout, m_bestSellerLayout, m_freeBooksLayout}) {
        l->setContentsMargins(4, 4, 4, 4);
        l->addStretch();
    }

    connect(ui->genreFilterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CustomerPage::onGenreFilterChanged);

    connect(m_authManager, &AuthManager::booksReceived, this, &CustomerPage::onBooksReceived);
    connect(m_authManager, &AuthManager::profileReceived, this, &CustomerPage::onProfileReceived);
    connect(m_authManager, &AuthManager::favoriteToggled, this,
            [this](bool success, const QString &message, const QString &bookId, const QString &action) {
                if (success) {
                    QMessageBox::information(this, "علاقه‌مندی‌ها", message);
                } else {
                    QMessageBox::warning(this, "خطا", message);
                }
            });


    m_stack = new QStackedWidget(ui->contentFrame);
    m_cartPage = new CartPage(m_authManager, this);
    m_libraryPage = new LibraryPage(m_authManager, this);
    m_profilePage = new ProfilePage(m_authManager, this);

    connect(m_cartPage, &CartPage::checkoutSuccessful, m_libraryPage, &LibraryPage::requestLibraryRefresh);

    QScrollArea *homeScrollArea = new QScrollArea;
    homeScrollArea->setWidgetResizable(true);
    homeScrollArea->setFrameShape(QFrame::NoFrame);
    homeScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    homeScrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    homeScrollArea->setWidget(homePage);

    m_stack->addWidget(homeScrollArea);

    QWidget *searchPage = new QWidget;
    QVBoxLayout *searchLayout = new QVBoxLayout(searchPage);

    QHBoxLayout *searchBar = new QHBoxLayout;
    m_searchInput = new QLineEdit;
    m_searchInput->setPlaceholderText("Search books...");
    m_searchField = new QComboBox;
    m_searchField->addItems({"title", "author", "genre"});
    QPushButton *goBtn = new QPushButton("Search");
    searchBar->addWidget(m_searchInput);
    searchBar->addWidget(m_searchField);
    searchBar->addWidget(goBtn);

    connect(goBtn, &QPushButton::clicked, this, &CustomerPage::onSearchGo);
    connect(m_authManager, &AuthManager::searchResultReceived,
            this, &CustomerPage::onSearchResults);

    m_searchResults = new QListWidget;
    searchLayout->addLayout(searchBar);
    searchLayout->addWidget(m_searchResults);

    m_stack->addWidget(searchPage);
    m_stack->addWidget(m_libraryPage);
    m_stack->addWidget(m_cartPage);
    m_stack->addWidget(m_profilePage);
    m_stack->addWidget(new QWidget(m_stack));

    m_bookDetailPage = new BookDetailPage(m_authManager, m_stack);
    m_stack->addWidget(m_bookDetailPage);
    connect(m_bookDetailPage, &BookDetailPage::toggleFavoriteRequested, this, [this](const QString &bookId) {
        m_authManager->toggleFavorite(m_username, bookId);
    });


#ifdef HAVE_QT_PDF
    m_pdfReaderPage = new PdfReaderPage(m_authManager, m_stack);
    m_stack->addWidget(m_pdfReaderPage);
#endif

    connect(m_searchResults, &QListWidget::itemClicked, this, [this](QListWidgetItem *item){
        QJsonObject b = QJsonObject::fromVariantMap(item->data(Qt::UserRole).toMap());
        openBookDetail(b);
    });

    connect(m_bookDetailPage, &BookDetailPage::backRequested, this, [this](){
        m_stack->setCurrentIndex(m_previousPageIndex);
    });

    connect(m_bookDetailPage, &BookDetailPage::addToCartRequested, this, [this](const QJsonObject &book){
        if (m_authManager && !m_username.isEmpty())
            m_authManager->addToCart(m_username, book["id"].toString());
    });
    connect(m_authManager, &AuthManager::itemAddedToCart, this, [this](bool success, const QString &message){
        if (success)
            QMessageBox::information(this, "سبد خرید", "کتاب به سبد خرید اضافه شد.");
        else
            QMessageBox::warning(this, "خطا", "افزودن به سبد خرید ناموفق بود: " + message);
    });

    connect(m_bookDetailPage, &BookDetailPage::saveForLaterRequested, this, [this](const QJsonObject &book){
        if (m_authManager && !m_username.isEmpty())
            m_authManager->saveBookForLater(m_username, book["id"].toString());
    });
    connect(m_authManager, &AuthManager::savedBookChanged, this, [this](bool success, const QString &message){
        if (success)
            QMessageBox::information(this, "کتابخانه شخصی", message);
        else
            QMessageBox::warning(this, "خطا", message);
    });

    connect(m_libraryPage, &LibraryPage::bookDetailRequested, this, &CustomerPage::openBookDetail);
    connect(authManager, &AuthManager::notificationReceived, this, &CustomerPage::handleNotification);
    connect(ui->notificationButton, &QPushButton::clicked, this, &CustomerPage::on_notificationButton_clicked);

    connect(m_libraryPage, &LibraryPage::readBookRequested, this, [this](const QJsonObject &book){
#ifdef HAVE_QT_PDF
        m_previousPageIndex = m_stack->currentIndex();
        m_pdfReaderPage->openBook(book);
        m_stack->setCurrentWidget(m_pdfReaderPage);
#else
        QString fileUrl = book.value("fileURL").toString();
        if (fileUrl.isEmpty()) {
            QMessageBox::information(this, "کتاب", "فایل PDF برای این کتاب تعریف نشده است.");
            return;
        }
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(fileUrl)))
            QMessageBox::warning(this, "خطا", "باز کردن فایل ممکن نشد: " + fileUrl);
#endif
    });
#ifdef HAVE_QT_PDF
    connect(m_pdfReaderPage, &PdfReaderPage::backRequested, this, [this](){
        m_stack->setCurrentIndex(m_previousPageIndex);
    });
#endif

    QLayout *oldLayout = ui->contentFrame->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr)
            delete item;
        delete oldLayout;
    }

    QVBoxLayout *contentLayout = new QVBoxLayout(ui->contentFrame);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->addWidget(m_stack);
}

CustomerPage::~CustomerPage() { delete ui; }

void CustomerPage::setUsername(const QString &username) {
    m_username = username;
    ui->welcomeLabel->setText(" welcome " + username);
    m_cartPage->setUsername(username);
    if (m_libraryPage) {
        m_libraryPage->setUsername(username);
        m_libraryPage->requestLibraryRefresh();
    }
    if (m_profilePage) {
        m_profilePage->setUsername(username);
    }
#ifdef HAVE_QT_PDF
    if (m_pdfReaderPage) {
        m_pdfReaderPage->setCurrentUsername(username);
    }
#endif

    if (m_authManager) {
        m_authManager->requestBooks();

        m_authManager->requestProfile(username);
    }
}

QWidget *CustomerPage::createBookCard(const QJsonObject &book) {
    ClickableFrame *card = new ClickableFrame;
    card->setFrameShape(QFrame::StyledPanel);
    card->setFixedSize(150, 190);
    card->setCursor(Qt::PointingHandCursor);
    card->setStyleSheet(
        "QFrame { background-color: #FFFFFF; border: 2px solid #FFC0CB; border-radius: 12px; }"
        "QFrame:hover { border-color: #FF69B4; }");

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(6, 6, 6, 6);

    QLabel *coverLabel = new QLabel;
    coverLabel->setFixedSize(136, 95);
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setScaledContents(false);
    QString coverPath = book["coverImage"].toString();
    QPixmap pixmap(coverPath);
    if (!coverPath.isEmpty() && !pixmap.isNull()) {
        coverLabel->setPixmap(pixmap.scaled(coverLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        coverLabel->setStyleSheet("border: none; background-color: transparent;");
    } else {
        coverLabel->setText("📖");
        coverLabel->setStyleSheet("border: none; background-color: #FFF0F5; border-radius: 8px; font-size: 40px;");
    }

    QLabel *titleLabel = new QLabel(book["title"].toString());
    titleLabel->setStyleSheet("font-weight: bold; color: #DB7093; border: none;");
    titleLabel->setWordWrap(true);

    QLabel *authorLabel = new QLabel(book["author"].toString());
    authorLabel->setWordWrap(true);
    authorLabel->setStyleSheet("border: none;");

    double price = book["price"].toDouble();
    QLabel *priceLabel = new QLabel(price > 0 ? QString("%1 تومان").arg(price) : "رایگان");
    priceLabel->setStyleSheet("border: none;");

    for (QLabel *label : {coverLabel, titleLabel, authorLabel, priceLabel})
        label->setAttribute(Qt::WA_TransparentForMouseEvents);

    layout->addWidget(coverLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(authorLabel);
    layout->addWidget(priceLabel);

    card->onClick = [this, book]() { openBookDetail(book); };

    return card;
}

void CustomerPage::openBookDetail(const QJsonObject &book) {
    m_previousPageIndex = m_stack->currentIndex();
    m_bookDetailPage->setCurrentUsername(m_username);
    m_bookDetailPage->setBookData(book);
    m_stack->setCurrentWidget(m_bookDetailPage); 
}

void CustomerPage::populateSection(QHBoxLayout *layout, const QJsonArray &books) {
    while (layout->count() > 1) {
        QLayoutItem *item = layout->takeAt(0);
        delete item->widget();
        delete item;
    }

    for (const QJsonValue &v : books) {
        layout->insertWidget(layout->count() - 1, createBookCard(v.toObject()));
    }
}

void CustomerPage::onBooksReceived(const QJsonArray &books) {
    m_allBooks = books;

    QJsonArray newBooks, bestSellers, freeBooks, popularBooks;

    for (const QJsonValue &v : books) {
        QJsonObject book = v.toObject();
        if (book["isNew"].toBool()) newBooks.append(book);
        if (book["isBestseller"].toBool()) bestSellers.append(book);
        if (book["isFree"].toBool()) freeBooks.append(book);
        if (book["isPopular"].toBool()) popularBooks.append(book);
    }

    populateSection(m_newBooksLayout, newBooks);
    populateSection(m_bestSellerLayout, bestSellers);
    populateSection(m_freeBooksLayout, freeBooks);
    populateSection(m_popularLayout, popularBooks);

    populateGenreFilterCombo(books);
    updateRecommendedSection();
}

void CustomerPage::onProfileReceived(const QJsonObject &profile) {
    if (profile.value("username").toString() != m_username) return;

    m_favoriteGenres.clear();
    for (const QJsonValue &v : profile.value("favoriteGenres").toArray())
        m_favoriteGenres.append(v.toString());

    updateRecommendedSection();
}

void CustomerPage::updateRecommendedSection() {
    if (m_favoriteGenres.isEmpty()) {
        populateSection(m_recommendedLayout, m_allBooks);
        return;
    }

    QJsonArray recommended;
    for (const QJsonValue &v : m_allBooks) {
        QJsonObject book = v.toObject();
        if (m_favoriteGenres.contains(book["genre"].toString()))
            recommended.append(book);
    }
    populateSection(m_recommendedLayout, recommended);
}

void CustomerPage::populateGenreFilterCombo(const QJsonArray &books) {
    QStringList genres;
    for (const QJsonValue &v : books) {
        QString genre = v.toObject()["genre"].toString();
        if (!genre.isEmpty() && !genres.contains(genre))
            genres.append(genre);
    }
    genres.sort();

    ui->genreFilterComboBox->blockSignals(true);
    ui->genreFilterComboBox->clear();
    ui->genreFilterComboBox->addItems(genres);
    ui->genreFilterComboBox->blockSignals(false);

    onGenreFilterChanged(ui->genreFilterComboBox->currentIndex());
}

void CustomerPage::onGenreFilterChanged(int index) {
    Q_UNUSED(index);
    QString genre = ui->genreFilterComboBox->currentText();

    QJsonArray filtered;
    for (const QJsonValue &v : m_allBooks) {
        QJsonObject book = v.toObject();
        if (book["genre"].toString() == genre)
            filtered.append(book);
    }
    populateSection(m_genreFilterLayout, filtered);
}

void CustomerPage::onLogout() {
    emit logoutRequested();
}

void CustomerPage::onSearchGo()
{
    QString q = m_searchInput->text().trimmed();
    if (q.isEmpty()) return;

    if (m_authManager) {
        m_authManager->searchBooks(q, m_searchField->currentText());
    }
}

void CustomerPage::onSearchResults(const QJsonArray &books)
{
    m_searchResults->clear();
    for (const QJsonValue &v : books) {
        QJsonObject b = v.toObject();
        QListWidgetItem *item = new QListWidgetItem(b["title"].toString() + " — " + b["author"].toString());
        item->setData(Qt::UserRole, b.toVariantMap());
        m_searchResults->addItem(item);
    }
}

void CustomerPage::on_homeButton_clicked()     { m_stack->setCurrentIndex(0); }
void CustomerPage::on_searchButton_clicked()   { m_stack->setCurrentIndex(1); }
void CustomerPage::on_libraryButton_clicked()
{
    m_stack->setCurrentIndex(2);
    m_libraryPage->requestLibraryRefresh();
}

void CustomerPage::on_cartButton_clicked() {
    m_stack->setCurrentIndex(3);
    m_cartPage->requestCartRefresh();
}

void CustomerPage::on_profileButton_clicked()
{
    m_stack->setCurrentIndex(4);
    m_profilePage->requestProfileRefresh();
}
void CustomerPage::on_historyButton_clicked()
{
    m_stack->setCurrentWidget(m_libraryPage);
    m_libraryPage->requestLibraryRefresh();
    m_libraryPage->showHistoryTab();
}

void CustomerPage::handleNotification(const QString &title, const QString &message) {
    m_notifications.append(qMakePair(title, message));
    updateNotificationButtonUI();

    // پاپ‌آپ لحظه‌ای
    QMessageBox::information(this, "🔔 " + title, message);
}

// کلیک روی دکمه اعلانات جهت مشاهده تمام اعلانات
void CustomerPage::on_notificationButton_clicked() {
    if (m_notifications.isEmpty()) {
        QMessageBox::information(this, "Notifications", "هیچ اعلان جدیدی ندارید");
        return;
    }

    QString text = "📋 لیست اعلانات شما:\n\n";
    for (int i = 0; i < m_notifications.size(); ++i) {
        text += QString("%1. %2\n   %3\n--------------------------------\n")
        .arg(i + 1)
            .arg(m_notifications[i].first)
            .arg(m_notifications[i].second);
    }

    QMessageBox::information(this, "Notofications", text);

    // پاکسازی لیست پس از مشاهده
    m_notifications.clear();
    updateNotificationButtonUI();
}

// به‌روزرسانی UI دکمه
void CustomerPage::updateNotificationButtonUI() {
    ui->notificationButton->setText(QString("🔔 Notifications (%1)").arg(m_notifications.size()));
}