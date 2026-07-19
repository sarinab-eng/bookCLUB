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

    void requestLibrary(const QString &username);
    void requestPurchaseHistory(const QString &username);

    // ---- Auth ----
    void loginUser(const QString &username, const QString &password);
    void registerUser(const QString &username, const QString &password,
                      const QString &question, const QString &answer,
                      const QString &role, const QVector<int> &genres);
    // ---- Password Recovery ----
    void getSecurityQuestion(const QString &username);
    void requestPasswordReset(const QString &username);
    void verifySecurityAnswer(const QString &username, const QString &answer);
    void resetPassword(const QString &username, const QString &newPassword);


    // ---- Admin / Users ----
    void requestUsersList();
    void adminAction(const QString &type, const QString &username);
    void saveGenres(const QString &username, const QStringList &genres);

    // ---- Books ----
    void requestBooks();

    // ---- Cart ----
    void addToCart(const QString &username, const QString &bookId, int quantity = 1);
    void requestCart(const QString &username);
    void removeFromCart(const QString &username, const QString &bookId);
    void checkout(const QString &username);

    void searchBooks(const QString &query, const QString &field);
    void postReview(const QString &username, int bookId, int rating, const QString &comment);
    void getReviews(int bookId);
    void editReview(const QString &username, int bookId, int rating, const QString &comment);
    void deleteReview(const QString &username, int bookId);

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
    void checkoutFinished(bool success,
                          const QString &message,
                          double total,
                          double discountAmount,
                          double finalAmount);
    void libraryReceived(const QJsonArray &books);
    void purchaseHistoryReceived(const QJsonArray &history);

    // ---- Books ----
    void booksReceived(const QJsonArray &books);

    // ---- Cart ----
    void itemAddedToCart(bool success, const QString &message);
    void cartReceived(const QJsonArray &items);
    void itemRemovedFromCart(bool success, const QString &message);
    void checkoutFinished(bool success, const QString &message);

    void searchResultReceived(const QJsonArray &books);
    void reviewPosted(bool success, const QString &message);
    void reviewsReceived(const QJsonArray &reviews);
    void reviewEdited(bool success, const QString &message);
    void reviewDeleted(bool success, const QString &message);

private:
    QTcpSocket *m_socket;
    QByteArray m_buffer;
    void sendJson(const QJsonObject &json);
};
#endif
