#include "server.h"
#include <QDebug>
#include <QFile>
#include <QDateTime>

// مسیر فایل‌های ذخیره‌سازی
const QString USERS_FILE = "users.json";
const QString BOOKS_FILE = "books.json";
const QString CARTS_FILE = "carts.json";

Server::Server(QObject *parent) : QTcpServer(parent) {}

// ================= Users =================

QJsonArray Server::loadUsers() {
    QFile file(USERS_FILE);
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonArray();
    }
    QByteArray data = file.readAll();
    file.close();
    return QJsonDocument::fromJson(data).array();
}

void Server::saveUsers(const QJsonArray& users) {
    QFile file(USERS_FILE);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(users).toJson());
        file.close();
    }
}

// ================= Books =================

QJsonArray Server::loadBooks() {
    QFile file(BOOKS_FILE);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "books.json not found or cannot be opened";
        return QJsonArray();
    }
    QByteArray data = file.readAll();
    file.close();
    return QJsonDocument::fromJson(data).array();
}

// ================= Carts =================
// هر آیتم کارت: { "username": "...", "book_id": "...", "quantity": N }

QJsonArray Server::loadCarts() {
    QFile file(CARTS_FILE);
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonArray();
    }
    QByteArray data = file.readAll();
    file.close();
    return QJsonDocument::fromJson(data).array();
}

void Server::saveCarts(const QJsonArray& carts) {
    QFile file(CARTS_FILE);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(carts).toJson());
        file.close();
    }
}

// ================= Server lifecycle =================

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

void Server::disconnected() {
    qDebug() << "Client disconnected";
}

// ================= Dispatch =================

void Server::readyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    qDebug() << "SERVER RECEIVED:";
    qDebug().noquote() << data;
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
    } else if (type == "save_genres") {
        handleSaveGenres(socket, json);
    } else if (type == "get_security_question") {
        handleGetSecurityQuestion(socket, json);
    } else if (type == "verify_security_answer") {
        handleVerifySecurityAnswer(socket, json);
    } else if (type == "reset_password") {
        handleResetPassword(socket, json);
    } else if (type == "get_books") {
        handleGetBooks(socket);
    } else if (type == "add_to_cart") {
        handleAddToCart(socket, json);
    } else if (type == "get_cart") {
        handleGetCart(socket, json);
    } else if (type == "remove_from_cart") {
        handleRemoveFromCart(socket, json);
    } else if (type == "checkout") {
        handleCheckout(socket, json);
    } else if (type == "get_library") {
        handleGetLibrary(socket, json);
    } else if (type == "get_purchase_history") {
        handleGetPurchaseHistory(socket, json);
    } else if (type == "search_books") {
        handleSearchBooks(socket, json);
    }



}

// ================= Auth handlers =================

void Server::handleLogin(QTcpSocket* socket, const QJsonObject& data) {
    QString username = data["username"].toString();
    QString passwordHash = data["password"].toString();
    QJsonArray users = loadUsers();

    QJsonObject response;
    response["type"] = "login_response";
    bool found = false;
    bool isBlocked = false;
    QString role;

    for (const QJsonValue& v : users) {
        QJsonObject userObj = v.toObject();
        if (userObj.value("username").toString() == username &&
            userObj.value("password").toString() == passwordHash) {

            found = true;
            QString status = userObj.value("status").toString("active");
            if (status == "blocked") {
                isBlocked = true;
            } else {
                role = userObj.value("role").toString();
            }

            bool firstLogin = userObj["firstLogin"].toBool(true);
            response["firstLogin"] = firstLogin;
            break;
        }
    }

    if (!found) {
        response["status"] = "error";
        response["message"] = "نام کاربری یا رمز عبور اشتباه است!";
    } else if (isBlocked) {
        response["status"] = "error";
        response["message"] = "حساب کاربری شما مسدود شده است و امکان ورود ندارید.";
    } else {
        response["status"] = "success";
        response["role"] = role;
        response["message"] = "ورود موفقیت‌آمیز";
    }

    socket->write(QJsonDocument(response).toJson());
}

void Server::handleRegister(QTcpSocket* socket, const QJsonObject& data) {
    QString username = data["username"].toString();
    QJsonArray users = loadUsers();

    QJsonObject response;
    response["type"] = "register_response";

    bool exists = false;
    for (const QJsonValue& v : users) {
        if (v.toObject().value("username").toString() == username) {
            exists = true;
            break;
        }
    }

    if (exists) {
        response["status"] = "error";
        response["message"] = "Username already exists!";
    } else {
        QJsonObject newUser = data;
        newUser["registration_date"] = QDateTime::currentDateTime().toString("yyyy/MM/dd");
        newUser["status"] = "active";
        newUser["firstLogin"] = true;
        newUser["favoriteGenres"] = QJsonArray();
        newUser["favoriteGenres"] = QJsonArray();
        newUser["personalLibrary"] = QJsonArray();


        users.append(newUser);
        saveUsers(users);

        response["status"] = "success";
        response["message"] = "Registration successful!";
    }

    socket->write(QJsonDocument(response).toJson());
}

void Server::handleGetUsers(QTcpSocket* socket) {
    QJsonArray users = loadUsers();
    QJsonArray usersList;

    for (const QJsonValue& v : users) {
        QJsonObject userObj = v.toObject();
        QJsonObject userSummary;

        userSummary["username"] = userObj.value("username").toString();
        userSummary["role"] = userObj.value("role").toString();
        userSummary["registration_date"] =
            userObj.value("registration_date").toString("نامشخص");
        userSummary["status"] =
            userObj.value("status").toString("active");

        QString role = userSummary["role"].toString().toLower();
        if (role == "customer") {
            userSummary["favoriteGenres"] =
                userObj.value("favoriteGenres").toArray();
        } else if (role == "publisher") {
            userSummary["publisher_name"] =
                userObj.value("publisher_name").toString("ثبت نشده");
        }

        usersList.append(userSummary);
    }

    QJsonObject response;
    response["type"] = "users_list";
    response["users"] = usersList;

    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

void Server::handleBlockUser(QTcpSocket* socket, const QJsonObject& data) {
    QString targetUser = data["username"].toString();
    QJsonArray users = loadUsers();
    bool success = false;

    for (int i = 0; i < users.size(); ++i) {
        QJsonObject userObj = users[i].toObject();
        if (userObj["username"].toString() == targetUser) {
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
    response["message"] = success ? "کاربر با موفقیت حذف شد" : "خطا در حذف کاربر";
    socket->write(QJsonDocument(response).toJson());
}

void Server::handleSaveGenres(QTcpSocket *socket, const QJsonObject &data)
{
    qDebug() << "handleSaveGenres entered";
    QString username = data["username"].toString();
    QJsonArray genres = data["genres"].toArray();

    QJsonArray users = loadUsers();
    bool success = false;

    for (int i = 0; i < users.size(); i++) {
        QJsonObject user = users[i].toObject();

        if (user["username"].toString() == username) {
            user["favoriteGenres"] = genres;
            user["firstLogin"] = false;

            users[i] = user;
            success = true;
            break;
        }
    }

    if (success) {
        qDebug() << "Saving genres for:" << username;
        qDebug() << genres;
        saveUsers(users);
        qDebug() << "Users saved";
        qDebug() << QJsonDocument(users).toJson();
    }

    QJsonObject response;
    response["type"] = "save_genres_response";
    response["success"] = success;

    qDebug() << "Sending save_genres_response";
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
    qDebug() << "save_genres_response sent";
}

void Server::handleGetSecurityQuestion(QTcpSocket* socket, const QJsonObject& data) {
    QString username = data["username"].toString();
    QJsonArray users = loadUsers();
    QJsonObject response;
    response["type"] = "get_security_question_response";

    for (const QJsonValue& v : users) {
        QJsonObject u = v.toObject();
        if (u["username"].toString() == username) {
            response["success"] = true;
            response["question"] = u["securityQuestion"].toString();
            socket->write(QJsonDocument(response).toJson());
            return;
        }
    }
    response["success"] = false;
    response["message"] = "کاربر یافت نشد";
    socket->write(QJsonDocument(response).toJson());
}

void Server::handleVerifySecurityAnswer(QTcpSocket* socket, const QJsonObject& data) {
    QString username = data["username"].toString();
    QString answer = data["answer"].toString();
    QJsonArray users = loadUsers();
    QJsonObject response;
    response["type"] = "verify_security_answer_response";

    for (const QJsonValue& v : users) {
        QJsonObject u = v.toObject();
        if (u["username"].toString() == username) {
            response["success"] = (u["securityAnswer"].toString() == answer);
            socket->write(QJsonDocument(response).toJson());
            return;
        }
    }
    response["success"] = false;
    socket->write(QJsonDocument(response).toJson());
}

void Server::handleResetPassword(QTcpSocket* socket, const QJsonObject& data) {
    QString username = data["username"].toString();
    QString newPasswordHash = data["password"].toString();
    QJsonArray users = loadUsers();
    QJsonObject response;
    response["type"] = "reset_password_response";
    bool success = false;

    for (int i = 0; i < users.size(); i++) {
        QJsonObject u = users[i].toObject();
        if (u["username"].toString() == username) {
            u["password"] = newPasswordHash;
            users[i] = u;
            success = true;
            break;
        }
    }
    if (success) saveUsers(users);
    response["success"] = success;
    socket->write(QJsonDocument(response).toJson());
}

// ================= Book handlers =================

void Server::handleGetBooks(QTcpSocket* socket) {
    QJsonArray books = loadBooks();

    QJsonObject response;
    response["type"] = "books_list";
    response["books"] = books;

    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

// ================= Cart handlers =================

void Server::handleAddToCart(QTcpSocket* socket, const QJsonObject& data) {
    QString username = data["username"].toString();
    QString bookId = data["book_id"].toString();
    int quantity = data.value("quantity").toInt(1);

    QJsonArray carts = loadCarts();
    bool found = false;

    for (int i = 0; i < carts.size(); ++i) {
        QJsonObject item = carts[i].toObject();
        if (item["username"].toString() == username &&
            item["book_id"].toString() == bookId) {
            item["quantity"] = item["quantity"].toInt(1) + quantity;
            carts[i] = item;
            found = true;
            break;
        }
    }

    if (!found) {
        QJsonObject newItem;
        newItem["username"] = username;
        newItem["book_id"] = bookId;
        newItem["quantity"] = quantity;
        carts.append(newItem);
    }

    saveCarts(carts);

    QJsonObject response;
    response["type"] = "add_to_cart_response";
    response["success"] = true;
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

void Server::handleGetCart(QTcpSocket* socket, const QJsonObject& data) {
    QString username = data["username"].toString();
    QJsonArray carts = loadCarts();
    QJsonArray books = loadBooks();

    QJsonArray cartItems;
    for (const QJsonValue& v : carts) {
        QJsonObject item = v.toObject();
        if (item["username"].toString() != username) continue;

        QString bookId = item["book_id"].toString();
        for (const QJsonValue& bv : books) {
            QJsonObject book = bv.toObject();
            if (book["id"].toString() == bookId) {
                QJsonObject cartEntry;
                cartEntry["book_id"] = bookId;
                cartEntry["title"] = book["title"].toString();
                cartEntry["price"] = book["price"].toDouble();
                cartEntry["quantity"] = item["quantity"].toInt(1);
                cartItems.append(cartEntry);
                break;
            }
        }
    }

    QJsonObject response;
    response["type"] = "cart_response";
    response["items"] = cartItems;
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

void Server::handleRemoveFromCart(QTcpSocket* socket, const QJsonObject& data) {
    QString username = data["username"].toString();
    QString bookId = data["book_id"].toString();

    QJsonArray carts = loadCarts();
    bool success = false;

    for (int i = 0; i < carts.size(); ++i) {
        QJsonObject item = carts[i].toObject();
        if (item["username"].toString() == username &&
            item["book_id"].toString() == bookId) {
            carts.removeAt(i);
            success = true;
            break;
        }
    }

    if (success) saveCarts(carts);

    QJsonObject response;
    response["type"] = "remove_from_cart_response";
    response["success"] = success;
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

double Server::calculateDiscount(double total, int itemCount)
{
    // نمونه سیاست تخفیف (قابل تنظیم طبق نیاز پروژه):
    // ۱۰٪ تخفیف برای خرید بالای ۳ آیتم یا مبلغ بالای ۵۰۰,۰۰۰ تومان
    if (itemCount >= 3 || total >= 500000)
        return total * 0.10;
    return 0.0;
}

QJsonArray Server::loadPurchaseHistory()
{
    QFile file("purchase_history.json");
    if (!file.open(QIODevice::ReadOnly))
        return QJsonArray();
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    return doc.array();
}

void Server::savePurchaseHistory(const QJsonArray &history)
{
    QFile file("purchase_history.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(history).toJson());
        file.close();
    }
}

void Server::addBooksToLibrary(const QString &username, const QJsonArray &purchasedItems)
{
    QJsonArray users = loadUsers();
    for (int i = 0; i < users.size(); ++i) {
        QJsonObject user = users[i].toObject();
        if (user["username"].toString() == username) {
            QJsonArray library = user.value("personalLibrary").toArray();
            for (const QJsonValue &v : purchasedItems) {
                QJsonObject item = v.toObject();
                library.append(item["book_id"].toString());
            }
            user["personalLibrary"] = library;
            users[i] = user;
            break;
        }
    }
    saveUsers(users);
}

void Server::handleCheckout(QTcpSocket* socket, const QJsonObject& data)
{
    QString username = data["username"].toString();
    QJsonArray carts = loadCarts();
    QJsonArray books = loadBooks();

    QJsonArray userItems;
    QJsonArray remainingCarts;
    double total = 0;
    bool stockError = false;
    QString stockErrorBook;

    // جدا کردن آیتم‌های این کاربر
    for (const QJsonValue &v : carts) {
        QJsonObject item = v.toObject();
        if (item["username"].toString() == username) {
            QString bookId = item["book_id"].toString();
            int qty = item["quantity"].toInt(1);

            // بررسی موجودی کتاب
            for (const QJsonValue &bv : books) {
                QJsonObject book = bv.toObject();
                if (book["id"].toString() == bookId) {
                    int stock = book["stock"].toInt();
                    if (stock < qty) {
                        stockError = true;
                        stockErrorBook = book["title"].toString();
                    }
                    break;
                }
            }
            userItems.append(item);
        } else {
            remainingCarts.append(item);
        }
    }

    // ۱) بررسی سبد خالی
    if (userItems.isEmpty()) {
        QJsonObject response;
        response["type"] = "checkout_response";
        response["success"] = false;
        response["message"] = "سبد خرید خالی است";
        response["total"] = 0;
        response["discountAmount"] = 0;
        response["finalAmount"] = 0;
        socket->write(QJsonDocument(response).toJson(QJsonDocument::Compact) + "\n");
        return;
    }

    // ۲) بررسی موجودی
    if (stockError) {
        QJsonObject response;
        response["type"] = "checkout_response";
        response["success"] = false;
        response["message"] = "موجودی کافی نیست: " + stockErrorBook;
        socket->write(QJsonDocument(response).toJson(QJsonDocument::Compact) + "\n");
        return;
    }

    // ۳) محاسبه مبلغ کل و کاهش موجودی
    int itemCount = 0;
    for (const QJsonValue &v : userItems) {
        QJsonObject item = v.toObject();
        QString bookId = item["book_id"].toString();
        int qty = item["quantity"].toInt(1);
        itemCount += qty;

        for (int i = 0; i < books.size(); ++i) {
            QJsonObject book = books[i].toObject();
            if (book["id"].toString() == bookId) {
                total += book["price"].toDouble() * qty;
                book["stock"] = book["stock"].toInt() - qty;
                books[i] = book;
                break;
            }
        }
    }

    saveBooks(books); // در صورت نبود، این متد را طبق الگوی saveUsers اضافه کنید

    // ۴) اعمال تخفیف
    double discount = calculateDiscount(total, itemCount);
    double finalAmount = total - discount;

    // ۵) ذخیره سبد باقی‌مانده
    saveCarts(remainingCarts);

    // ۶) ثبت تاریخچه خرید
    QJsonArray history = loadPurchaseHistory();
    QJsonObject record;
    record["username"] = username;
    record["items"] = userItems;
    record["total"] = total;
    record["discountAmount"] = discount;
    record["finalAmount"] = finalAmount;
    record["date"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    history.append(record);
    savePurchaseHistory(history);

    // ۷) انتقال به کتابخانه شخصی
    addBooksToLibrary(username, userItems);

    // ۸) پاسخ نهایی
    QJsonObject response;
    response["type"] = "checkout_response";
    response["success"] = true;
    response["total"] = total;
    response["discountAmount"] = discount;
    response["finalAmount"] = finalAmount;
    response["message"] = "خرید با موفقیت انجام شد";
    socket->write(QJsonDocument(response).toJson(QJsonDocument::Compact) + "\n");
}
void Server::saveBooks(const QJsonArray &books) {
    QFile file(BOOKS_FILE);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(books).toJson());
        file.close();
    }
}
//دریافت کتابخانه شخصی کاربر
void Server::handleGetLibrary(QTcpSocket* socket, const QJsonObject& data)
{
    QString username = data["username"].toString();
    QJsonArray users = loadUsers();
    QJsonArray library;

    for (const QJsonValue &v : users) {
        QJsonObject user = v.toObject();
        if (user["username"].toString() == username) {
            QJsonArray bookIds = user.value("personalLibrary").toArray();
            QJsonArray allBooks = loadBooks();

            // تبدیل شناسه‌ها به اطلاعات کامل کتاب
            for (const QJsonValue &idVal : bookIds) {
                QString bookId = idVal.toString();
                for (const QJsonValue &bv : allBooks) {
                    QJsonObject book = bv.toObject();
                    if (book["id"].toString() == bookId) {
                        library.append(book);
                        break;
                    }
                }
            }
            break;
        }
    }
    //دریافت  تاریخچه خرید کاربر
    QJsonObject response;
    response["type"] = "library_response";
    response["items"] = library;
    socket->write(QJsonDocument(response).toJson(QJsonDocument::Compact) + "\n");
}
void Server::handleGetPurchaseHistory(QTcpSocket* socket, const QJsonObject& data)
{
    QString username = data["username"].toString();
    QJsonArray allHistory = loadPurchaseHistory();
    QJsonArray userHistory;

    for (const QJsonValue &v : allHistory) {
        QJsonObject record = v.toObject();
        if (record["username"].toString() == username) {
            userHistory.append(record);
        }
    }

    QJsonObject response;
    response["type"] = "purchase_history_response";
    response["items"] = userHistory;
    socket->write(QJsonDocument(response).toJson(QJsonDocument::Compact) + "\n");
}

void Server::handleSearchBooks(QTcpSocket* socket, const QJsonObject &json)
{
    QString query = json["query"].toString().toLower().trimmed();
    QString field = json["field"].toString();

    QJsonArray allBooks = loadBooks();
    QJsonArray result;
    for (const QJsonValue &val : allBooks) {
        QJsonObject book = val.toObject();
        if (book[field].toString().toLower().contains(query))
            result.append(book);
    }

    QJsonObject response;
    response["type"] = "search_result";
    response["books"] = result;
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}

