#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
//#include "BookManager.h"
//#include "Book.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void startServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void readyRead();
    void disconnected();
    void handleGetUsers(QTcpSocket* socket);



private:
    // --- Auth / User management ---
    void handleLogin(QTcpSocket* socket, const QJsonObject& data);
    void handleRegister(QTcpSocket* socket, const QJsonObject& data);
    QJsonArray loadUsers();
    void saveUsers(const QJsonArray& users);
    void handleBlockUser(QTcpSocket* socket, const QJsonObject& data);
    void handleDeleteUser(QTcpSocket* socket, const QJsonObject& data);
    void handleSaveGenres(QTcpSocket *socket, const QJsonObject &data);
    void handleGetSecurityQuestion(QTcpSocket* socket, const QJsonObject& data);
    void handleVerifySecurityAnswer(QTcpSocket* socket, const QJsonObject& data);
    void handleResetPassword(QTcpSocket* socket, const QJsonObject& data);
    void handleGetLibrary(QTcpSocket* socket, const QJsonObject& data);
    void handleGetPurchaseHistory(QTcpSocket* socket, const QJsonObject& data);
    void handleSearchBooks(QTcpSocket* socket, const QJsonObject &json);


    // --- Books ---
    QJsonArray loadBooks();
    void handleGetBooks(QTcpSocket* socket);
    void saveBooks(const QJsonArray &books);



    // --- Cart ---
    QJsonArray loadCarts();
    void saveCarts(const QJsonArray& carts);
    void handleAddToCart(QTcpSocket* socket, const QJsonObject& data);
    void handleGetCart(QTcpSocket* socket, const QJsonObject& data);
    void handleRemoveFromCart(QTcpSocket* socket, const QJsonObject& data);
    void handleCheckout(QTcpSocket* socket, const QJsonObject& data);

    // --- Purchase history & personal library ---
    QJsonArray loadPurchaseHistory();
    void savePurchaseHistory(const QJsonArray &history);
    void addBooksToLibrary(const QString &username, const QJsonArray &purchasedItems);
    double calculateDiscount(double total, int itemCount);

};

#endif // SERVER_H
