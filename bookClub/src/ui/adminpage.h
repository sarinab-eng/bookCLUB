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
    void filterUsers(); // تابع فیلتر
    void onBlockClicked();
    void onDeleteClicked();
    void handleActionResponse(bool success, const QString &message);

    // ---- مدیریت کتاب‌ها و محتوا ----
    void onAdminBooksReceived(const QJsonArray &books);
    void onEditBookClicked();
    void onDeleteBookClicked();
    void onAdminBookUpdated(bool success, const QString &message);
    void onAdminBookDeleted(bool success, const QString &message);

    // ---- نظارت بر نظرات ----
    void onAdminReviewsReceived(const QJsonArray &reviews);
    void onDeleteReviewClicked();
    void onAdminReviewDeleted(bool success, const QString &message);

private:
    Ui::AdminPage *ui;
    AuthManager *m_authManager;

    QJsonArray m_allUsers;      // لیست خام دریافتی از سرور (منبع اصلی)
    QJsonArray m_filteredUsers; // لیستی که بعد از فیلتر در جدول نمایش داده می‌شود
    QString m_selectedUsername; // نام کاربری انتخاب شده فعلی

    void setupTableHeaders();
    void updateTable(const QJsonArray &usersToShow);
    void resetDetailLabels(); // تابع کمکی برای پاک کردن جزئیات سمت راست

    void buildContentTabs();
    QJsonObject selectedBook() const;

    // ---- تب کتاب‌ها ----
    QJsonArray m_allBooks;
    QTableWidget *m_booksTable;
    QPushButton *m_editBookButton;
    QPushButton *m_deleteBookButton;

    // ---- تب نظرات ----
    QJsonArray m_allReviews;
    QTableWidget *m_reviewsTable;
    QPushButton *m_deleteReviewButton;
};

#endif // ADMINPAGE_H
