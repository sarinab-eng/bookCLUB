#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
//#include "genreselectiondialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    // ۱. تنظیم شبکه
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost("127.0.0.1", 1234);

    m_authManager = new AuthManager(m_socket, this);

    // ۲. ایجاد صفحات و مدیریت هویت
    m_loginPage = new LoginPage(this);
    m_registerPage = new RegisterPage(this);
    m_adminPage = new AdminPage(m_authManager, this); // فعلاً به عنوان جایگاه (Placeholder)
    m_publisherPage = new QWidget(this);
    m_customerPage = new CustomerPage(m_authManager, this);



    m_stackedWidget->addWidget(m_loginPage);     // Index 0
    m_stackedWidget->addWidget(m_registerPage);  // Index 1
    m_stackedWidget->addWidget(m_adminPage);     // Index 2
    m_stackedWidget->addWidget(m_publisherPage); // Index 3
    m_stackedWidget->addWidget(m_customerPage);  // Index 4

    setupConnections();
}

void MainWindow::setupConnections() {
    connect(m_loginPage, &LoginPage::registerRequested, this, &MainWindow::showRegisterPage);
    connect(m_registerPage, &RegisterPage::goToLoginRequested, this, &MainWindow::showLoginPage);
    connect(m_loginPage, &LoginPage::loginRequested, this, &MainWindow::onLoginRequested);
    connect(m_registerPage, &RegisterPage::registerRequested, this, &MainWindow::onRegisterRequested);
    connect(m_authManager, &AuthManager::loginFinished, this, &MainWindow::handleLoginResult);
    connect(m_authManager, &AuthManager::registerFinished, this, &MainWindow::handleRegisterResult);
    connect(m_authManager, &AuthManager::genresSaved, this, &MainWindow::handleGenresSaved);
    connect(m_customerPage, &CustomerPage::logoutRequested, this, [this]() {
        m_stackedWidget->setCurrentIndex(0);
    });
    connect(m_loginPage, &LoginPage::forgotPasswordRequested, this, &MainWindow::onForgotPasswordRequested);
    connect(m_authManager, &AuthManager::securityQuestionReceived, this, &MainWindow::handleSecurityQuestion);
    connect(m_authManager, &AuthManager::securityAnswerVerified, this, &MainWindow::handleSecurityAnswerVerified);
    connect(m_authManager, &AuthManager::passwordResetFinished, this, &MainWindow::handlePasswordResetFinished);
}

void MainWindow::onRegisterRequested(const QString &username, const QString &password,
                                     const QString &question, const QString &answer,
                                     const QString &role, const QVector<int> &genres) {
    m_authManager->registerUser(username, password, question, answer, role, genres);
}

void MainWindow::onLoginRequested(const QString &username, const QString &password) {
    m_currentUsername = username;
    qDebug() << "MainWindow received login request for:" << username;
    m_authManager->loginUser(username, password);
}

void MainWindow::handleLoginResult(bool success, const QString &message, const QString &role, bool firstLogin)
{
    if (!success) {
        QMessageBox::critical(this, "Error", message);
        return;
    }

    QString userRole = role.toLower();

    if (userRole == "admin") {

        m_stackedWidget->setCurrentWidget(m_adminPage);
        m_adminPage->loadData();

    }
    else if (userRole == "publisher") {

        m_stackedWidget->setCurrentWidget(m_publisherPage);

    }
    else if (userRole == "customer") {

        if(firstLogin)
        {
            GenreSelectionDialog dialog(this);

            if(dialog.exec()!=QDialog::Accepted)
                return;

            QStringList genres=dialog.selectedGenres();
            qDebug() << "Selected genres =" << genres;
            qDebug() << "Calling saveGenres...";
            m_authManager->saveGenres(m_currentUsername, genres);
            QCoreApplication::processEvents();
        }
        else
        {
            m_customerPage->setUsername(m_currentUsername);
            m_stackedWidget->setCurrentWidget(m_customerPage);
            QMessageBox::information(this,"Login","Welcome!");
        }
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

void MainWindow::handleGenresSaved(bool success)
{
    if(success)
    {
        qDebug() << "Switching to customer page";

        QMessageBox::information(this, "Success", "ژانرهای مورد علاقه ذخیره شد.");

        m_customerPage->setUsername(m_currentUsername);
        m_stackedWidget->setCurrentWidget(m_customerPage);
        QMessageBox::information(this,"Login","Welcome!");
        qDebug() << m_stackedWidget->currentWidget();
        qDebug() << m_customerPage;
    }
    else
    {
        QMessageBox::critical(this, "Error", "ذخیره ژانرها انجام نشد.");
    }
}

void MainWindow::onForgotPasswordRequested(const QString &username) {
    m_resetUsername = username;
    m_authManager->getSecurityQuestion(username);
}

void MainWindow::handleSecurityQuestion(bool success, const QString &question) {
    if (!success) {
        QMessageBox::critical(this, "خطا", "کاربر یافت نشد");
        return;
    }
    QString decryptedQ = User::decrypt(question);
    bool ok;
    QString answer = QInputDialog::getText(this, "بازیابی رمز عبور",
                                           decryptedQ + "\n\nجواب:", QLineEdit::Normal, "", &ok);
    if (!ok || answer.trimmed().isEmpty()) return;
    m_authManager->verifySecurityAnswer(m_resetUsername, answer.trimmed());
}

void MainWindow::handleSecurityAnswerVerified(bool success) {
    if (!success) {
        QMessageBox::critical(this, "خطا", "جواب اشتباه است");
        return;
    }
    bool ok;
    QString newPass = QInputDialog::getText(this, "رمز جدید",
                                            "رمز عبور جدید:", QLineEdit::Password, "", &ok);
    if (!ok || newPass.trimmed().isEmpty()) return;
    m_authManager->resetPassword(m_resetUsername, newPass.trimmed());
}

void MainWindow::handlePasswordResetFinished(bool success) {
    if (success)
        QMessageBox::information(this, "موفق", "رمز عبور با موفقیت تغییر کرد");
    else
        QMessageBox::critical(this, "خطا", "خطا در تغییر رمز عبور");
}

void MainWindow::showLoginPage() { m_stackedWidget->setCurrentWidget(m_loginPage); }
void MainWindow::showRegisterPage() { m_stackedWidget->setCurrentWidget(m_registerPage); }

MainWindow::~MainWindow() { delete ui; }