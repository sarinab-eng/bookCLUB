QT += core gui widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/model/Admin.cpp \
    src/model/Book.cpp \
    src/model/Customer.cpp \
    src/model/Publisher.cpp \
    src/model/User.cpp \
    src/network/Authmanager.cpp \
    src/ui/addeditbookdialog.cpp \
    src/ui/adminpage.cpp \
    src/ui/bookdetailpage.cpp \
    src/ui/cartpage.cpp \
    src/ui/customerpage.cpp \
    src/ui/genreselectiondialog.cpp \
    src/ui/librarypage.cpp \
    src/ui/loginpage.cpp \
    src/ui/profilepage.cpp \
    src/ui/publisherpage.cpp \
    src/ui/registerpage.cpp

HEADERS += \
    mainwindow.h \
    src/model/Admin.h \
    src/model/Book.h \
    src/model/Customer.h \
    src/model/Publisher.h \
    src/model/User.h \
    src/network/Authmanager.h \
    src/ui/addeditbookdialog.h \
    src/ui/adminpage.h \
    src/ui/bookdetailpage.h \
    src/ui/cartpage.h \
    src/ui/customerpage.h \
    src/ui/genreselectiondialog.h \
    src/ui/librarypage.h \
    src/ui/loginpage.h \
    src/ui/profilepage.h \
    src/ui/publisherpage.h \
    src/ui/registerpage.h

# ماژول مطالعه‌ی داخلی PDF روی ویندوز فقط با کیت MSVC در دسترس است، نه
# MinGW (چون QtPdf بر پایه‌ی PDFium ساخته شده و برای MinGW بیلد نمیشود). اگر این
# ماژول موجود نباشد PdfReaderPage اصلاً کامپایل نمی‌شود و «مطالعه کتاب»
# فایل رو با نرم‌افزار پیش‌فرض سیستم باز می‌کنه ( کد src/ui/customerpage.cpp
# داخل بلاک #ifdef HAVE_QT_PDF ).
qtHaveModule(pdf):qtHaveModule(pdfwidgets) {
    QT += pdf pdfwidgets
    DEFINES += HAVE_QT_PDF
    SOURCES += src/ui/pdfreaderpage.cpp
    HEADERS += src/ui/pdfreaderpage.h
} else {
    warning("Qt PDF module not found - in-app PDF reader disabled, falling back to external file open.")
}

FORMS += \
    mainwindow.ui \
    src/ui/adminpage.ui \
    src/ui/cartpage.ui \
    src/ui/customerpage.ui \
    src/ui/genreselectiondialog.ui \
    src/ui/librarypage.ui \
    src/ui/loginpage.ui \
    src/ui/registerpage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD \
               $$PWD/src \
               $$PWD/src/ui \
               $$PWD/src/network \
               $$PWD/src/model
