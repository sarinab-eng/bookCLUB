#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTcpSocket>
#include "ui/loginpage.h"
#include "ui/registerpage.h"
#include "network/Authmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // اسلات‌های تغییر صفحه
    void showLoginPage();
    void showRegisterPage();

    // اسلات واسط ثبت‌نام (۶ پارامتر)
    void onRegisterRequested(const QString &username, const QString &password,
                             const QString &question, const QString &answer,
                             const QString &role, const QVector<int> &genres);

    // اسلات واسط ورود
    void onLoginRequested(const QString &username, const QString &password);

    // اسلات‌های دریافت پاسخ از سرور
    void handleLoginResult(bool success, const QString &message);
    void handleRegisterResult(bool success, const QString &message);

private:
    Ui::MainWindow *ui;
    QStackedWidget *m_stackedWidget;
    LoginPage *m_loginPage;
    RegisterPage *m_registerPage;

    QTcpSocket *m_socket;
    AuthManager *m_authManager;

    void setupConnections();
};
#endif