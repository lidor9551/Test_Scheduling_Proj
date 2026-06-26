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

/*
 * main.cpp is the application entry point.
 *
 * It initializes the Qt application, creates the main controller objects,
 * exposes them to QML, loads the main QML screen, and starts the event loop.
 */
int main(int argc, char *argv[]) {
    // Force a non-native style to allow full UI customization
    /*
     * Use the Basic Qt Quick Controls style.
     *
     * This gives the QML UI a predictable look and allows full visual customization
     * instead of relying on platform-specific native styles.
     */
    QQuickStyle::setStyle("Basic");

    /*
     * QGuiApplication manages the Qt GUI application lifecycle.
     *
     * It processes command-line arguments and owns the main event loop.
     */
    QGuiApplication app(argc, argv);

    /*
     * AppController is the main bridge between QML and the application logic.
     *
     * CalendarManager manages exam-period calendar data for the QML calendar screens.
     */
    AppController appController;
    CalendarManager calendarManager;

    /*
     * Connect the controller to the calendar manager.
     *
     * This allows AppController to synchronize loaded or edited exam periods
     * with the calendar-related workflow.
     */
    appController.setCalendarManager(&calendarManager);

    /*
     * QQmlApplicationEngine loads and runs the QML user interface.
     */
    QQmlApplicationEngine engine;

    // Expose the AppController and CalendarManager to QML with appController.(methods/properties) and calendarManager.(methods/properties)
    /*
     * Expose C++ objects to QML as context properties.
     *
     * QML files can access them directly using:
     * - appController
     * - calendarManager
     */
    engine.rootContext()->setContextProperty("appController", &appController);
    engine.rootContext()->setContextProperty("calendarManager", &calendarManager);
 
    // Register the ExamPeriod and Course types with the Qt meta-object system for use in signals/slots and QML
    /*
     * Register domain types with Qt's meta-object system.
     *
     * This is required when these types are used in QVariant, signals, slots,
     * or communication between C++ and QML.
     */
    qRegisterMetaType<ExamPeriod>("ExamPeriod");
    qRegisterMetaType<Course>("Course");

    /*
     * Load the main QML file from the application output directory.
     */
    engine.load(QUrl::fromLocalFile(
    QCoreApplication::applicationDirPath() + "/qml/Main.qml"
));

    /*
     * If no root QML object was created, loading failed.
     *
     * Returning -1 tells the operating system that the application ended
     * because of an initialization error.
     */
    if (engine.rootObjects().isEmpty()) {
        qCritical() << ">>> FATAL ERROR: Engine root objects is empty!";
        return -1;
    }

    /*
     * Start the Qt event loop.
     *
     * From this point, the application reacts to UI events, signals, and slots.
     */
    return app.exec();
}