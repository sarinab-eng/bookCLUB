#include "ClientNetworkManager.h"
#include <QHostAddress>

ClientNetworkManager* ClientNetworkManager::instance = nullptr;

ClientNetworkManager* ClientNetworkManager::getInstance() {
    if (!instance) {
        instance = new ClientNetworkManager();
    }
    return instance;
}

ClientNetworkManager::ClientNetworkManager(QObject *parent)
    : QObject(parent), currentUserId(-1), currentUsername("") {
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &ClientNetworkManager::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &ClientNetworkManager::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &ClientNetworkManager::onDisconnected);
}

void ClientNetworkManager::connectToServer(const QString &host, quint16 port) {
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        socket->connectToHost(host, port);
    }
}

void ClientNetworkManager::disconnectFromServer() {
    socket->disconnectFromHost();
}

bool ClientNetworkManager::isConnected() const {
    return socket->state() == QAbstractSocket::ConnectedState;
}

void ClientNetworkManager::setCurrentUser(int userId, const QString &username) {
    currentUserId = userId;
    currentUsername = username;
}

// ارسال درخواست خرید سبد به سرور
void ClientNetworkManager::sendPurchaseRequest(const ShoppingCart &cart) {
    if (!isConnected()) return;

    QJsonObject request;
    request["action"] = "purchase";
    request["userId"] = currentUserId;
    request["cart"] = cart.toJson(); // تبدیل کل سبد به آرایه JSON در قالب شیء

    QJsonDocument doc(request);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n"); // تفکیک پیام‌ها با خط جدید
}

// ارسال ثبت/ویرایش نظر به سرور
void ClientNetworkManager::sendReview(const Review &review) {
    if (!isConnected()) return;

    QJsonObject request;
    request["action"] = "submit_review";
    request["review"] = review.toJson();

    QJsonDocument doc(request);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

// درخواست کتاب‌های خریده شده کاربر (کتابخانه شخصی)
void ClientNetworkManager::requestPersonalLibrary() {
    if (!isConnected()) return;

    QJsonObject request;
    request["action"] = "get_personal_library";
    request["userId"] = currentUserId;

    QJsonDocument doc(request);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

// درخواست تمام کتاب‌های فروشگاه
void ClientNetworkManager::requestAllBooks() {
    if (!isConnected()) return;

    QJsonObject request;
    request["action"] = "get_all_books";

    QJsonDocument doc(request);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

// دریافت و پردازش پاسخ‌های سرور
void ClientNetworkManager::onReadyRead() {
    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (doc.isNull()) continue;

        QJsonObject response = doc.object();
        QString action = response["action"].toString();

        if (action == "get_all_books_response") {
            QVector<Book> books;
            QJsonArray arr = response["books"].toArray();
            for (auto ref : arr) {
                books.append(Book::fromJson(ref.toObject()));
            }
            emit allBooksReceived(books);
        }
        else if (action == "get_personal_library_response") {
            QVector<Book> books;
            QJsonArray arr = response["books"].toArray();
            for (auto ref : arr) {
                books.append(Book::fromJson(ref.toObject()));
            }
            emit personalLibraryReceived(books);
        }
        else if (action == "purchase_response") {
            bool success = response["success"].toBool();
            QString message = response["message"].toString();
            emit purchaseStatusReceived(success, message);
        }
        else if (action == "submit_review_response") {
            bool success = response["success"].toBool();
            QString message = response["message"].toString();
            emit reviewStatusReceived(success, message);
        }
        else if (action == "broadcast_new_review") {
            // دریافت نظر جدیدی که کاربر دیگری ثبت کرده (به‌روزرسانی آنی طبق داکیومنت)
            Review review = Review::fromJson(response["review"].toObject());
            emit newReviewBroadcasted(review);
        }
    }
}

void ClientNetworkManager::onConnected() {
    emit connectionStatusChanged(true);
}

void ClientNetworkManager::onDisconnected() {
    emit connectionStatusChanged(false);
}
