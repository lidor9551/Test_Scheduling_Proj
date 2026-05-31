#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

#include "gui/AppController.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    AppController appController;
    engine.rootContext()->setContextProperty("appController", &appController);

    qDebug() << "Loading Main.qml from disk...";
    engine.load(QStringLiteral("Main.qml"));

    if (engine.rootObjects().isEmpty()) {
        qDebug() << ">>> FATAL ERROR: Engine root objects is empty!";
        return -1;
    }

    return app.exec();
}