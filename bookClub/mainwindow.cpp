#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    // ۱. تنظیم شبکه
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost("127.0.0.1", 1234); // آی‌پی و پورت سرور خود را چک کنید

    // ۲. ایجاد صفحات و مدیریت هویت
    m_loginPage = new LoginPage(this);
    m_registerPage = new RegisterPage(this);
    m_authManager = new AuthManager(m_socket, this); // ارسال سوکت به AuthManager

    m_stackedWidget->addWidget(m_loginPage);
    m_stackedWidget->addWidget(m_registerPage);

    setupConnections();
}

void MainWindow::setupConnections() {
    // اتصال دکمه‌های ناوبری بین صفحات
    connect(m_loginPage, &LoginPage::registerRequested, this, &MainWindow::showRegisterPage);
    connect(m_registerPage, &RegisterPage::goToLoginRequested, this, &MainWindow::showLoginPage);

    // اتصال درخواست‌های UI به متدهای MainWindow
    connect(m_loginPage, &LoginPage::loginRequested, this, &MainWindow::onLoginRequested);
    connect(m_registerPage, &RegisterPage::registerRequested, this, &MainWindow::onRegisterRequested);

    // اتصال نتایج AuthManager به هندلرهای MainWindow
    connect(m_authManager, &AuthManager::loginFinished, this, &MainWindow::handleLoginResult);
    connect(m_authManager, &AuthManager::registerFinished, this, &MainWindow::handleRegisterResult);
}

// پیاده‌سازی متد ثبت‌نام که به AuthManager پاس می‌دهد
void MainWindow::onRegisterRequested(const QString &username, const QString &password,
                                     const QString &question, const QString &answer,
                                     const QString &role, const QVector<int> &genres) {
    m_authManager->registerUser(username, password, question, answer, role, genres);
}

void MainWindow::onLoginRequested(const QString &username, const QString &password) {
    m_authManager->loginUser(username, password);
}

void MainWindow::handleLoginResult(bool success, const QString &message) {
    if (success) {
        QMessageBox::information(this, "Login", "Welcome!");
    } else {
        QMessageBox::critical(this, "Error", message);
    }
}

void MainWindow::handleRegisterResult(bool success, const QString &message) {
    if (success) {
        QMessageBox::information(this, "Success", "Registration successful. Please login.");
        showLoginPage();
    } else {
        QMessageBox::critical(this, "Error", message);
    }
}

void MainWindow::showLoginPage() { m_stackedWidget->setCurrentWidget(m_loginPage); }
void MainWindow::showRegisterPage() { m_stackedWidget->setCurrentWidget(m_registerPage); }

MainWindow::~MainWindow() { delete ui; }