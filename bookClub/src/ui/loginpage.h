#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>

namespace Ui {
class LoginPage;
}

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

signals:
    void loginRequested(const QString &username, const QString &password);
    void registerRequested();
    void forgotPasswordRequested(const QString &username);

private slots:
    void on_loginButton_clicked();
    void on_goToRegisterButton_clicked();
    void on_forgotPasswordButton_clicked();

private:
    Ui::LoginPage *ui;
};

#endif