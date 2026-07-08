#include "Authmanager.h"

AuthManager::AuthManager(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket) {
    if (m_socket) connect(m_socket, &QTcpSocket::readyRead, this, &AuthManager::onReadyRead);
}

void AuthManager::registerUser(const QString &username, const QString &password,
                               const QString &question, const QString &answer,
                               const QString &role, const QVector<int> &genres) {
    QJsonObject request;
    request["type"] = "register";
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
    if (!m_socket) return;
    QJsonObject request;
    request["type"] = "login";
    request["username"] = username;
    request["password"] = User::hashPassword(password);
    sendJson(request);

    qDebug() << "Data sent to server!";
}

void AuthManager::sendJson(const QJsonObject &json) {
    if (m_socket && m_socket->isOpen()) {
        m_socket->write(QJsonDocument(json).toJson());
        m_socket->flush();
    }
}

void AuthManager::onReadyRead() {
    QByteArray data = m_socket->readAll();
    qDebug() << "Raw data from server:" << data;
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) return;

    QJsonObject response = doc.object();
    QString type = response["type"].toString();

    if (type == "login_response") {
        QString status = response["status"].toString();
        QString message = response["message"].toString();
        QString role = response["role"].toString(); // استخراج نقش از جیسون سرور
        emit loginFinished(status == "success", message, role);
    }
    else if (type == "register_response") {
        QString status = response["status"].toString();
        QString message = response["message"].toString();
        emit registerFinished(status == "success", message);
    }
    else if (type == "users_list") {
        QJsonArray users = response["users"].toArray();
        emit usersListReceived(users);
    }
    else if (type == "admin_action_response") {
        bool success = response["success"].toBool();
        QString message = response["message"].toString();
        emit actionFinished(success, message);
    }
}

/*void AuthManager::handleServerResponse(const QJsonObject &responseObj) {
    QString action = responseObj["action"].toString();
    if (action == "login") {
        emit loginFinished(responseObj["success"].toBool(), responseObj["message"].toString(), responseObj["role"].toString());
    }
    else if (action == "register") {
        emit registerFinished(responseObj["success"].toBool(), responseObj["message"].toString());
    }
}*/
void AuthManager::requestUsersList() {
    QJsonObject request;
    request["type"] = "get_users";

    // تبدیل به بایت و ارسال از طریق سوکت
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
    QJsonDocument doc(request);
    m_socket->write(doc.toJson());
    }
}
void AuthManager::adminAction(const QString &type, const QString &username) {
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        emit actionFinished(false, "عدم اتصال به سرور");
        return;
    }
    QJsonObject request;
    request["type"] = type; // "block_user" یا "delete_user"
    request["username"] = username;
    sendJson(request);
}