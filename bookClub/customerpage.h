#ifndef CUSTOMERPAGE_H
#define CUSTOMERPAGE_H

#include "GenreSelectionDialog.h"
#include <QWidget>
#include <QStackedWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QJsonArray>
#include "AuthManager.h"

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
    void on_homeButton_clicked();
    void on_searchButton_clicked();
    void on_libraryButton_clicked();
    void on_cartButton_clicked();
    void on_profileButton_clicked();
    void on_historyButton_clicked();
    void on_settingsButton_clicked();
    void onSearchGo();
    void onSearchResults(const QJsonArray &books);

private:
    Ui::CustomerPage *ui;
    AuthManager      *m_authManager;
    QStackedWidget   *m_stack;
    QLineEdit        *m_searchInput   = nullptr;
    QComboBox        *m_searchField   = nullptr;
    QListWidget      *m_searchResults = nullptr;
};

#endif
