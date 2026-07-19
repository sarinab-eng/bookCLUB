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
#include <functional>

// QFrame ساده که با کلیک، callback داده‌شده رو صدا می‌زند؛ برای کلیک‌پذیر
// کردن کارت‌های کتاب تو صفحه‌ی Home
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
    ui->setupUi(this); // اینجا connectSlotsByName هم اجرا می‌شود

    connect(ui->logoutButton,    &QPushButton::clicked, this, &CustomerPage::onLogout);
    connect(ui->logoutTopButton, &QPushButton::clicked, this, &CustomerPage::onLogout);

    // ---- ساخت QStackedWidget داخلی به‌صورت برنامه‌نویسی ----

    // ۱) انتقال محتوای فعلی contentFrame به یک صفحه‌ی مستقل (Home)
    QWidget *homePage = new QWidget(this);
    QVBoxLayout *homeLayout = new QVBoxLayout(homePage);
    homeLayout->setContentsMargins(0, 0, 0, 0);

    homeLayout->addWidget(ui->recommendedLabel);
    homeLayout->addWidget(ui->recommendedScrollArea);
    homeLayout->addWidget(ui->genreFilterLabel);
    homeLayout->addWidget(ui->genreFilterComboBox);
    homeLayout->addWidget(ui->genreFilterScrollArea);
    homeLayout->addWidget(ui->popularLabel);
    homeLayout->addWidget(ui->popularScrollArea);
    homeLayout->addWidget(ui->newBooksLabel);
    homeLayout->addWidget(ui->newBooksScrollArea);
    homeLayout->addWidget(ui->bestSellerLabel);
    homeLayout->addWidget(ui->bestSellerScrollArea);
    homeLayout->addWidget(ui->freeBooksLabel);
    homeLayout->addWidget(ui->freeBooksScrollArea);

    // ساخت layout افقی برای هر کانتینر تا کارت‌های کتاب کنارهم چیده شوند
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

    // ۲) ساخت stacked widget و افزودن homePage + صفحات خالی
    m_stack = new QStackedWidget(ui->contentFrame);
    m_cartPage = new CartPage(m_authManager, this);
    m_libraryPage = new LibraryPage(m_authManager, this);
    m_profilePage = new ProfilePage(m_authManager, this);

    connect(m_cartPage, &CartPage::checkoutSuccessful, m_libraryPage, &LibraryPage::requestLibraryRefresh);

    m_stack->addWidget(homePage);              // index 0 - Home

    // ساخت صفحه واقعی جست‌وجو (index 1)
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

    m_stack->addWidget(searchPage);            // index 1 - Search
    m_stack->addWidget(m_libraryPage);          // index 2 - Library
    m_stack->addWidget(m_cartPage);             // index 3 - Cart
    m_stack->addWidget(m_profilePage);          // index 4 - Profile
    m_stack->addWidget(new QWidget(m_stack));  // index 5 - History
    m_stack->addWidget(new QWidget(m_stack));  // index 6 - Settings

    m_bookDetailPage = new BookDetailPage(m_authManager, m_stack);
    m_stack->addWidget(m_bookDetailPage);      // index 7 - Book detail

    // کلیک روی نتیجه‌ی جست‌وجو، جزئیات کتاب رو باز می‌کنه
    connect(m_searchResults, &QListWidget::itemClicked, this, [this](QListWidgetItem *item){
        QJsonObject b = QJsonObject::fromVariantMap(item->data(Qt::UserRole).toMap());
        openBookDetail(b);
    });

    // اتصال دکمه بازگشتِ صفحه جزئیات به همون صفحه‌ای که کاربر ازش وارد شده بود
    connect(m_bookDetailPage, &BookDetailPage::backRequested, this, [this](){
        m_stack->setCurrentIndex(m_previousPageIndex);
    });

    // ۳) جایگزینی layout قدیمی contentFrame با stack
    QLayout *oldLayout = ui->contentFrame->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr)
            delete item; // ویجت‌ها قبلاً به homeLayout منتقل شده‌اند، حذف امن است
        delete oldLayout;
    }

    QVBoxLayout *contentLayout = new QVBoxLayout(ui->contentFrame);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->addWidget(m_stack);

    // نکته: دکمه‌های سایدبار نیازی به connect دستی ندارند (auto-connect فعال است)
}

CustomerPage::~CustomerPage() { delete ui; }

void CustomerPage::setUsername(const QString &username) {
    m_username = username;
    ui->welcomeLabel->setText("خوش آمدید، " + username);
    m_cartPage->setUsername(username);
    if (m_libraryPage) {
        m_libraryPage->setUsername(username);
        m_libraryPage->requestLibraryRefresh(); // اولین لود داده‌ها به محض ورود کاربر
    }
    if (m_profilePage) {
        m_profilePage->setUsername(username);
    }

    if (m_authManager) {
        m_authManager->requestBooks();

        m_authManager->requestProfile(username);
    }
}

QWidget *CustomerPage::createBookCard(const QJsonObject &book) {
    ClickableFrame *card = new ClickableFrame;
    card->setFrameShape(QFrame::StyledPanel);
    card->setFixedSize(150, 90);
    card->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(6, 6, 6, 6);

    QLabel *titleLabel = new QLabel(book["title"].toString());
    titleLabel->setStyleSheet("font-weight: bold;");
    titleLabel->setWordWrap(true);

    QLabel *authorLabel = new QLabel(book["author"].toString());
    authorLabel->setWordWrap(true);

    double price = book["price"].toDouble();
    QLabel *priceLabel = new QLabel(price > 0 ? QString("%1 تومان").arg(price) : "رایگان");

    // بدون این، کلیک روی خودِ لیبل‌ها می‌گیره و به کارتِ زیرشون (ClickableFrame) نمی‌رسه
    for (QLabel *label : {titleLabel, authorLabel, priceLabel})
        label->setAttribute(Qt::WA_TransparentForMouseEvents);

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
        // تا وقتی ژانرهای مورد علاقه از سرور نرسیده یا کاربر هیچ ژانری انتخاب نکرده،
        // کل لیست کتاب‌ها به عنوان پیشنهادی نمایش داده می‌شود
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
    m_libraryPage->requestLibraryRefresh(); // فراخوانی رفرش از سرور
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
void CustomerPage::on_historyButton_clicked()  { m_stack->setCurrentIndex(5); }
void CustomerPage::on_settingsButton_clicked() { m_stack->setCurrentIndex(6); }
