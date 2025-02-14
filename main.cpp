#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "CoordinateConverter.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    QGuiApplication::setOrganizationName("S1yang");
    QGuiApplication::setOrganizationDomain("www.sb6657.cn");

    QQmlApplicationEngine engine;
    CoordinateConverter converter;
    engine.rootContext()->setContextProperty("converter", &converter);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
