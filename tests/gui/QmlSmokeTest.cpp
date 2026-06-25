#include "presentation/AppController.h"
#include "presentation/CalendarManager.h"

#include "domain/Course.h"
#include "domain/ExamPeriod.h"
#include "domain/ScheduleGenerationResult.h"

#include "TestMacros.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QEventLoop>
#include <QFileInfo>
#include <QGuiApplication>
#include <QMetaType>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QTimer>
#include <QUrl>
#include <QVariant>
#include <QVariantList>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

namespace {

QString qmlPath(const QString& fileName) {
    return QFileInfo("../qml/" + fileName).absoluteFilePath();
}

void failWithQmlErrors(const QString& fileName, const QList<QQmlError>& errors) {
    std::cerr << "FAILED: QML file did not load: "
              << fileName.toStdString()
              << std::endl;

    for (const QQmlError& error : errors) {
        std::cerr << error.toString().toStdString() << std::endl;
    }

    TEST_FAIL("QML load failed for " << fileName.toStdString());
}

void seedCalendarManager(CalendarManager& calendarManager) {
    ExcludedRange excluded;
    excluded.start = Date(3, 1, 2026);
    excluded.end = Date(3, 1, 2026);
    excluded.reason = "Saturday";

    ExamPeriod period(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(7, 1, 2026),
        { excluded }
    );

    QVariantList periods;
    periods.append(QVariant::fromValue(period));

    calendarManager.setData(periods, QVariantList{});
}

void configureContext(
    QQmlEngine& engine,
    AppController& appController,
    CalendarManager& calendarManager
) {
    engine.rootContext()->setContextProperty("appController", &appController);
    engine.rootContext()->setContextProperty("calendarManager", &calendarManager);
    engine.rootContext()->setContextProperty("outputManager", appController.outputManager());
}

void waitForComponentIfNeeded(QQmlComponent& component) {
    if (component.status() != QQmlComponent::Loading) {
        return;
    }

    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    QObject::connect(
        &component,
        &QQmlComponent::statusChanged,
        &loop,
        [&](QQmlComponent::Status status) {
            if (status != QQmlComponent::Loading) {
                loop.quit();
            }
        }
    );

    QObject::connect(
        &timeoutTimer,
        &QTimer::timeout,
        &loop,
        [&]() {
            loop.quit();
        }
    );

    timeoutTimer.start(3000);
    loop.exec();
    timeoutTimer.stop();
}

void testQmlComponentLoads(
    const QString& fileName,
    AppController& appController,
    CalendarManager& calendarManager
) {
    QQmlEngine engine;
    configureContext(engine, appController, calendarManager);

    const QString path = qmlPath(fileName);

    TEST_EXPECT_TRUE(QFileInfo::exists(path));

    QQmlComponent component(&engine, QUrl::fromLocalFile(path));

    waitForComponentIfNeeded(component);

    if (component.isError()) {
        failWithQmlErrors(fileName, component.errors());
    }

    std::unique_ptr<QObject> object(component.create());

    if (!object) {
        failWithQmlErrors(fileName, component.errors());
    }

    TEST_EXPECT_TRUE(object != nullptr);

    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}

void testMainWindowLoads(
    AppController& appController,
    CalendarManager& calendarManager
) {
    QQmlApplicationEngine engine;
    configureContext(engine, appController, calendarManager);

    const QString path = qmlPath("Main.qml");

    TEST_EXPECT_TRUE(QFileInfo::exists(path));

    engine.load(QUrl::fromLocalFile(path));

    if (engine.rootObjects().isEmpty()) {
        std::cerr << "FAILED: Main.qml did not create any root objects."
                  << std::endl;
        TEST_FAIL("Main.qml root object list is empty");
    }

    QObject* rootObject = engine.rootObjects().first();

    TEST_EXPECT_TRUE(rootObject != nullptr);
    TEST_EXPECT_TRUE(qobject_cast<QQuickWindow*>(rootObject) != nullptr);

    QQuickWindow* window = qobject_cast<QQuickWindow*>(rootObject);
    TEST_EXPECT_EQ(window->title(), QString("מתזמן בחינות 2.0 - קלט"));

    window->close();

    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}

void testSettingsScreenLoadsWithControllerState() {
    AppController appController;
    CalendarManager calendarManager;

    seedCalendarManager(calendarManager);

    appController.saveHardConstraints(
        true,  2,
        false, 3,
        true,  4,
        false, 5,
        true,  6
    );

    testQmlComponentLoads("SettingsScreen.qml", appController, calendarManager);
}

void testCalendarScreenLoadsWithCalendarData() {
    AppController appController;
    CalendarManager calendarManager;

    seedCalendarManager(calendarManager);

    testQmlComponentLoads("CalendarScreen.qml", appController, calendarManager);
}

void testOutputScreenLoadsWithOutputManager() {
    AppController appController;
    CalendarManager calendarManager;

    seedCalendarManager(calendarManager);

    testQmlComponentLoads("OutputScreen.qml", appController, calendarManager);
}

void testPeriodEditorScreenLoadsWithCalendarData() {
    AppController appController;
    CalendarManager calendarManager;

    seedCalendarManager(calendarManager);

    testQmlComponentLoads("PeriodEditorScreen.qml", appController, calendarManager);
}

void testMainQmlLoads() {
    AppController appController;
    CalendarManager calendarManager;

    seedCalendarManager(calendarManager);

    testMainWindowLoads(appController, calendarManager);
}

} // namespace

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", QByteArray("offscreen"));
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArray("Basic"));

    QGuiApplication app(argc, argv);

    qRegisterMetaType<Course>("Course");
    qRegisterMetaType<ExamPeriod>("ExamPeriod");
    qRegisterMetaType<std::vector<ScheduleGenerationResult>>(
        "std::vector<ScheduleGenerationResult>"
    );

    testSettingsScreenLoadsWithControllerState();
    testCalendarScreenLoadsWithCalendarData();
    testOutputScreenLoadsWithOutputManager();
    testPeriodEditorScreenLoadsWithCalendarData();
    testMainQmlLoads();

    std::cout << "QmlSmokeTest passed." << std::endl;
    return EXIT_SUCCESS;
}