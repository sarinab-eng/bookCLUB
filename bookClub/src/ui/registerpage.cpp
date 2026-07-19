#include "registerpage.h"
#include "ui_registerpage.h"
#include <QMessageBox>

RegisterPage::RegisterPage(QWidget *parent) : QWidget(parent), ui(new Ui::RegisterPage) {
    ui->setupUi(this);
    ui->userTypeComboBox->addItem("Customer");
    ui->userTypeComboBox->addItem("Publisher");
}

void RegisterPage::on_registerButton_clicked() {
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString confirm = ui->confirmPasswordLineEdit->text();
    QString userType = ui->userTypeComboBox->currentText();
    QString question = "Who's your favorite author?";
    QString answer = ui->securityAnswerLineEdit->text().trimmed();

    QVector<int> genres; // فعلاً خالی

    if (username.isEmpty() || password.isEmpty() || confirm.isEmpty() || answer.isEmpty()) {
        QMessageBox::warning(this, "Error", "Fill all fields.");
        return;
    }
    if (password != confirm) {
        QMessageBox::warning(this, "Error", "Passwords match fail.");
        return;
    }

    emit registerRequested(username, password, question, answer, userType, genres);
}

void RegisterPage::on_goToLoginButton_clicked() { emit goToLoginRequested(); }
RegisterPage::~RegisterPage() { delete ui; }