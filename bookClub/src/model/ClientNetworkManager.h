#ifndef CLIENTNETWORKMANAGER_H
#define CLIENTNETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "ShoppingCart.h"
#include "Review.h"
#include "book.h"

class ClientNetworkManager : public QObject {
    Q_OBJECT
private:
    QTcpSocket *socket;
    static ClientNetworkManager *instance;
    int currentUserId;
    QString currentUsername;

    explicit ClientNetworkManager(QObject *parent = nullptr);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();

public:
    static ClientNetworkManager* getInstance();

    void connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;

    // ثبت اطلاعات کاربر جاری پس از لاگین موفق
    void setCurrentUser(int userId, const QString &username);
    int getCurrentUserId() const { return currentUserId; }
    QString getCurrentUsername() const { return currentUsername; }

    // متدهای ارسال درخواست به سرور (بخش کتاب و خرید)
    void sendPurchaseRequest(const ShoppingCart &cart);
    void sendReview(const Review &review);
    void requestPersonalLibrary();
    void requestAllBooks();

signals:
    // سیگنال‌ها برای اطلاع‌رسانی به رابط کاربری (UI)
    void allBooksReceived(const QVector<Book> &books);
    void personalLibraryReceived(const QVector<Book> &books);
    void purchaseStatusReceived(bool success, const QString &message);
    void reviewStatusReceived(bool success, const QString &message);
    void newReviewBroadcasted(const Review &review); // برای به‌روزرسانی آنی نظرات طبق داکیومنت
    void connectionStatusChanged(bool connected);
};

#endif // CLIENTNETWORKMANAGER_H
