#include "Authmanager.h"
#include <QJsonArray>

AuthManager::AuthManager(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket) {
}

// ارسال داده کمکی به سوکت
void AuthManager::sendJson(const QJsonObject &json) {
    if (m_socket && m_socket->isOpen()) {
        QJsonDocument doc(json);
        m_socket->write(doc.toJson(QJsonDocument::Compact) + "\n"); // افزودن کاراکتر خط جدید برای تفکیک پیام‌ها
    }
}

// ۱. ارسال درخواست ورود
void AuthManager::sendLoginRequest(const QString &username, const QString &password) {
    QJsonObject request;
    request["action"] = "login";
    // رمزنگاری دوطرفه نام کاربری و هش یک‌طرفه رمز عبور
    request["username"] = User::encrypt(username);
    request["passwordHash"] = User::hashPassword(password);

    sendJson(request);
}

// ۲. ارسال درخواست ثبت‌نام
void AuthManager::sendRegisterRequest(const QString &username, const QString &password,
                                      const QString &secQuestion, const QString &secAnswer,
                                      const QString &role, const QVector<int> &favGenres) {
    QJsonObject request;
    request["action"] = "register";
    request["username"] = User::encrypt(username);
    request["passwordHash"] = User::hashPassword(password);
    request["securityQuestion"] = User::encrypt(secQuestion);
    request["securityAnswer"] = User::encrypt(secAnswer);
    request["role"] = role;

    // تبدیل ژانرهای مورد علاقه به آرایه JSON (برای نقش Customer در اولین ورود)
    if (role == "Customer") {
        QJsonArray genresArray;
        for (int genreId : favGenres) {
            genresArray.append(genreId);
        }
        request["favoriteGenres"] = genresArray;
    }

    sendJson(request);
}

// ۳. ارسال درخواست فراموشی و بازیابی رمز عبور
void AuthManager::sendResetPasswordRequest(const QString &username, const QString &secAnswer, const QString &newPassword) {
    QJsonObject request;
    request["action"] = "reset_password";
    request["username"] = User::encrypt(username);
    request["securityAnswer"] = User::encrypt(secAnswer);
    request["newPasswordHash"] = User::hashPassword(newPassword);

    sendJson(request);
}

// ۴. پردازش پاسخ‌های دریافتی از سرور
void AuthManager::handleServerResponse(const QJsonObject &responseObj) {
    QString action = responseObj["action"].toString();
    bool success = responseObj["success"].toBool();
    QString message = responseObj["message"].toString();

    if (action == "login") {
        if (success) {
            QString role = responseObj["role"].toString();
            QString decryptedUsername = User::decrypt(responseObj["username"].toString());
            emit loginSuccess(role, decryptedUsername);
        } else {
            emit loginFailed(message);
        }
    }
    else if (action == "register") {
        if (success) {
            emit registrationSuccess();
        } else {
            emit registrationFailed(message);
        }
    }
    else if (action == "reset_password") {
        if (success) {
            emit passwordResetSuccess();
        } else {
            emit passwordResetFailed(message);
        }
    }
}