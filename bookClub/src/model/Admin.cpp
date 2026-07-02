#include "Admin.h"

Admin::Admin(QObject *parent) : User(parent) {
}

// پیاده‌سازی متد مسدود سازی
void Admin::setBlockStatus(User* targetUser, bool status) {
    if (targetUser) {
        targetUser->setBlocked(status);
    }
}

void Admin::validatePublisher(const QString &brandName) {
    // منطق تایید ناشر
}