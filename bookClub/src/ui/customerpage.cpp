#include "customerpage.h"
#include "ui_customerpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>

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
    homeLayout->addWidget(ui->newBooksLabel);
    homeLayout->addWidget(ui->newBooksScrollArea);
    homeLayout->addWidget(ui->bestSellerLabel);
    homeLayout->addWidget(ui->bestSellerScrollArea);
    homeLayout->addWidget(ui->freeBooksLabel);
    homeLayout->addWidget(ui->freeBooksScrollArea);

    // ساخت layout افقی برای هر کانتینر تا کارت‌های کتاب کنارهم چیده شوند
    m_recommendedLayout = new QHBoxLayout(ui->recommendedContainer);
    m_newBooksLayout    = new QHBoxLayout(ui->newBooksContainer);
    m_bestSellerLayout  = new QHBoxLayout(ui->bestSellerContainer);
    m_freeBooksLayout   = new QHBoxLayout(ui->freeBooksContainer);
    for (QHBoxLayout *l : {m_recommendedLayout, m_newBooksLayout, m_bestSellerLayout, m_freeBooksLayout}) {
        l->setContentsMargins(4, 4, 4, 4);
        l->addStretch();
    }

    connect(m_authManager, &AuthManager::booksReceived, this, &CustomerPage::onBooksReceived);

    // ۲) ساخت stacked widget و افزودن homePage + صفحات خالی
    m_stack = new QStackedWidget(ui->contentFrame);
    m_cartPage = new CartPage(m_authManager, this);
    m_libraryPage = new LibraryPage(m_authManager, this);

    connect(m_cartPage, &CartPage::checkoutSuccessful, m_libraryPage, &LibraryPage::requestLibraryRefresh);

    m_stack->addWidget(homePage);              // index 0 - Home
    m_stack->addWidget(new QWidget(m_stack));  // index 1 - Search
    m_stack->addWidget(m_libraryPage);
    m_stack->addWidget(m_cartPage);
    m_stack->addWidget(new QWidget(m_stack));  // index 4 - Profile
    m_stack->addWidget(new QWidget(m_stack));  // index 5 - History
    m_stack->addWidget(new QWidget(m_stack));  // index 6 - Settings

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
        // todo
        // m_libraryPage->requestLibraryRefresh(); // اولین لود داده‌ها به محض ورود کاربر
    }

    if (m_authManager) m_authManager->requestBooks();
}

QWidget *CustomerPage::createBookCard(const QJsonObject &book) {
    QFrame *card = new QFrame;
    card->setFrameShape(QFrame::StyledPanel);
    card->setFixedSize(150, 90);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(6, 6, 6, 6);

    QLabel *titleLabel = new QLabel(book["title"].toString());
    titleLabel->setStyleSheet("font-weight: bold;");
    titleLabel->setWordWrap(true);

    QLabel *authorLabel = new QLabel(book["author"].toString());
    authorLabel->setWordWrap(true);

    double price = book["price"].toDouble();
    QLabel *priceLabel = new QLabel(price > 0 ? QString("%1 تومان").arg(price) : "رایگان");

    layout->addWidget(titleLabel);
    layout->addWidget(authorLabel);
    layout->addWidget(priceLabel);

    return card;
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
    QJsonArray newBooks, bestSellers, freeBooks;

    for (const QJsonValue &v : books) {
        QJsonObject book = v.toObject();
        if (book["isNew"].toBool()) newBooks.append(book);
        if (book["isBestseller"].toBool()) bestSellers.append(book);
        if (book["isFree"].toBool()) freeBooks.append(book);
    }

    // todo منطق پیشنهاد بر اساس ژانر مورد علاقه هنوز سمت کلاینت پیاده نشده،
    // فعلاً کل لیست کتاب‌ها به عنوان پیشنهادی نمایش داده می‌شود
    populateSection(m_recommendedLayout, books);
    populateSection(m_newBooksLayout, newBooks);
    populateSection(m_bestSellerLayout, bestSellers);
    populateSection(m_freeBooksLayout, freeBooks);
}

void CustomerPage::onLogout() {
    emit logoutRequested();
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

void CustomerPage::on_profileButton_clicked()  { m_stack->setCurrentIndex(4); }
void CustomerPage::on_historyButton_clicked()  { m_stack->setCurrentIndex(5); }
void CustomerPage::on_settingsButton_clicked() { m_stack->setCurrentIndex(6); }
