#include "customerpage.h"
#include "ui_customerpage.h"

CustomerPage::CustomerPage(AuthManager *authManager,
                           QWidget *parent)
    : QWidget(parent),
    ui(new Ui::CustomerPage),
    m_authManager(authManager)
{
    ui->setupUi(this);
}

CustomerPage::~CustomerPage()
{
    delete ui;
}