#ifndef LIBRARYPAGE_H
#define LIBRARYPAGE_H

#include <QWidget>
#include <QJsonArray>
#include "Authmanager.h"

namespace Ui {
class LibraryPage;
}

class LibraryPage : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryPage(AuthManager *authManager, QWidget *parent = nullptr);
    ~LibraryPage();

    void setUsername(const QString &username);
    void requestLibraryRefresh();

private slots:
    void onLibraryReceived(const QJsonArray &books);
    void onPurchaseHistoryReceived(const QJsonArray &history);

private:
    Ui::LibraryPage *ui;
    AuthManager *m_authManager;
    QString m_username;

    void setupTables();
};

#endif // LIBRARYPAGE_H
