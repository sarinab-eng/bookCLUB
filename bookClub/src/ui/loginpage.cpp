#include "loginpage.h"
#include "ui_loginpage.h"
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setXOffset(0);
    shadow->setYOffset(8);
    shadow->setColor(QColor(0, 0, 0, 40));
    ui->mainFrame->setGraphicsEffect(shadow);

    /*ui->usernameLineEdit->addAction(QIcon(":/icons/user.png"), QLineEdit::LeadingPosition);
    ui->passwordLineEdit->addAction(QIcon(":/icons/lock.png"), QLineEdit::LeadingPosition);*/
}

LoginPage::~LoginPage() {
    delete ui;
}

void LoginPage::on_loginButton_clicked() {
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "لطفاً تمام فیلدها را پر کنید");
        return;
    }

    emit loginRequested(username, password);
}

void LoginPage::on_goToRegisterButton_clicked() {
    emit registerRequested();
}

void LoginPage::on_forgotPasswordButton_clicked() {
    QString username = ui->usernameLineEdit->text().trimmed();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "خطا", "ابتدا نام کاربری را وارد کنید");
        return;
    }
    emit forgotPasswordRequested(username);
}