QT += core gui widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/model/Admin.cpp \
    src/model/Customer.cpp \
    src/model/Publisher.cpp \
    src/model/User.cpp \
    src/network/Authmanager.cpp \
    src/ui/adminpage.cpp \
    src/ui/loginpage.cpp \
    src/ui/registerpage.cpp

HEADERS += \
    mainwindow.h \
    src/model/Admin.h \
    src/model/Customer.h \
    src/model/Publisher.h \
    src/model/User.h \
    src/network/Authmanager.h \
    src/ui/adminpage.h \
    src/ui/loginpage.h \
    src/ui/registerpage.h

FORMS += \
    mainwindow.ui \
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
