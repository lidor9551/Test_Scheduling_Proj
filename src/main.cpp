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

    // Expose the AppController and CalendarManager to QML with appController.(methods/properties) and calendarManager.(methods/properties)
    engine.rootContext()->setContextProperty("appController", &appController);
    engine.rootContext()->setContextProperty("calendarManager", &calendarManager);
 
    // Register the ExamPeriod and Course types with the Qt meta-object system for use in signals/slots and QML
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