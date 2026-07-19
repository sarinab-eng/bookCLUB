#ifndef ADMINPAGE_H
#define ADMINPAGE_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
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

private slots:
    void onRefreshButtonClicked();
    void handleUsersList(const QJsonArray &users);
    void onUserSelected(int row, int column);
    void filterUsers(); // تابع فیلتر
    void onBlockClicked();
    void onDeleteClicked();
    void handleActionResponse(bool success, const QString &message);

private:
    Ui::AdminPage *ui;
    AuthManager *m_authManager;

    QJsonArray m_allUsers;      // لیست خام دریافتی از سرور (منبع اصلی)
    QJsonArray m_filteredUsers; // لیستی که بعد از فیلتر در جدول نمایش داده می‌شود
    QString m_selectedUsername; // نام کاربری انتخاب شده فعلی

    void setupTableHeaders();
    void updateTable(const QJsonArray &usersToShow);
    void resetDetailLabels(); // تابع کمکی برای پاک کردن جزئیات سمت راست
};

#endif // ADMINPAGE_H