#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVector>
#include <QStringList>
#include "User.h"

class AuthManager : public QObject {
    Q_OBJECT
public:
    explicit AuthManager(QTcpSocket *socket, QObject *parent = nullptr);

    void loginUser(const QString &username, const QString &password);
    void registerUser(const QString &username, const QString &password,
                      const QString &question, const QString &answer,
                      const QString &role, const QVector<int> &genres);

    void requestUsersList();
    void adminAction(const QString &type, const QString &username);
    void saveGenres(const QString &username, const QStringList &genres);
    void getSecurityQuestion(const QString &username);
    void requestPasswordReset(const QString &username);
    void verifySecurityAnswer(const QString &username, const QString &answer);
    void resetPassword(const QString &username, const QString &newPassword);

private slots:
    void onReadyRead();

signals:
    void loginFinished(bool success, const QString &message, const QString &role, bool firstLogin);
    void registerFinished(bool success, const QString &message);
    void usersListReceived(const QJsonArray &users);
    void actionFinished(bool success, const QString &message);
    void genresSaved(bool success);
    void securityQuestionReceived(bool success, const QString &question);
    void securityAnswerVerified(bool correct, const QString &message);
    void passwordResetFinished(bool success, const QString &message);

private:
    QTcpSocket *m_socket;
    QByteArray m_buffer;
    void sendJson(const QJsonObject &json);
    //void handleServerResponse(const QJsonObject &responseObj);
};
#endif