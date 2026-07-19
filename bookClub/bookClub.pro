QT += quick qml core gui widgets network
QT += core gui widgets



QT       += core gui widgets network

# غیرفعال کردن موقت ماژول pdf برای رفع خطای کامپایل
# QT += pdf pdfwidgets

CONFIG += c++17

# لیست تمام فایل‌های اجرایی پروژه (بدون Bookreader)
SOURCES += \
    Autmmanager.cpp \
    customerpage.cpp \
    main.cpp \
    src/model/Admin.cpp \
    src/model/Book.cpp \
    src/model/Server.cpp \
    src/model/User.cpp \
    src/model/bookdetailpage.cpp \
    src/model/mainwindow.cpp

# لیست تمام هدرها (بدون Bookreader)
HEADERS += \
    Autmmanager.h \
    customerpage.h \
    src/model/Admin.h \
    src/model/Book.h \
    src/model/Publisher.h \
    src/model/Server.h \
    src/model/User.h \
    src/model/bookdetailpage.h \
    src/model/mainwindow.h

# فایل‌های طراحی رابط کاربری
FORMS +=

# تنظیمات پیش‌فرض برای نصب و مسیر هدف
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/mainwindow/target
INSTALLS += target
QT += core gui widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    src/model/Admin.cpp \
    src/model/Customer.cpp \
    src/model/User.cpp \
    src/network/Authmanager.cpp \
    src/ui/adminpage.cpp \
    src/ui/loginpage.cpp \
    src/ui/registerpage.cpp

HEADERS += \
    src/model/Admin.h \
    src/model/Customer.h \
    src/model/User.h \
    src/network/Authmanager.h \
    src/ui/adminpage.h \
    src/ui/loginpage.h \
    src/ui/registerpage.h

FORMS += \
    src/ui/adminpage.ui \
    src/ui/loginpage.ui \
    src/ui/registerpage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += src/model
INCLUDEPATH += src/network
INCLUDEPATH += $$PWD \
               $$PWD/ui \
               $$PWD/network \
               $$PWD/src/model
               INCLUDEPATH += $$PWD \
                              $$PWD/src

# لیست تمام فایل‌های اجرایی پروژه (بدون Bookreader)
SOURCES += \
    main.cpp \
    src/model/Book.cpp \
    src/model/User.cpp

# لیست تمام هدرها (بدون Bookreader)
HEADERS += \
    src/model/Book.h \
    src/model/User.h

# فایل‌های طراحی رابط کاربری
FORMS +=

# تنظیمات پیش‌فرض برای نصب و مسیر هدف
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/mainwindow/target
INSTALLS += target
QT += core gui widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    src/model/Admin.cpp \
    src/model/Customer.cpp \
    src/model/User.cpp \
    src/network/Authmanager.cpp \
    src/ui/adminpage.cpp \
    src/ui/loginpage.cpp \
    src/ui/registerpage.cpp

HEADERS += \
    src/model/Admin.h \
    src/model/Customer.h \
    src/model/User.h \
    src/network/Authmanager.h \
    src/ui/adminpage.h \
    src/ui/loginpage.h \
    src/ui/registerpage.h



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += src/model
INCLUDEPATH += src/network
INCLUDEPATH += $$PWD \
               $$PWD/ui \
               $$PWD/network \
               $$PWD/src/model
               INCLUDEPATH += $$PWD \
                              $$PWD/src

DISTFILES += \
    src/model/Login.qml
