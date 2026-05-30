#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QMetaType>
#include <QQmlContext> 
#include "scheduler/ScheduleGenerator.h"

Q_DECLARE_METATYPE(std::vector<std::vector<int>>)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    SchedulingBlock emptyBlock; 
    ScheduleGenerator myGenerator(emptyBlock);

    qRegisterMetaType<std::vector<std::vector<int>>>("std::vector<std::vector<int>>");

    engine.rootContext()->setContextProperty("scheduler", &myGenerator);

    qDebug() << "Loading Main.qml from disk...";
    engine.load(QStringLiteral("Main.qml"));

    if (engine.rootObjects().isEmpty()) {
        qDebug() << ">>> FATAL ERROR: Engine root objects is empty!";
        return -1;
    }
    
    return app.exec();
}