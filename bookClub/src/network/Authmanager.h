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

    // ---- Profile ----
    void requestProfile(const QString &username);
    void changePassword(const QString &username, const QString &oldPassword, const QString &newPassword);

    // ---- Books ----
    void requestBooks();

    // ---- Cart ----
    void addToCart(const QString &username, const QString &bookId, int quantity = 1);
    void requestCart(const QString &username);
    void removeFromCart(const QString &username, const QString &bookId);
    void checkout(const QString &username);
    
    void searchBooks(const QString &query, const QString &field);
    void postReview(const QString &username, const QString &bookId, int rating, const QString &comment);
    void getReviews(const QString &bookId);
    void editReview(const QString &username, const QString &reviewId, int rating, const QString &comment);
    void deleteReview(const QString &username, const QString &reviewId);

    // ---- saved books ----
    void saveBookForLater(const QString &username, const QString &bookId);
    void unsaveBook(const QString &username, const QString &bookId);
    void requestSavedBooks(const QString &username);

    // ---- shelves ----
    void createShelf(const QString &username, const QString &name);
    void deleteShelf(const QString &username, const QString &shelfId);
    void addBookToShelf(const QString &username, const QString &shelfId, const QString &bookId);
    void removeBookFromShelf(const QString &username, const QString &shelfId, const QString &bookId);
    void requestShelves(const QString &username);

    // ---- reading progress ----
    void requestReadingProgress(const QString &username, const QString &bookId);
    void saveReadingProgress(const QString &username, const QString &bookId, int page);

    // ---- publisher ----
    void publishBook(const QString &username, const QJsonObject &bookData);
    void updateBook(const QString &username, const QString &bookId, const QJsonObject &fields);
    void deactivateBook(const QString &username, const QString &bookId);
    void activateBook(const QString &username, const QString &bookId);
    void requestPublisherBooks(const QString &username);
    void requestPublisherStats(const QString &username);

    // ---- admin(books & rates) ----
    void requestAllBooksForAdmin();
    void adminUpdateBook(const QString &bookId, const QJsonObject &fields);
    void adminDeleteBook(const QString &bookId);
    void requestAllReviewsForAdmin();
    void adminDeleteReview(const QString &reviewId);

private slots:
    void onReadyRead();

signals:
    void loginFinished(bool success, const QString &message, const QString &role, bool firstLogin);
    void registerFinished(bool success, const QString &message);
    void usersListReceived(const QJsonArray &users);
    void actionFinished(bool success, const QString &message);
    void genresSaved(bool success);
    void profileReceived(const QJsonObject &profile);
    void passwordChanged(bool success, const QString &message);
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
    void cartSummaryReceived(int itemCount, double total, double discountAmount, double finalAmount);
    void itemRemovedFromCart(bool success, const QString &message);
    void checkoutFinished(bool success, const QString &message);

    void searchResultReceived(const QJsonArray &books);
    void reviewPosted(bool success, const QString &message);
    void reviewsReceived(const QString &bookId, const QJsonArray &reviews);
    void reviewEdited(bool success, const QString &message);
    void reviewDeleted(bool success, const QString &message);

    // ---- saved books ----
    void savedBookChanged(bool success, const QString &message);
    void savedBooksReceived(const QJsonArray &items);

    // ---- shelves ----
    void shelvesReceived(bool success, const QString &message, const QJsonArray &shelves);

    // ---- reading progress ----
    void readingProgressReceived(const QString &bookId, int page);

    // ---- publisher ----
    void bookPublished(bool success, const QString &message, const QJsonObject &book);
    void bookUpdated(bool success, const QString &message);
    void bookActiveStatusChanged(bool success, const QString &message);
    void publisherBooksReceived(const QJsonArray &books);
    void publisherStatsReceived(const QJsonObject &stats);

    // ---- admin(books & rates) ----
    void adminBooksReceived(const QJsonArray &books);
    void adminBookUpdated(bool success, const QString &message);
    void adminBookDeleted(bool success, const QString &message);
    void adminReviewsReceived(const QJsonArray &reviews);
    void adminReviewDeleted(bool success, const QString &message);

private:
    QTcpSocket *m_socket;
    QByteArray m_buffer;
    void sendJson(const QJsonObject &json);
    };
#endif
