#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include "src/ui/genreselectiondialog.h"
#include "src/model/User.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::connected, this, [this]() {
        qDebug() << "Connected to server";
    });

    connect(m_socket, &QTcpSocket::errorOccurred, this, [this]() {
        qDebug() << "Socket error:" << m_socket->errorString();
    });

    m_socket->connectToHost("127.0.0.1", 1234);
    m_socket->waitForConnected(3000);

    m_authManager = new AuthManager(m_socket, this);

    m_loginPage    = new LoginPage(this);
    m_registerPage = new RegisterPage(this);
    m_adminPage    = new AdminPage(m_authManager, this);
    m_publisherPage = new PublisherPage(m_authManager, this);
    m_customerPage = new CustomerPage(m_authManager, this);

    m_stackedWidget->addWidget(m_loginPage);      // 0
    m_stackedWidget->addWidget(m_registerPage);   // 1
    m_stackedWidget->addWidget(m_adminPage);      // 2
    m_stackedWidget->addWidget(m_publisherPage);  // 3
    m_stackedWidget->addWidget(m_customerPage);   // 4

    setupConnections();
}

void MainWindow::setupConnections() {
    connect(m_loginPage,    &LoginPage::registerRequested,    this, &MainWindow::showRegisterPage);
    connect(m_registerPage, &RegisterPage::goToLoginRequested,this, &MainWindow::showLoginPage);
    connect(m_loginPage,    &LoginPage::loginRequested,       this, &MainWindow::onLoginRequested);
    connect(m_registerPage, &RegisterPage::registerRequested, this, &MainWindow::onRegisterRequested);

    connect(m_authManager, &AuthManager::loginFinished,    this, &MainWindow::handleLoginResult);
    connect(m_authManager, &AuthManager::registerFinished, this, &MainWindow::handleRegisterResult);
    connect(m_authManager, &AuthManager::genresSaved,      this, &MainWindow::handleGenresSaved);

    // logout — فقط یک‌بار اینجا
    connect(m_customerPage, &CustomerPage::logoutRequested, this, [this]() {
        m_currentUsername.clear();
        m_stackedWidget->setCurrentWidget(m_loginPage);
    });
    connect(m_publisherPage, &PublisherPage::logoutRequested, this, [this]() {
        m_currentUsername.clear();
        m_stackedWidget->setCurrentWidget(m_loginPage);
    });
    connect(m_adminPage, &AdminPage::logoutRequested, this, [this]() {
        m_currentUsername.clear();
        m_stackedWidget->setCurrentWidget(m_loginPage);
    });

    connect(m_loginPage, &LoginPage::forgotPasswordRequested,  this, &MainWindow::onForgotPasswordRequested);
    connect(m_authManager, &AuthManager::securityQuestionReceived, this, &MainWindow::handleSecurityQuestion);
    connect(m_authManager, &AuthManager::securityAnswerVerified,   this, &MainWindow::handleSecurityAnswerVerified);
    connect(m_authManager, &AuthManager::passwordResetFinished,    this, &MainWindow::handlePasswordResetFinished);
}

void MainWindow::onRegisterRequested(const QString &username, const QString &password,
                                     const QString &question, const QString &answer,
                                     const QString &role, const QVector<int> &genres) {
    m_authManager->registerUser(username, password, question, answer, role, genres);
}

void MainWindow::onLoginRequested(const QString &username, const QString &password) {
    m_currentUsername = username;
    m_authManager->loginUser(username, password);
}

void MainWindow::handleLoginResult(bool success, const QString &message, const QString &role, bool firstLogin) {
    if (!success) {
        QMessageBox::critical(this, "Error", message);
        return;
    }

    QString userRole = role.toLower();

    if (userRole == "admin") {
        m_stackedWidget->setCurrentWidget(m_adminPage);
        m_adminPage->loadData();
    } else if (userRole == "publisher") {
        m_publisherPage->setUsername(m_currentUsername);
        m_stackedWidget->setCurrentWidget(m_publisherPage);
    } else if (userRole == "customer") {
        if (firstLogin) {
            GenreSelectionDialog dialog(this);
            if (dialog.exec() != QDialog::Accepted) return;
            m_awaitingFirstLoginGenres = true;
            m_authManager->saveGenres(m_currentUsername, dialog.selectedGenres());
        } else {
            m_customerPage->setUsername(m_currentUsername);
            m_stackedWidget->setCurrentWidget(m_customerPage);
            QTimer::singleShot(0, this, [this]() {
                QMessageBox::information(this, "Login", "Welcome!");
            });
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

void MainWindow::handleGenresSaved(bool success) {
    if (!m_awaitingFirstLoginGenres) return;
    m_awaitingFirstLoginGenres = false;

    QTimer::singleShot(0, this, [this, success]() {
        if (success) {
            QMessageBox::information(this, "Success", "ژانرهای مورد علاقه ذخیره شد.");
            m_customerPage->setUsername(m_currentUsername);
            m_stackedWidget->setCurrentWidget(m_customerPage);
            QMessageBox::information(this, "Login", "Welcome!");
        } else {
            QMessageBox::critical(this, "Error", "ذخیره ژانرها انجام نشد.");
        }
    });
}

void MainWindow::onForgotPasswordRequested(const QString &username) {
    m_resetUsername = username;
    m_authManager->getSecurityQuestion(username);
}

void MainWindow::handleSecurityQuestion(bool success, const QString &question) {
    if (!success) { QMessageBox::critical(this, "خطا", "کاربر یافت نشد"); return; }
    bool ok;
    QString answer = QInputDialog::getText(this, "بازیابی رمز عبور",
                                           User::decrypt(question) + "\n\nجواب:",
                                           QLineEdit::Normal, "", &ok);
    if (!ok || answer.trimmed().isEmpty()) return;
    m_authManager->verifySecurityAnswer(m_resetUsername, answer.trimmed());
}

void MainWindow::handleSecurityAnswerVerified(bool success) {
    if (!success) { QMessageBox::critical(this, "خطا", "جواب اشتباه است"); return; }
    bool ok;
    QString newPass = QInputDialog::getText(this, "رمز جدید", "رمز عبور جدید:",
                                            QLineEdit::Password, "", &ok);
    if (!ok || newPass.trimmed().isEmpty()) return;
    m_authManager->resetPassword(m_resetUsername, newPass.trimmed());
}

void MainWindow::handlePasswordResetFinished(bool success) {
    if (success) QMessageBox::information(this, "موفق", "رمز عبور با موفقیت تغییر کرد");
    else         QMessageBox::critical(this, "خطا", "خطا در تغییر رمز عبور");
}

void MainWindow::showLoginPage()    { m_stackedWidget->setCurrentWidget(m_loginPage); }
void MainWindow::showRegisterPage() { m_stackedWidget->setCurrentWidget(m_registerPage); }

MainWindow::~MainWindow() { delete ui; }
