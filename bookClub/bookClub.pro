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
    src/model/AddBookDialog.cpp \
    src/model/Admin.cpp \
    src/model/Book.cpp \
    src/model/BookDetailsWidget.cpp \
    src/model/BookManager.cpp \
    src/model/Clientnetworkmanager.cpp \
    src/model/GlobalReviewManager.cpp \
    src/model/PdfReaderWidget.cpp \
    src/model/PersonalLibrary.cpp \
    src/model/PublisherDashboard.cpp \
    src/model/PublisherDashboard.cpp \
    src/model/Review.cpp \
    src/model/Review.cpp \
    src/model/ReviewManager.cpp \
    src/model/Server.cpp \
    src/model/Shoppingcart.cpp \
    src/model/User.cpp \
    src/model/bookdetailswidget.cpp \
    src/model/mainwindow.cpp \
    src/model/personallibrary.cpp

# لیست تمام هدرها (بدون Bookreader)
HEADERS += \
    Autmmanager.h \
    customerpage.h \
    src/model/AddBookDialog.h \
    src/model/Admin.h \
    src/model/Book.h \
    src/model/BookDetailsWidget.h \
    src/model/BookManager.h \
    src/model/ClientNetworkManager.h \
    src/model/GlobalReviewManager.h \
    src/model/PdfReaderWidget.h \
    src/model/PersonalLibrary.h \
    src/model/Publisher.h \
    src/model/PublisherDashboard.h \
    src/model/PublisherDashboard.h \
    src/model/Review.h \
    src/model/ReviewManager.h \
    src/model/Server.h \
    src/model/Shoppingcart.h \
    src/model/User.h \
    src/model/bookdetailswidget.h \
    src/model/mainwindow.h \
    src/model/ui_addbookdialog.h

# فایل‌های طراحی رابط کاربری
FORMS += \
    src/model/AddBookDialog.ui \
    src/model/AddBookDialog.ui

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
    src/model/PublisherDashboard.cpp \
    src/model/User.cpp \
    src/network/Authmanager.cpp \
    src/ui/adminpage.cpp \
    src/ui/loginpage.cpp \
    src/ui/registerpage.cpp

HEADERS += \
    src/model/Admin.h \
    src/model/Customer.h \
    src/model/PublisherDashboard.h \
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
    src/model/BookDetailsWidget.cpp \
    src/model/BookManager.cpp \
    src/model/PersonalLibrary.cpp \
    src/model/Review.cpp \
    src/model/User.cpp \
    src/model/bookdetailswidget.cpp \
    src/model/personallibrary.cpp

# لیست تمام هدرها (بدون Bookreader)
HEADERS += \
    src/model/Book.h \
    src/model/BookDetailsWidget.h \
    src/model/BookManager.h \
    src/model/PersonalLibrary.h \
    src/model/Review.h \
    src/model/ReviewManager.h \
    src/model/Shoppingcart.h \
    src/model/User.h \
    src/model/bookdetailswidget.h

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
    src/model/PublisherDashboard.cpp \
    src/model/User.cpp \
    src/network/Authmanager.cpp \
    src/ui/adminpage.cpp \
    src/ui/loginpage.cpp \
    src/ui/registerpage.cpp

HEADERS += \
    src/model/Admin.h \
    src/model/Customer.h \
    src/model/PublisherDashboard.h \
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
