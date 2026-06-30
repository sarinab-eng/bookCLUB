#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QDateTime>

// کلاس User به صورت انتزاعی (Abstract) تعریف شده است
class User : public QObject
{
    Q_OBJECT

public:
    explicit User(const QString &username, const QString &email, const QString &hashedPassword, QObject *parent = nullptr);
    virtual ~User() {}

    // Getters
    QString getUsername() const;
    QString getEmail() const;
    QDateTime getRegistrationDate() const;

    // متد بررسی صحت پسورد
    bool checkPassword(const QString &password) const;

    // متد خالص مجازی (Pure Virtual): هر نقش باید پیاده‌سازی مخصوص خود را داشته باشد
    virtual void logActivity(const QString &action) = 0;

protected:
    QString m_username;
    QString m_email;
    QString m_hashedPassword; // ذخیره پسورد به صورت هش شده برای امنیت
    QDateTime m_registrationDate;
};

#endif // USER_H