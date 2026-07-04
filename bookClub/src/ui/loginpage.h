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
    // سیگنالی که وقتی کاربر دکمه Login را می‌زند، به MainWindow ارسال می‌شود
    void loginRequested(const QString &username, const QString &password);

    // سیگنالی برای وقتی که کاربر می‌خواهد به صفحه ثبت‌نام برود
    void registerRequested();

private slots:
    // اسلات داخلی برای کلیک شدن دکمه ورود در فایل ui
    void on_loginButton_clicked();

    // اسلات داخلی برای دکمه "ثبت‌نام نکرده‌اید؟"
    void on_goToRegisterButton_clicked();

private:
    Ui::LoginPage *ui;
};

#endif // LOGINPAGE_H