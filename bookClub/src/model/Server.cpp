#include "server.h"
#include <QDebug>
#include <QFile>
#include <QDateTime>

// مسیر فایل ذخیره‌سازی کاربران
const QString USERS_FILE = "users.json";

Server::Server(QObject *parent) : QTcpServer(parent) {}

// پیاده‌سازی لود کاربران
QJsonArray Server::loadUsers() {
    QFile file(USERS_FILE);
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonArray();
    }
    QByteArray data = file.readAll();
    file.close();
    return QJsonDocument::fromJson(data).array();
}

// پیاده‌سازی ذخیره کاربران
void Server::saveUsers(const QJsonArray& users) {
    QFile file(USERS_FILE);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(users).toJson());
        file.close();
    }
}

void Server::startServer() {
    if (!this->listen(QHostAddress::Any, 1234)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on port 1234...";
    }
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::readyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::disconnected);
    qDebug() << "New client connected:" << socketDescriptor;
}

void Server::handleBlockUser(QTcpSocket* socket, const QJsonObject& data) {
    QString targetUser = data["username"].toString();
    QJsonArray users = loadUsers();
    bool success = false;

    for (int i = 0; i < users.size(); ++i) {
        QJsonObject userObj = users[i].toObject();
        if (userObj["username"].toString() == targetUser) {
            // تغییر وضعیت: اگر فعال بود مسدود شود و برعکس (Toggle)
            QString currentStatus = userObj.value("status").toString("active");
            userObj["status"] = (currentStatus == "active") ? "blocked" : "active";
            users[i] = userObj;
            success = true;
            break;
        }
    }

    if (success) saveUsers(users);

    QJsonObject response;
    response["type"] = "admin_action_response";
    response["success"] = success;
    response["message"] = success ? "وضعیت کاربر تغییر کرد" : "کاربر یافت نشد";
    socket->write(QJsonDocument(response).toJson());
}

void Server::handleDeleteUser(QTcpSocket* socket, const QJsonObject& data) {
    QString targetUser = data["username"].toString();
    QJsonArray users = loadUsers();
    bool success = false;

    for (int i = 0; i < users.size(); ++i) {
        if (users[i].toObject()["username"].toString() == targetUser) {
            users.removeAt(i);
            success = true;
            break;
        }
    }

    if (success) saveUsers(users);

    QJsonObject response;
    response["type"] = "admin_action_response";
    response["success"] = success;
    response["message"] = success ? "کاربر با موفقیت حذف شد" : "خطا در حذف کاربر" ;
    socket->write(QJsonDocument(response).toJson());
}

void Server::readyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) return;

    QJsonObject json = doc.object();
    QString type = json.contains("type") ? json["type"].toString() : json["action"].toString();

    if (type == "login") {
        handleLogin(socket, json);
    } else if (type == "register") {
        handleRegister(socket, json);
    } else if (type == "get_users") {
        handleGetUsers(socket);
    } else if (type == "block_user") {
        handleBlockUser(socket, json);
    } else if (type == "delete_user") {
        handleDeleteUser(socket, json);
    }
}

void Server::handleLogin(QTcpSocket* socket, const QJsonObject& data) {
    QString username = data["username"].toString();
    QString passwordHash = data["password"].toString();
    // این متد ادامه دارد...