#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QQuickStyle>
#include <QCoreApplication>

#include "presentation/AppController.h"
#include "presentation/CalendarManager.h"
#include "domain/Course.h"
#include "domain/ExamPeriod.h"

int main(int argc, char *argv[]) {
    // Force a non-native style to allow full UI customization
    QQuickStyle::setStyle("Basic");

    QGuiApplication app(argc, argv);

    AppController appController;
    CalendarManager calendarManager;

    appController.setCalendarManager(&calendarManager);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("appController", &appController);
    engine.rootContext()->setContextProperty("calendarManager", &calendarManager);
 
    qRegisterMetaType<ExamPeriod>("ExamPeriod");
    qRegisterMetaType<Course>("Course");

    qDebug() << "Loading Main.qml from disk...";
    engine.load(QUrl::fromLocalFile(
    QCoreApplication::applicationDirPath() + "/qml/Main.qml"
));

    if (engine.rootObjects().isEmpty()) {
        qDebug() << ">>> FATAL ERROR: Engine root objects is empty!";
        return -1;
    }

    return app.exec();
}