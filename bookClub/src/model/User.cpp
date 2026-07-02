#include "User.h"
#include <QCryptographicHash>

User::User(QObject *parent) : QObject(parent), isBlocked(false) {
    this->username = "";
    this->passwordHash = "";
    this->securityQuestion = "";
    this->securityAnswer = "";
}

QString User::hashPassword(const QString &pass) {
    return QString(QCryptographicHash::hash(pass.toUtf8(), QCryptographicHash::Sha256).toHex());
}

QString User::encrypt(const QString &data, const QString &key) {
    QByteArray result = data.toUtf8();
    QByteArray k = key.toUtf8();
    if (k.isEmpty()) return data;
    for (int i = 0; i < result.size(); ++i)
        result[i] = result[i] ^ k[i % k.size()];
    return result.toBase64();
}

QString User::decrypt(const QString &data, const QString &key) {
    QByteArray result = QByteArray::fromBase64(data.toUtf8());
    QByteArray k = key.toUtf8();
    if (k.isEmpty()) return data;
    for (int i = 0; i < result.size(); ++i)
        result[i] = result[i] ^ k[i % k.size()];
    return QString::fromUtf8(result);
}