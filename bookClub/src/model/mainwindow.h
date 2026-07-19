#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTcpSocket>
#include "ui/loginpage.h"
#include "ui/registerpage.h"
#include "network/Authmanager.h"
#include "ui/adminpage.h"
#include "ui/customerpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showLoginPage();
    void showRegisterPage();
    void onRegisterRequested(const QString &username, const QString &password,
                             const QString &question, const QString &answer,
                             const QString &role, const QVector<int> &genres);
    void onLoginRequested(const QString &username, const QString &password);
    void handleLoginResult(bool success, const QString &message, const QString &role, bool firstLogin);
    void handleRegisterResult(bool success, const QString &message);
    void handleGenresSaved(bool success);
    void onForgotPasswordRequested(const QString &username);
    void handleSecurityQuestion(bool success, const QString &question);
    void handleSecurityAnswerVerified(bool success);
    void handlePasswordResetFinished(bool success);

private:
    Ui::MainWindow *ui;
    QStackedWidget *m_stackedWidget;
    LoginPage *m_loginPage;
    RegisterPage *m_registerPage;
    AdminPage *m_adminPage;
    CustomerPage *m_customerPage;
    QWidget *m_publisherPage;
    QString m_currentUsername;
    QTcpSocket *m_socket;
    AuthManager *m_authManager;
    QString m_resetUsername;

    void setupConnections();
};
#endif