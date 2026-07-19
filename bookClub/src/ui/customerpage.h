#ifndef CUSTOMERPAGE_H
#define CUSTOMERPAGE_H
#include "genreselectiondialog.h"
#include <QWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include "Authmanager.h"
#include "cartpage.h"
#include "librarypage.h"



namespace Ui { class CustomerPage; }

class CustomerPage : public QWidget
{
    Q_OBJECT

public:
    explicit CustomerPage(AuthManager *authManager, QWidget *parent = nullptr);
    ~CustomerPage();

    void setUsername(const QString &username);

signals:
    void logoutRequested();

private slots:
    void onLogout();
    void onBooksReceived(const QJsonArray &books);

    // این اسلات‌ها طبق قرارداد نام‌گذاری Qt، خودکار به دکمه‌های سایدبار وصل می‌شوند
    void on_homeButton_clicked();
    void on_searchButton_clicked();
    void on_libraryButton_clicked();
    void on_cartButton_clicked();
    void on_profileButton_clicked();
    void on_historyButton_clicked();
    void on_settingsButton_clicked();

private:
    Ui::CustomerPage *ui;
    AuthManager *m_authManager;
    QString m_username;
    QStackedWidget *m_stack;
    CartPage *m_cartPage;
    LibraryPage *m_libraryPage;

    QHBoxLayout *m_recommendedLayout;
    QHBoxLayout *m_newBooksLayout;
    QHBoxLayout *m_bestSellerLayout;
    QHBoxLayout *m_freeBooksLayout;

    QWidget *createBookCard(const QJsonObject &book);
    void populateSection(QHBoxLayout *layout, const QJsonArray &books);
};

#endif
