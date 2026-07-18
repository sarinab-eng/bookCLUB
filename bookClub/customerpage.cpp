#include "customerpage.h"
#include "ui_customerpage.h"
#include <QVBoxLayout>

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

    // ۲) ساخت stacked widget و افزودن homePage + صفحات خالی
    m_stack = new QStackedWidget(ui->contentFrame);
    m_stack->addWidget(homePage);              // index 0 - Home
    m_stack->addWidget(new QWidget(m_stack));  // index 1 - Search
    m_stack->addWidget(new QWidget(m_stack));  // index 2 - Library
    m_stack->addWidget(new QWidget(m_stack));  // index 3 - Cart
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

void CustomerPage::setUsername(const QString &username)
{
    ui->welcomeLabel->setText("خوش آمدید، " + username);
}

void CustomerPage::onLogout()
{
    emit logoutRequested();
}

void CustomerPage::on_homeButton_clicked()     { m_stack->setCurrentIndex(0); }
void CustomerPage::on_searchButton_clicked()   { m_stack->setCurrentIndex(1); }
void CustomerPage::on_libraryButton_clicked()  { m_stack->setCurrentIndex(2); }
void CustomerPage::on_cartButton_clicked()     { m_stack->setCurrentIndex(3); }
void CustomerPage::on_profileButton_clicked()  { m_stack->setCurrentIndex(4); }
void CustomerPage::on_historyButton_clicked()  { m_stack->setCurrentIndex(5); }
void CustomerPage::on_settingsButton_clicked() { m_stack->setCurrentIndex(6); }


void CustomerPage::onSearchGo() {
    QString q = m_searchInput->text().trimmed();
    if (q.isEmpty()) return;
    m_authManager->searchBooks(q, m_searchField->currentText());
}

void CustomerPage::onSearchResults(const QJsonArray &books) {
    m_searchResults->clear();
    for (const QJsonValue &v : books) {
        QJsonObject b = v.toObject();
        m_searchResults->addItem(
            b["title"].toString() + " — " + b["author"].toString()
            );
    }
}
