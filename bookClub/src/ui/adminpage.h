#ifndef ADMINPAGE_H
#define ADMINPAGE_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidget>
#include <QPushButton>
#include "../network/Authmanager.h"

namespace Ui {
class AdminPage;
}

class AdminPage : public QWidget
{
    Q_OBJECT

public:
    explicit AdminPage(AuthManager *authManager, QWidget *parent = nullptr);
    ~AdminPage();

    void loadData();

signals:
    void logoutRequested();

private slots:
    void onRefreshButtonClicked();
    void handleUsersList(const QJsonArray &users);
    void onUserSelected(int row, int column);
    void filterUsers();
    void onBlockClicked();
    void onDeleteClicked();
    void handleActionResponse(bool success, const QString &message);

    void onAdminBooksReceived(const QJsonArray &books);
    void onEditBookClicked();
    void onDeleteBookClicked();
    void onAdminBookUpdated(bool success, const QString &message);
    void onAdminBookDeleted(bool success, const QString &message);

    void onAdminReviewsReceived(const QJsonArray &reviews);
    void onDeleteReviewClicked();
    void onAdminReviewDeleted(bool success, const QString &message);

private:
    Ui::AdminPage *ui;
    AuthManager *m_authManager;

    QJsonArray m_allUsers;
    QJsonArray m_filteredUsers;
    QString m_selectedUsername;

    void setupTableHeaders();
    void updateTable(const QJsonArray &usersToShow);
    void resetDetailLabels();

    void buildContentTabs();
    QJsonObject selectedBook() const;

    QJsonArray m_allBooks;
    QTableWidget *m_booksTable;
    QPushButton *m_editBookButton;
    QPushButton *m_deleteBookButton;

    QJsonArray m_allReviews;
    QTableWidget *m_reviewsTable;
    QPushButton *m_deleteReviewButton;
};

#endif
