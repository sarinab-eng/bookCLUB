#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"
#include <QVector>

class Admin : public User {
    Q_OBJECT
public:
    explicit Admin(QObject *parent = nullptr);

    QString getRole() const override { return "Admin"; }

    // متدهای نظارتی
    void setBlockStatus(User* targetUser, bool status);
    void validatePublisher(const QString &brandName);
};

#endif // ADMIN_H