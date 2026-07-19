#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>

class User : public QObject {
    Q_OBJECT
public:
    explicit User(QObject *parent = nullptr);

    QString username;
    QString passwordHash;
    QString securityQuestion;
    QString securityAnswer;
    bool isBlocked;

    virtual QString getRole() const = 0;

    // فقط تعریف توابع در اینجا باشد
    static QString hashPassword(const QString& password);
    static QString encrypt(const QString& data, const QString &key = "");
    static QString decrypt(const QString& data, const QString &key = "");

    void setBlocked(bool status) { isBlocked = status; }
};

#endif