#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include "User.h"

class AuthManager : public QObject {
    Q_OBJECT
public:
    explicit AuthManager(QTcpSocket *socket, QObject *parent = nullptr);

    // متدهای اصلی درخواست کلاینت (ارسال به سرور)
    void sendLoginRequest(const QString &username, const QString &password);
    void sendRegisterRequest(const QString &username, const QString &password,
                             const QString &secQuestion, const QString &secAnswer,
                             const QString &role, const QVector<int> &favGenres = {});
    void sendResetPasswordRequest(const QString &username, const QString &secAnswer, const QString &newPassword);

    // پردازش پاسخ‌های دریافتی از سرور
    void handleServerResponse(const QJsonObject &responseObj);

signals:
    // سیگنال‌هایی که به UI متصل می‌شوند تا نتیجه را نمایش دهند
    void loginSuccess(const QString &role, const QString &username);
    void loginFailed(const QString &errorMsg);
    void registrationSuccess();
    void registrationFailed(const QString &errorMsg);
    void passwordResetSuccess();
    void passwordResetFailed(const QString &errorMsg);

private:
    QTcpSocket *m_socket; // سوکت شبکه مشترک با کلاینت اصلی
    void sendJson(const QJsonObject &json);
};

#endif // AUTHMANAGER_H