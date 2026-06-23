#include "presentation/AppController.h"
#include "TestMacros.h"

#include <QCoreApplication>
#include <QDir>
#include <QStringList>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>

#include <cstdlib>
#include <iostream>

namespace {

bool containsString(const QStringList& values, const QString& expected) {
    for (const QString& value : values) {
        if (value == expected) {
            return true;
        }
    }

    return false;
}

void testDefaultState() {
    AppController controller;

    TEST_EXPECT_TRUE(controller.coursesFilePath().isEmpty());
    TEST_EXPECT_TRUE(controller.examPeriodsFilePath().isEmpty());

    TEST_EXPECT_TRUE(controller.statusMessage().isEmpty());
    TEST_EXPECT_TRUE(controller.errorMessage().isEmpty());

    TEST_EXPECT_EQ(controller.courseCount(), 0);
    TEST_EXPECT_EQ(controller.examPeriodCount(), 0);
    TEST_EXPECT_FALSE(controller.hasData());

    TEST_EXPECT_TRUE(controller.getCoursesVariant().isEmpty());
    TEST_EXPECT_TRUE(controller.getExamPeriodsVariant().isEmpty());

    TEST_EXPECT_TRUE(controller.programCourseModel() != nullptr);
    TEST_EXPECT_TRUE(controller.outputManager() != nullptr);

    TEST_EXPECT_EQ(controller.getExamPeriodDays(), 0);
}

void testAvailablePrograms() {
    AppController controller;

    const QStringList programs = controller.availablePrograms();

    TEST_EXPECT_EQ(programs.size(), 10);
    TEST_EXPECT_TRUE(containsString(programs, "83101"));
    TEST_EXPECT_TRUE(containsString(programs, "83102"));
    TEST_EXPECT_TRUE(containsString(programs, "83103"));
    TEST_EXPECT_TRUE(containsString(programs, "83104"));
    TEST_EXPECT_TRUE(containsString(programs, "83105"));
    TEST_EXPECT_TRUE(containsString(programs, "83107"));
    TEST_EXPECT_TRUE(containsString(programs, "83108"));
    TEST_EXPECT_TRUE(containsString(programs, "83109"));
    TEST_EXPECT_TRUE(containsString(programs, "83115"));
    TEST_EXPECT_TRUE(containsString(programs, "83182"));
}

void testFilePathSettersNormalizePlainPaths() {
    AppController controller;

    controller.setCoursesFilePath("relative/courses.txt");
    controller.setExamPeriodsFilePath("relative/periods.txt");

    TEST_EXPECT_EQ(
        controller.coursesFilePath(),
        QDir::toNativeSeparators("relative/courses.txt")
    );

    TEST_EXPECT_EQ(
        controller.examPeriodsFilePath(),
        QDir::toNativeSeparators("relative/periods.txt")
    );
}

void testFilePathSettersNormalizeLocalFileUrls() {
    AppController controller;

    const QString coursesLocalPath =
        QDir::currentPath() + "/courses.txt";

    const QString periodsLocalPath =
        QDir::currentPath() + "/periods.txt";

    controller.setCoursesFilePath(QUrl::fromLocalFile(coursesLocalPath).toString());
    controller.setExamPeriodsFilePath(QUrl::fromLocalFile(periodsLocalPath).toString());

    TEST_EXPECT_EQ(
        controller.coursesFilePath(),
        QDir::toNativeSeparators(coursesLocalPath)
    );

    TEST_EXPECT_EQ(
        controller.examPeriodsFilePath(),
        QDir::toNativeSeparators(periodsLocalPath)
    );
}

void testReplaceDataWithMissingFilesSetsError() {
    AppController controller;

    controller.setCoursesFilePath("missing_courses_file.txt");
    controller.setExamPeriodsFilePath("missing_periods_file.txt");

    controller.replaceData();

    TEST_EXPECT_FALSE(controller.errorMessage().isEmpty());
    TEST_EXPECT_TRUE(controller.statusMessage().isEmpty());
    TEST_EXPECT_FALSE(controller.hasData());
    TEST_EXPECT_EQ(controller.courseCount(), 0);
    TEST_EXPECT_EQ(controller.examPeriodCount(), 0);
}

void testClearMessagesClearsErrorMessage() {
    AppController controller;

    controller.onSchedulingFailed("unit test failure");

    TEST_EXPECT_FALSE(controller.errorMessage().isEmpty());

    controller.clearMessages();

    TEST_EXPECT_TRUE(controller.errorMessage().isEmpty());
    TEST_EXPECT_TRUE(controller.statusMessage().isEmpty());
}

void testProgramToggleSelectsAndDeselectsProgram() {
    AppController controller;

    TEST_EXPECT_TRUE(controller.selectedPrograms().isEmpty());

    controller.toggleProgram("83101");

    QStringList selected = controller.selectedPrograms();

    TEST_EXPECT_EQ(selected.size(), 1);
    TEST_EXPECT_TRUE(containsString(selected, "83101"));

    controller.toggleProgram("83101");

    TEST_EXPECT_TRUE(controller.selectedPrograms().isEmpty());
}

void testHardConstraintDefaults() {
    AppController controller;

    const QVariantMap map = controller.getHardConstraints();

    TEST_EXPECT_EQ(map.value("rule21Enabled").toBool(), false);
    TEST_EXPECT_EQ(map.value("rule21K").toInt(), 1);

    TEST_EXPECT_EQ(map.value("rule22Enabled").toBool(), false);
    TEST_EXPECT_EQ(map.value("rule22K").toInt(), 1);

    TEST_EXPECT_EQ(map.value("rule23Enabled").toBool(), false);
    TEST_EXPECT_EQ(map.value("rule23K").toInt(), 1);

    TEST_EXPECT_EQ(map.value("rule24Enabled").toBool(), false);
    TEST_EXPECT_EQ(map.value("rule24K").toInt(), 1);

    TEST_EXPECT_EQ(map.value("rule25Enabled").toBool(), false);
    TEST_EXPECT_EQ(map.value("rule25K").toInt(), 1);
}

void testSaveAndReadHardConstraints() {
    AppController controller;

    controller.saveHardConstraints(
        true,  2,
        false, 3,
        true,  4,
        false, 5,
        true,  6
    );

    const QVariantMap map = controller.getHardConstraints();

    TEST_EXPECT_EQ(map.value("rule21Enabled").toBool(), true);
    TEST_EXPECT_EQ(map.value("rule21K").toInt(), 2);

    TEST_EXPECT_EQ(map.value("rule22Enabled").toBool(), false);
    TEST_EXPECT_EQ(map.value("rule22K").toInt(), 3);

    TEST_EXPECT_EQ(map.value("rule23Enabled").toBool(), true);
    TEST_EXPECT_EQ(map.value("rule23K").toInt(), 4);

    TEST_EXPECT_EQ(map.value("rule24Enabled").toBool(), false);
    TEST_EXPECT_EQ(map.value("rule24K").toInt(), 5);

    TEST_EXPECT_EQ(map.value("rule25Enabled").toBool(), true);
    TEST_EXPECT_EQ(map.value("rule25K").toInt(), 6);
}

void testSortingPrioritiesDefaultEmpty() {
    AppController controller;

    TEST_EXPECT_TRUE(controller.getSortingPriorities().isEmpty());
}

void testSaveAndReadSortingPrioritiesPreservesOrder() {
    AppController controller;

    QVariantList priorities;
    priorities.append("2.4");
    priorities.append("2.1");
    priorities.append("2.5");

    controller.saveSortingPriorities(priorities);

    const QVariantList stored = controller.getSortingPriorities();

    TEST_EXPECT_EQ(stored.size(), 3);
    TEST_EXPECT_EQ(stored[0].toString(), QString("2.4"));
    TEST_EXPECT_EQ(stored[1].toString(), QString("2.1"));
    TEST_EXPECT_EQ(stored[2].toString(), QString("2.5"));
}

void testGetCoursesForProgramReturnsEmptyWithoutData() {
    AppController controller;

    QVariantList courses = controller.getCoursesForProgram("83101");

    TEST_EXPECT_TRUE(courses.isEmpty());
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    testDefaultState();
    testAvailablePrograms();

    testFilePathSettersNormalizePlainPaths();
    testFilePathSettersNormalizeLocalFileUrls();

    testReplaceDataWithMissingFilesSetsError();
    testClearMessagesClearsErrorMessage();

    testProgramToggleSelectsAndDeselectsProgram();

    testHardConstraintDefaults();
    testSaveAndReadHardConstraints();

    testSortingPrioritiesDefaultEmpty();
    testSaveAndReadSortingPrioritiesPreservesOrder();

    testGetCoursesForProgramReturnsEmptyWithoutData();

    std::cout << "AppControllerTest passed." << std::endl;
    return EXIT_SUCCESS;
}