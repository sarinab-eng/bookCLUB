#include "Authmanager.h"

AuthManager::AuthManager(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket) {
    if (m_socket) connect(m_socket, &QTcpSocket::readyRead, this, &AuthManager::onReadyRead);
}

void AuthManager::registerUser(const QString &username, const QString &password,
                               const QString &question, const QString &answer,
                               const QString &role, const QVector<int> &genres) {
    QJsonObject request;
    request["action"] = "register";
    request["username"] = username;
    request["password"] = User::hashPassword(password);
    request["securityQuestion"] = User::encrypt(question);
    request["securityAnswer"] = User::encrypt(answer);
    request["role"] = role;

    QJsonArray genresArray;
    for (int id : genres) genresArray.append(id);
    request["favoriteGenres"] = genresArray;

    sendJson(request);
}

void AuthManager::loginUser(const QString &username, const QString &password) {
    QJsonObject request;
    request["action"] = "login";
    request["username"] = username;
    request["password"] = User::hashPassword(password);
    sendJson(request);
}

void AuthManager::sendJson(const QJsonObject &json) {
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n");
        m_socket->flush();
    } else {
        emit registerFinished(false, "Network disconnected");
    }
}

void AuthManager::onReadyRead() {
    while (m_socket && m_socket->canReadLine()) {
        QByteArray line = m_socket->readLine().trimmed();
        handleServerResponse(QJsonDocument::fromJson(line).object());
    }
}

void AuthManager::handleServerResponse(const QJsonObject &responseObj) {
    QString action = responseObj["action"].toString();
    if (action == "login") emit loginFinished(responseObj["success"].toBool(), responseObj["message"].toString());
    else if (action == "register") emit registerFinished(responseObj["success"].toBool(), responseObj["message"].toString());
}