#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QDateTime>

class User : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString username READ getUsername CONSTANT)

public:
    explicit User(QObject *parent = nullptr);
    User(const QString &username, const QString &email, const QString &password, QObject *parent = nullptr);

    static QString hashPassword(const QString &password);
    static QString encrypt(const QString &data, const QString &key = "");
    static QString decrypt(const QString &data, const QString &key = "");

    QString getUsername() const;
    QString getEmail() const;
    QDateTime getRegistrationDate() const;
    bool getIsBlocked() const;
    virtual QString getRole() const { return QString(); }

    void setBlocked(bool blocked);

    bool checkPassword(const QString &password) const;

private:
    QString m_username;
    QString m_email;
    QString m_hashedPassword;
    QString m_securityQuestion;
    QString m_securityAnswer;
    QDateTime m_registrationDate;
    bool m_isBlocked;
};

#endif
