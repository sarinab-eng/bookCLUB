#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
        QApplication a(argc, argv);

        QFont font("Segoe UI", 10); // نام فونت و سایز
        a.setFont(font);

    MainWindow w;
    w.show();
    return QApplication::exec();
}
