#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include "book.h" // چون هر دو در یک پوشه هستند، مسیر مستقیم دادم

class BookManager : public QObject {
    Q_OBJECT
public:
    explicit BookManager(QTcpSocket *socket, QObject *parent = nullptr);

    // متدهای درخواست از سرور
    void requestAllBooks();
    void requestBooksByGenre(const QString &genre);
    void addNewBook(Book *book); // ارسال کتاب جدید به سرور (توسط ناشر)

signals:
    // سیگنالی که وقتی لیست کتاب‌ها از سرور رسید، به UI خبر می‌دهد
    void booksReceived(const QList<Book*> &books);
    void actionFinished(bool success, const QString &message);

private:
    QTcpSocket *socket;
};

#endif // BOOKMANAGER_H
