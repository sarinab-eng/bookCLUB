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
    // پیام‌ها با '\n' جدا می‌شوند چون چند پیام JSON پشت‌سرهم می‌توانند در یک
    // بسته‌ی TCP به هم بچسبند؛ بدون این جداکننده، سمت کلاینت نمی‌تواند
    // مرز بین پیام‌ها را تشخیص بدهد و parse همه‌شان با هم شکست می‌خورد.
    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n";
    m_socket->write(data);
    m_socket->flush();
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
    m_buffer.append(m_socket->readAll());

    int newlineIndex;
    while ((newlineIndex = m_buffer.indexOf('\n')) != -1) {
        QByteArray line = m_buffer.left(newlineIndex);
        m_buffer.remove(0, newlineIndex + 1);
        if (line.trimmed().isEmpty()) continue;

        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (doc.isNull()) continue;

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
    else if (type == "search_result") {
        emit searchResultReceived(response["books"].toArray());
    }

    else if (type == "add_to_cart_response") {
        emit itemAddedToCart(response["success"].toBool(), response["message"].toString());
    }
    else if (type == "cart_list" || type == "cart_response") {   // ⬅️ پشتیبانی از هر دو
        emit cartReceived(response["items"].toArray());
        emit cartSummaryReceived(response["itemCount"].toInt(),
                                  response["total"].toDouble(),
                                  response["discountAmount"].toDouble(),
                                  response["finalAmount"].toDouble());
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
      else if (type == "review_posted")
        emit reviewPosted(response["success"].toBool(), response["message"].toString());
    else if (type == "reviews_list")
        emit reviewsReceived(response["book_id"].toString(), response["reviews"].toArray());
    else if (type == "review_edited")
        emit reviewEdited(response["success"].toBool(), response["message"].toString());
    else if (type == "review_deleted")
        emit reviewDeleted(response["success"].toBool(), response["message"].toString());
    else if (type == "profile_response")
        emit profileReceived(response);
    else if (type == "change_password_response")
        emit passwordChanged(response["success"].toBool(), response["message"].toString());
    }
}

// ---------------- Admin / Users ----------------

void AuthManager::requestUsersList() {
    QJsonObject request;
    request["type"] = "get_users";
    sendJson(request);
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

// ---------------- Profile ----------------

void AuthManager::requestProfile(const QString &username) {
    QJsonObject request;
    request["type"] = "get_profile";
    request["username"] = username;
    sendJson(request);
}

void AuthManager::changePassword(const QString &username, const QString &oldPassword, const QString &newPassword) {
    QJsonObject request;
    request["type"] = "change_password";
    request["username"] = username;
    request["oldPassword"] = User::hashPassword(oldPassword);
    request["newPassword"] = User::hashPassword(newPassword);
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
void AuthManager::searchBooks(const QString &query, const QString &field) {
    QJsonObject request;
    request["type"] = "search_books";
    request["query"] = query;
    request["field"] = field;
    sendJson(request);
}
void AuthManager::postReview(const QString &username, const QString &bookId, int rating, const QString &comment) {
    QJsonObject req;
    req["type"] = "post_review";
    req["username"] = username;
    req["book_id"] = bookId;
    req["rating"] = rating;
    req["comment"] = comment;
    sendJson(req);
}

void AuthManager::getReviews(const QString &bookId) {
    QJsonObject req;
    req["type"] = "get_reviews";
    req["book_id"] = bookId;
    sendJson(req);
}

void AuthManager::editReview(const QString &username, const QString &reviewId, int rating, const QString &comment) {
    QJsonObject req;
    req["type"] = "edit_review";
    req["username"] = username;
    req["review_id"] = reviewId;
    req["rating"] = rating;
    req["comment"] = comment;
    sendJson(req);
}

void AuthManager::deleteReview(const QString &username, const QString &reviewId) {
    QJsonObject req;
    req["type"] = "delete_review";
    req["username"] = username;
    req["review_id"] = reviewId;
    sendJson(req);
}
