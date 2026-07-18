#include "customerpage.h"
#include "ui_customerpage.h"

CustomerPage::CustomerPage(AuthManager *authManager, QWidget *parent)
    : QWidget(parent), ui(new Ui::CustomerPage), m_authManager(authManager)
{
    ui->setupUi(this);
    connect(ui->logoutButton,    &QPushButton::clicked, this, &CustomerPage::onLogout);
    connect(ui->logoutTopButton, &QPushButton::clicked, this, &CustomerPage::onLogout);
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