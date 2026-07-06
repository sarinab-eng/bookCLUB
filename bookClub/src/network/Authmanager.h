#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVector>
#include "User.h"

class AuthManager : public QObject {
    Q_OBJECT
public:
    explicit AuthManager(QTcpSocket *socket, QObject *parent = nullptr);

    void loginUser(const QString &username, const QString &password);

    // هماهنگ شده با ۶ پارامتر ارسالی از UI
    void registerUser(const QString &username, const QString &password,
                      const QString &question, const QString &answer,
                      const QString &role, const QVector<int> &genres);

private slots:
    void onReadyRead();

signals:
    void loginFinished(bool success, const QString &message, const QString &role);
    void registerFinished(bool success, const QString &message);

private:
    QTcpSocket *m_socket;
    void sendJson(const QJsonObject &json);
    void handleServerResponse(const QJsonObject &responseObj);
};
#endif