#ifndef CARTPAGE_H
#define CARTPAGE_H

#include <QWidget>
#include <QJsonArray>
#include "Authmanager.h"

namespace Ui { class CartPage; }

class CartPage : public QWidget
{
    Q_OBJECT

public:
    explicit CartPage(AuthManager *authManager, QWidget *parent = nullptr);
    ~CartPage();
    void setUsername(const QString &username);

signals:
    void checkoutSuccessful();


public slots:
    void requestCartRefresh();

private slots:
    void onCartReceived(const QJsonArray &items);
    void onCartSummaryReceived(int itemCount, double total, double discountAmount, double finalAmount);
    void onCheckoutFinished(bool success, const QString &message,
                            double total, double discountAmount, double finalAmount);
    void on_removeButton_clicked();
    void on_checkoutButton_clicked();

private:
    Ui::CartPage *ui;
    AuthManager *m_authManager;
    QString m_username;

    void populateTable(const QJsonArray &items);
};

#endif
