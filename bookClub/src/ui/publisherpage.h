#ifndef PUBLISHERPAGE_H
#define PUBLISHERPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QJsonArray>
#include <QJsonObject>
#include "Authmanager.h"

// پنل ناشر: مدیریت حساب، مدیریت کتاب‌های منتشرشده و داشبورد آمار فروش
class PublisherPage : public QWidget
{
    Q_OBJECT
public:
    explicit PublisherPage(AuthManager *authManager, QWidget *parent = nullptr);

    void setUsername(const QString &username);
    void refresh();

signals:
    void logoutRequested();

private slots:
    void onProfileReceived(const QJsonObject &profile);
    void onPasswordChanged(bool success, const QString &message);
    void onChangePasswordClicked();

    void onPublisherBooksReceived(const QJsonArray &books);
    void onAddBookClicked();
    void onEditBookClicked();
    void onToggleActiveClicked();
    void onBookPublished(bool success, const QString &message, const QJsonObject &book);
    void onBookUpdated(bool success, const QString &message);
    void onBookActiveStatusChanged(bool success, const QString &message);

    void onPublisherStatsReceived(const QJsonObject &stats);

private:
    void setupUi();
    QJsonObject selectedBook() const;
    void populateStatsTable(QTableWidget *table, const QJsonArray &books);

    AuthManager *m_authManager;
    QString m_username;
    QJsonArray m_myBooks;

    // ---- تب حساب کاربری ----
    QLabel *m_usernameValueLabel;
    QLabel *m_roleValueLabel;
    QLabel *m_registrationDateValueLabel;
    QLineEdit *m_oldPasswordEdit;
    QLineEdit *m_newPasswordEdit;
    QLineEdit *m_confirmPasswordEdit;
    QPushButton *m_changePasswordButton;

    // ---- تب کتاب‌های من ----
    QTableWidget *m_booksTable;
    QPushButton *m_addBookButton;
    QPushButton *m_editBookButton;
    QPushButton *m_toggleActiveButton;

    // ---- تب آمار ----
    QLabel *m_totalBooksLabel;
    QLabel *m_totalRevenueLabel;
    QTableWidget *m_bestSellersTable;
    QTableWidget *m_worstSellersTable;
};

#endif // PUBLISHERPAGE_H
