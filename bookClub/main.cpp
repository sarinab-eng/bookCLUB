#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // مسیر فایل QML شما - دقت کنید که نام و مسیر درست باشد
    const QUrl url(QStringLiteral("qrc:/src/model/Login.qml"));

    // اگر از فایل مستقیم (بدون Resource) استفاده می‌کنید:
    // const QUrl url = QUrl::fromLocalFile("src/model/Login.qml");

    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
