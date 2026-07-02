#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QCryptographicHash>

class User : public QObject {
    Q_OBJECT
public:
    explicit User(QObject *parent = nullptr);

    QString username;
    QString passwordHash;
    QString securityQuestion;
    QString securityAnswer;
    bool isBlocked;

    // متد خالص مجازی برای انتزاعی کردن کلاس
    virtual QString getRole() const = 0;

    static QString hashPassword(const QString &pass);
    static QString encrypt(const QString &data, const QString &key = "bookClubKey");
    static QString decrypt(const QString &data, const QString &key = "bookClubKey");

    // متد کمکی برای تغییر وضعیت مسدودیت
    void setBlocked(bool status) { isBlocked = status; }
};

#endif