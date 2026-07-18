#ifndef CUSTOMERPAGE_H
#define CUSTOMERPAGE_H
#include "GenreSelectionDialog.h"
#include <QWidget>
#include "AuthManager.h"

namespace Ui {class CustomerPage; }

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

private:
    Ui::CustomerPage *ui;
    AuthManager *m_authManager;
};

#endif