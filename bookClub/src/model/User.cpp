#include "User.h"
#include <QCryptographicHash>

// سازنده پیش‌فرض
User::User(QObject *parent)
    : QObject(parent), m_username(""), m_email(""), m_hashedPassword(""), m_isBlocked(false)
{
    m_registrationDate = QDateTime::currentDateTime();
}

// سازنده اصلی با پارامترها
User::User(const QString &username, const QString &email, const QString &password, QObject *parent)
    : QObject(parent), m_username(username), m_email(email), m_isBlocked(false)
{
    m_hashedPassword = hashPassword(password);
    m_registrationDate = QDateTime::currentDateTime();
}

// هش کردن پسورد (SHA-256) برای امنیت بالا
QString User::hashPassword(const QString &password) {
    if (password.isEmpty()) return "";
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

// رمزنگاری XOR برای داده‌های حساس (مثل سوال امنیتی)
QString User::encrypt(const QString &data, const QString &key) {
    QString finalKey = key.isEmpty() ? "BookClubKey2024" : key;
    QByteArray result = data.toUtf8();
    QByteArray k = finalKey.toUtf8();

    for (int i = 0; i < result.size(); ++i)
        result[i] = result[i] ^ k[i % k.size()];

    return result.toBase64();
}

// رمزگشایی داده‌ها
QString User::decrypt(const QString &data, const QString &key) {
    QString finalKey = key.isEmpty() ? "BookClubKey2024" : key;
    QByteArray result = QByteArray::fromBase64(data.toUtf8());
    QByteArray k = finalKey.toUtf8();

    for (int i = 0; i < result.size(); ++i)
        result[i] = result[i] ^ k[i % k.size()];

    return QString::fromUtf8(result);
}

// متدهای بازیابی اطلاعات (Getters)
QString User::getUsername() const { return m_username; }
QString User::getEmail() const { return m_email; }
QDateTime User::getRegistrationDate() const { return m_registrationDate; }
bool User::getIsBlocked() const { return m_isBlocked; }

// متد بررسی صحت پسورد در هنگام لاگین
bool User::checkPassword(const QString &password) const {
    // پسورد وارد شده را هش کرده و با مقدار ذخیره شده در دیتابیس مقایسه می‌کنیم
    return (m_hashedPassword == hashPassword(password));
}
