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
    qDebug() << "onReadyRead called";
    QByteArray data = m_socket->readAll();
    qDebug() << "Raw data from server:" << data;
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) return;

    QJsonObject response = doc.object();
    QString type = response["type"].toString();
    qDebug() << "TYPE =" << type;

    if (type == "login_response") {
        QString status = response["status"].toString();
        QString message = response["message"].toString();
        QString role = response["role"].toString();
        bool firstLogin = response["firstLogin"].toBool();
        emit loginFinished(status == "success", message, role, firstLogin);
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
    else if(type == "save_genres_response")
    {
        emit genresSaved(response["success"].toBool());
    }
    else if (type == "get_security_question_response") {
        emit securityQuestionReceived(response["success"].toBool(), response["question"].toString());
    }
    else if (type == "verify_security_answer_response") {
        emit securityAnswerVerified(response["success"].toBool(), response["message"].toString() );
    }
    else if (type == "reset_password_response") {
        emit passwordResetFinished(response["success"].toBool(), response["message"].toString());
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
    request["type"] = type;
    request["username"] = username;
    sendJson(request);
}
void AuthManager::saveGenres(const QString &username, const QStringList &genres)
{
    qDebug() << "saveGenres() entered";
    QJsonObject request;
    request["type"] = "save_genres";
    request["username"] = username;

    QJsonArray arr;
    for(const QString &g : genres)
        arr.append(g);

    request["genres"] = arr;

    sendJson(request);
}

void AuthManager::getSecurityQuestion(const QString &username) {
    QJsonObject req;
    req["type"] = "get_security_question";
    req["username"] = username;
    sendJson(req);
}

void AuthManager::verifySecurityAnswer(const QString &username, const QString &answer) {
    QJsonObject req;
    req["type"] = "verify_security_answer";
    req["username"] = username;
    req["answer"] = User::encrypt(answer);
    sendJson(req);
}

void AuthManager::resetPassword(const QString &username, const QString &newPassword) {
    QJsonObject req;
    req["type"] = "reset_password";
    req["username"] = username;
    req["password"] = User::hashPassword(newPassword);
    sendJson(req);
}