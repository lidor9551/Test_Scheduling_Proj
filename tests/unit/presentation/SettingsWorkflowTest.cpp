#include "presentation/AppController.h"
#include "TestMacros.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

#include <cstdlib>
#include <iostream>
#include <vector>

namespace {

struct GenerationWaitResult {
    bool completed = false;
    bool failed = false;
    bool timedOut = false;
};

bool isGenerationTerminal(const AppController& controller) {
    if (!controller.errorMessage().isEmpty()) {
        return true;
    }

    return controller.statusMessage().contains("השיבוץ הושלם");
}

GenerationWaitResult waitForGeneration(AppController& controller, int timeoutMs = 5000) {
    GenerationWaitResult result;

    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    QObject::connect(
        &controller,
        &AppController::statusMessageChanged,
        &loop,
        [&]() {
            if (isGenerationTerminal(controller)) {
                result.completed = controller.errorMessage().isEmpty();
                result.failed = !controller.errorMessage().isEmpty();
                loop.quit();
            }
        }
    );

    QObject::connect(
        &controller,
        &AppController::errorMessageChanged,
        &loop,
        [&]() {
            if (!controller.errorMessage().isEmpty()) {
                result.failed = true;
                loop.quit();
            }
        }
    );

    QObject::connect(
        &timeoutTimer,
        &QTimer::timeout,
        &loop,
        [&]() {
            result.timedOut = true;
            loop.quit();
        }
    );

    if (isGenerationTerminal(controller)) {
        result.completed = controller.errorMessage().isEmpty();
        result.failed = !controller.errorMessage().isEmpty();
        return result;
    }

    timeoutTimer.start(timeoutMs);
    loop.exec();
    timeoutTimer.stop();

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    return result;
}

void loadSimpleValidData(AppController& controller) {
    const QString basePath = "../tests/data/valid/simple/";

    controller.setCoursesFilePath(basePath + "courses.txt");
    controller.setExamPeriodsFilePath(basePath + "periods.txt");

    controller.replaceData();

    TEST_EXPECT_TRUE(controller.errorMessage().isEmpty());
    TEST_EXPECT_TRUE(controller.hasData());

    TEST_EXPECT_EQ(controller.courseCount(), 3);
    TEST_EXPECT_EQ(controller.examPeriodCount(), 1);

    TEST_EXPECT_EQ(controller.getExamPeriodDays(), 7);
}

void selectSimpleProgram(AppController& controller) {
    TEST_EXPECT_TRUE(controller.selectedPrograms().isEmpty());

    controller.toggleProgram("83101");

    QStringList selected = controller.selectedPrograms();

    TEST_EXPECT_EQ(selected.size(), 1);
    TEST_EXPECT_EQ(selected[0], QString("83101"));
}

void saveRelaxedHardConstraints(AppController& controller) {
    controller.saveHardConstraints(
        false, 1,
        false, 1,
        false, 1,
        false, 1,
        false, 1
    );
}

void saveEnabledButSafeHardConstraints(AppController& controller) {
    controller.saveHardConstraints(
        true,  1,
        true,  1,
        false, 1,
        false, 1,
        false, 1
    );
}

int generateAndReturnScheduleCount(AppController& controller) {
    controller.clearMessages();

    controller.generateSchedules();

    GenerationWaitResult waitResult = waitForGeneration(controller);

    TEST_EXPECT_FALSE(waitResult.timedOut);
    TEST_EXPECT_TRUE(waitResult.completed);
    TEST_EXPECT_FALSE(waitResult.failed);

    TEST_EXPECT_TRUE(controller.errorMessage().isEmpty());
    TEST_EXPECT_TRUE(controller.statusMessage().contains("השיבוץ הושלם"));

    return controller.outputManager()->getTotalSchedulesCount();
}

void testSettingsRoundTripAfterDataLoad() {
    AppController controller;

    loadSimpleValidData(controller);

    controller.saveHardConstraints(
        true,  2,
        false, 3,
        true,  4,
        false, 5,
        true,  6
    );

    QVariantMap hardConstraints = controller.getHardConstraints();

    TEST_EXPECT_EQ(hardConstraints.value("rule21Enabled").toBool(), true);
    TEST_EXPECT_EQ(hardConstraints.value("rule21K").toInt(), 2);

    TEST_EXPECT_EQ(hardConstraints.value("rule22Enabled").toBool(), false);
    TEST_EXPECT_EQ(hardConstraints.value("rule22K").toInt(), 3);

    TEST_EXPECT_EQ(hardConstraints.value("rule23Enabled").toBool(), true);
    TEST_EXPECT_EQ(hardConstraints.value("rule23K").toInt(), 4);

    TEST_EXPECT_EQ(hardConstraints.value("rule24Enabled").toBool(), false);
    TEST_EXPECT_EQ(hardConstraints.value("rule24K").toInt(), 5);

    TEST_EXPECT_EQ(hardConstraints.value("rule25Enabled").toBool(), true);
    TEST_EXPECT_EQ(hardConstraints.value("rule25K").toInt(), 6);

    QVariantList priorities;
    priorities.append("2.2");
    priorities.append("2.1");
    priorities.append("2.5");
    priorities.append("2.3");
    priorities.append("2.4");

    controller.saveSortingPriorities(priorities);

    QVariantList storedPriorities = controller.getSortingPriorities();

    TEST_EXPECT_EQ(storedPriorities.size(), 5);
    TEST_EXPECT_EQ(storedPriorities[0].toString(), QString("2.2"));
    TEST_EXPECT_EQ(storedPriorities[1].toString(), QString("2.1"));
    TEST_EXPECT_EQ(storedPriorities[2].toString(), QString("2.5"));
    TEST_EXPECT_EQ(storedPriorities[3].toString(), QString("2.3"));
    TEST_EXPECT_EQ(storedPriorities[4].toString(), QString("2.4"));
}

void testGenerateAfterApplyingSettings() {
    AppController controller;

    loadSimpleValidData(controller);
    selectSimpleProgram(controller);

    saveRelaxedHardConstraints(controller);

    int scheduleCount = generateAndReturnScheduleCount(controller);

    TEST_EXPECT_TRUE(scheduleCount > 0);

    TEST_EXPECT_TRUE(!controller.outputManager()->getAvailableSemesters().isEmpty());
    TEST_EXPECT_TRUE(!controller.outputManager()->getAvailableMoeds().isEmpty());
    TEST_EXPECT_TRUE(!controller.outputManager()->getCurrentCalendarData().isEmpty());
}

void testRegenerateAfterChangingHardConstraintSettings() {
    AppController controller;

    loadSimpleValidData(controller);
    selectSimpleProgram(controller);

    saveRelaxedHardConstraints(controller);

    int firstScheduleCount = generateAndReturnScheduleCount(controller);

    TEST_EXPECT_TRUE(firstScheduleCount > 0);

    saveEnabledButSafeHardConstraints(controller);

    QVariantMap updatedSettings = controller.getHardConstraints();

    TEST_EXPECT_EQ(updatedSettings.value("rule21Enabled").toBool(), true);
    TEST_EXPECT_EQ(updatedSettings.value("rule21K").toInt(), 1);

    TEST_EXPECT_EQ(updatedSettings.value("rule22Enabled").toBool(), true);
    TEST_EXPECT_EQ(updatedSettings.value("rule22K").toInt(), 1);

    int secondScheduleCount = generateAndReturnScheduleCount(controller);

    TEST_EXPECT_TRUE(secondScheduleCount > 0);
}

void testSortingPrioritiesCanBeAppliedAfterGeneration() {
    AppController controller;

    loadSimpleValidData(controller);
    selectSimpleProgram(controller);

    saveRelaxedHardConstraints(controller);

    int scheduleCount = generateAndReturnScheduleCount(controller);

    TEST_EXPECT_TRUE(scheduleCount > 0);

    QVariantList priorities;
    priorities.append("2.5");
    priorities.append("2.4");
    priorities.append("2.3");
    priorities.append("2.2");
    priorities.append("2.1");

    controller.saveSortingPriorities(priorities);

    QVariantList storedPriorities = controller.getSortingPriorities();

    TEST_EXPECT_EQ(storedPriorities.size(), 5);
    TEST_EXPECT_EQ(storedPriorities[0].toString(), QString("2.5"));
    TEST_EXPECT_EQ(storedPriorities[1].toString(), QString("2.4"));
    TEST_EXPECT_EQ(storedPriorities[2].toString(), QString("2.3"));
    TEST_EXPECT_EQ(storedPriorities[3].toString(), QString("2.2"));
    TEST_EXPECT_EQ(storedPriorities[4].toString(), QString("2.1"));

    TEST_EXPECT_EQ(controller.outputManager()->getTotalSchedulesCount(), scheduleCount);
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    qRegisterMetaType<std::vector<ScheduleGenerationResult>>(
        "std::vector<ScheduleGenerationResult>"
    );

    testSettingsRoundTripAfterDataLoad();
    testGenerateAfterApplyingSettings();
    testRegenerateAfterChangingHardConstraintSettings();
    testSortingPrioritiesCanBeAppliedAfterGeneration();

    std::cout << "SettingsWorkflowTest passed." << std::endl;
    return EXIT_SUCCESS;
}