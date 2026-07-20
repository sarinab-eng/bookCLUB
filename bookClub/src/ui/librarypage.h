#ifndef LIBRARYPAGE_H
#define LIBRARYPAGE_H

#include <QWidget>
#include <QJsonArray>
#include <QTableWidget>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
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
    void showHistoryTab();

signals:
    void bookDetailRequested(const QJsonObject &book);
    void readBookRequested(const QJsonObject &book);

private slots:
    void onLibraryReceived(const QJsonArray &books);
    void onPurchaseHistoryReceived(const QJsonArray &history);
    void onSavedBooksReceived(const QJsonArray &items);
    void onSavedBookChanged(bool success, const QString &message);
    void onShelvesReceived(bool success, const QString &message, const QJsonArray &shelves);

    void onReadBookClicked();
    void onViewDetailsClicked();
    void onRemoveSavedClicked();
    void onShelfSelectionChanged();
    void onNewShelfClicked();
    void onDeleteShelfClicked();
    void onAddBookToShelfClicked();
    void onRemoveBookFromShelfClicked();

private:
    Ui::LibraryPage *ui;
    AuthManager *m_authManager;
    QString m_username;

    QJsonArray m_libraryBooks;  // کتاب‌های خریداری‌شده (برای «مشاهده جزئیات»/«باز کردن فایل» و پرکردن ترکیب‌های قفسه)
    QJsonArray m_shelves;       // آخرین لیست قفسه‌ها به همراه جزئیات کتاب‌های هرکدام
    QString m_selectedShelfId;

    void setupTables();
    void buildExtraTabs();
    QJsonObject currentShelf() const;

    QTabWidget *m_tabs = nullptr;
    int m_historyTabIndex = -1;

    // ---- تب کتاب‌های ذخیره‌شده ----
    QWidget *m_savedTab;
    QTableWidget *m_savedTable;
    QPushButton *m_removeSavedButton;

    // ---- تب قفسه‌ها ----
    QWidget *m_shelvesTab;
    QListWidget *m_shelvesList;
    QPushButton *m_newShelfButton;
    QPushButton *m_deleteShelfButton;
    QTableWidget *m_shelfContentsTable;
    QComboBox *m_addToShelfCombo;
    QPushButton *m_addBookToShelfButton;
    QPushButton *m_removeBookFromShelfButton;
};

#endif // LIBRARYPAGE_H
