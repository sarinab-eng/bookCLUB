#include "loginpage.h"
#include "ui_loginpage.h"
#include <QMessageBox>

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);
}

LoginPage::~LoginPage() {
    delete ui;
}

void LoginPage::on_loginButton_clicked() {
    // مطمئن شو در فایل .ui نام این لایِن‌ادیت‌ها دقیقاً همین باشد
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