#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
    void handleLogin(QTcpSocket* socket, const QJsonObject& data);
    void handleRegister(QTcpSocket* socket, const QJsonObject& data);
    QJsonArray loadUsers();
    void saveUsers(const QJsonArray& users);
    void handleBlockUser(QTcpSocket* socket, const QJsonObject& data);
    void handleDeleteUser(QTcpSocket* socket, const QJsonObject& data);
};

#endif // SERVER_H
