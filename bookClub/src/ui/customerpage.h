#ifndef CUSTOMERPAGE_H
#define CUSTOMERPAGE_H

#include <QWidget>
#include "AuthManager.h"

namespace Ui {
class CustomerPage;
}

class CustomerPage : public QWidget
{
    Q_OBJECT

public:
    explicit CustomerPage(AuthManager *authManager,
                          QWidget *parent = nullptr);
    ~CustomerPage();

private:
    Ui::CustomerPage *ui;

    AuthManager *m_authManager;
};

#endif