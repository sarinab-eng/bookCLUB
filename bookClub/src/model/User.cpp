#include "User.h"

// سازنده کلاس: متغیرها را مقداردهی می‌کند و تاریخ ثبت‌نام را زمان فعلی سیستم قرار می‌دهد
User::User(const QString &username, const QString &email, const QString &hashedPassword, QObject *parent)
    : QObject(parent), m_username(username), m_email(email), m_hashedPassword(hashedPassword)
{
    m_registrationDate = QDateTime::currentDateTime();
}

// بازگرداندن نام کاربری
QString User::getUsername() const
{
    return m_username;
}

// بازگرداندن ایمیل
QString User::getEmail() const
{
    return m_email;
}

// بازگرداندن تاریخ ثبت‌نام
QDateTime User::getRegistrationDate() const
{
    return m_registrationDate;
}

// بررسی پسورد (فعلاً یک مقایسه ساده، در مراحل بعد می‌توان آن را با الگوریتم‌های امنیتی ترکیب کرد)
bool User::checkPassword(const QString &password) const
{
    return (m_hashedPassword == password);
}