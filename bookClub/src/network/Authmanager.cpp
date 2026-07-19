#include "Authmanager.h"
#include <QTimer>
#include <QPointer>

AuthManager::AuthManager(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket) {
    if (m_socket) connect(m_socket, &QTcpSocket::readyRead, this, &AuthManager::onReadyRead);
}

// ---------------- Auth ----------------

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
        if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
            emit loginFinished(false, "عدم اتصال به سرور", {}, false);
            return;
        }
    QJsonObject request;
    request["type"] = "login";
    request["username"] = username;
    request["password"] = User::hashPassword(password);
    sendJson(request);
}

void AuthManager::sendJson(const QJsonObject &json)
{
    if (!m_socket) {
        qDebug() << "Socket is NULL";
        return;
    }
    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);
    qDebug() << "[CLIENT] sending:" << json.value("type").toString()
             << "| socket state:" << m_socket->state()
             << "| bytes:" << data.size();
    qint64 written = m_socket->write(data);
    bool flushed = m_socket->flush();
    qDebug() << "[CLIENT] write() returned:" << written << "| flush() returned:" << flushed;
}

void AuthManager::getSecurityQuestion(const QString &username) {
    QJsonObject request;
    request["type"] = "get_security_question";
    request["username"] = username;
    sendJson(request);
}

void AuthManager::verifySecurityAnswer(const QString &username, const QString &answer) {
    QJsonObject request;
    request["type"] = "verify_security_answer";
    request["username"] = username;
    request["answer"] = User::encrypt(answer);
    sendJson(request);
}

void AuthManager::resetPassword(const QString &username, const QString &newPassword) {
    QJsonObject request;
    request["type"] = "reset_password";
    request["username"] = username;
    request["newPassword"] = User::hashPassword(newPassword);
    sendJson(request);
}

void AuthManager::onReadyRead() {
    QByteArray data = m_socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) return;

    QJsonObject response = doc.object();
    QString type = response["type"].toString();

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
        emit usersListReceived(users);/*response["users"].toArray()*/
    }
    else if (type == "admin_action_response") {
        bool success = response["success"].toBool();
        QString message = response["message"].toString();
        emit actionFinished(success, message);//(response["success"].toBool(), response["message"].toString());
    }
    else if (type == "save_genres_response") {
        emit genresSaved(response["success"].toBool());
    }
    else if (type == "books_list") {
        emit booksReceived(response["books"].toArray());
    }
    else if (type == "add_to_cart_response") {
        emit itemAddedToCart(response["success"].toBool(), response["message"].toString());
    }
    else if (type == "cart_list" || type == "cart_response") {   // ⬅️ پشتیبانی از هر دو
        emit cartReceived(response["items"].toArray());
    }

    else if (type == "remove_from_cart_response") {
        emit itemRemovedFromCart(response["success"].toBool(), response["message"].toString());
    }
    /*else if (type == "checkout_response") {
        emit checkoutFinished(response["success"].toBool(), response["message"].toString());
    }*/
    else if (type == "get_security_question_response") {
        emit securityQuestionReceived(response["success"].toBool(), response["question"].toString());
    }
    else if (type == "verify_security_answer_response") {
        emit securityAnswerVerified(response["success"].toBool(), response["message"].toString() );
    }
    else if (type == "reset_password_response") {
        emit passwordResetFinished(response["success"].toBool(), response["message"].toString());
    }
    else if (type == "checkout_response") {
        emit checkoutFinished(response["success"].toBool(), response["message"].toString());
        bool success = response["success"].toBool();
        QString message = response["message"].toString();
        double total = response["total"].toDouble();
        double discount = response["discountAmount"].toDouble();
        double finalAmount = response["finalAmount"].toDouble();
        emit checkoutFinished(success, message, total, discount, finalAmount);
    }
    else if (type == "library_response") {
        emit libraryReceived(response["items"].toArray());
    }
    else if (type == "purchase_history_response") {
        emit purchaseHistoryReceived(response["items"].toArray());
    }
}

// ---------------- Admin / Users ----------------

void AuthManager::requestUsersList() {
    QJsonObject request;
    request["type"] = "get_users";
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        QJsonDocument doc(request);
        m_socket->write(doc.toJson());
    }
    //sendJson(request);
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
    QJsonObject request;
    request["type"] = "save_genres";
    request["username"] = username;
    QJsonArray arr;
    for (const QString &g : genres) arr.append(g);
    request["genres"] = arr;
    sendJson(request);
}

// ---------------- Books ----------------

void AuthManager::requestBooks() {
    QJsonObject request;
    request["type"] = "get_books";
    sendJson(request);
}

// ---------------- Cart ----------------

void AuthManager::addToCart(const QString &username, const QString &bookId, int quantity) {
    QJsonObject request;
    request["type"] = "add_to_cart";
    request["username"] = username;
    request["book_id"] = bookId;   // ⬅️ تغییر از "bookId" به "book_id"
    request["quantity"] = quantity;
    sendJson(request);
}

void AuthManager::requestCart(const QString &username) {
    QJsonObject request;
    request["type"] = "get_cart";
    request["username"] = username;
    sendJson(request);
}

void AuthManager::removeFromCart(const QString &username, const QString &bookId) {
    QJsonObject request;
    request["type"] = "remove_from_cart";
    request["username"] = username;
    request["book_id"] = bookId;   // ⬅️ تغییر از "bookId" به "book_id"
    sendJson(request);
}


void AuthManager::checkout(const QString &username) {
    QJsonObject request;
    request["type"] = "checkout";
    request["username"] = username;
    sendJson(request);
}
void AuthManager::requestLibrary(const QString &username)
{
    QJsonObject request;
    request["type"] = "get_library";
    request["username"] = username;
    sendJson(request);
}

void AuthManager::requestPurchaseHistory(const QString &username)
{
    QJsonObject request;
    request["type"] = "get_purchase_history";
    request["username"] = username;
    sendJson(request);
}
