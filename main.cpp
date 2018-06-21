#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QtQml>

#include "MapManager.h"

QObject *g_pQmlRoot = NULL;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<MapManager>("MapCanvasManager", 1, 0, "MapManager");

    QQuickView viewer;

    viewer.setResizeMode(QQuickView::SizeViewToRootObject);
    viewer.setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    g_pQmlRoot = (QObject*)(viewer.rootObject());

    QObject::connect(viewer.engine(), SIGNAL(quit()), &app, SLOT(quit()));

    viewer.show();

    return app.exec();
}
