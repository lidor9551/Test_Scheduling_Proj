#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // טעינה ישירה מהנתיב שבו הקובץ נמצא
    qDebug() << "Loading Main.qml from disk...";
    engine.load(QStringLiteral("Main.qml"));

    if (engine.rootObjects().isEmpty()) {
        qDebug() << ">>> FATAL ERROR: Engine root objects is empty!";
        return -1;
    }
    
    return app.exec();
}