#include "presentation/ScheduleOutputManager.h"
#include "scheduling/MetricsCalculator.h"
#include "TestMacros.h"

#include <QCoreApplication>
#include <QVariantList>
#include <QVariantMap>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

Course makeCourse(
    const std::string& name,
    const std::string& number,
    Requirement requirement
) {
    Course course(name, number, "Dr. Metrics", Evaluation::EXAM);

    course.addProgram(
        "83108",
        Year::FIRST,
        Semester::FALL,
        requirement
    );

    return course;
}

RuntimeCourse makeRuntimeCourse(
    int id,
    const Course& course,
    int groupId,
    Requirement requirement
) {
    RuntimeCourse runtimeCourse;
    runtimeCourse.id = id;
    runtimeCourse.course = &course;
    runtimeCourse.memberships = {
        CourseMembership{groupId, requirement}
    };

    return runtimeCourse;
}

SchedulingBlock makeBlock(
    const std::vector<RuntimeCourse>& runtimeCourses
) {
    SchedulingBlock block;

    block.semester = "FALL";
    block.moed = "Aleph";
    block.allowedDates = {
        Date(1, 1, 2026),
        Date(2, 1, 2026),
        Date(4, 1, 2026),
        Date(5, 1, 2026),
        Date(6, 1, 2026),
        Date(7, 1, 2026),
        Date(8, 1, 2026),
        Date(9, 1, 2026)
    };
    block.runtimeCourses = runtimeCourses;

    return block;
}

std::vector<ExamPeriod> makePeriods() {
    return {
        ExamPeriod(
            Semester::FALL,
            Moed::ALEPH,
            Date(1, 1, 2026),
            Date(10, 1, 2026),
            {}
        )
    };
}

ScheduleGenerationResult makeResultWithCalculatedMetrics(
    const std::vector<ExamAssignment>& assignments,
    const SchedulingBlock& block
) {
    ScheduleGenerationResult result(assignments);

    result.metrics = MetricsCalculator::calculate(result, block);

    return result;
}

void prepareManager(
    ScheduleOutputManager& manager,
    const std::vector<ScheduleGenerationResult>& solutions,
    const std::vector<Course>& courses
) {
    manager.setAvailablePeriods({"FALL"}, {"Aleph"});
    manager.setPeriodFilter("FALL", "Aleph");
    manager.setSchedulingData(solutions, courses, makePeriods());
}

double currentMetricDouble(
    const ScheduleOutputManager& manager,
    const QString& key
) {
    return manager.getCurrentMetrics().value(key).toDouble();
}

int currentMetricInt(
    const ScheduleOutputManager& manager,
    const QString& key
) {
    return manager.getCurrentMetrics().value(key).toInt();
}

bool hasAnyDisplayedExam(const ScheduleOutputManager& manager) {
    const QVariantList calendarData = manager.getCurrentCalendarData();

    for (const QVariant& item : calendarData) {
        const QVariantMap day = item.toMap();
        const QVariantList exams = day.value("exams").toList();

        if (!exams.isEmpty()) {
            return true;
        }
    }

    return false;
}

void expectDoubleNear(
    double actual,
    double expected
) {
    TEST_EXPECT_TRUE(std::abs(actual - expected) < 0.0001);
}

void testMetricsCalculatedBeforeSortingAndBestObligatoryAverageWins() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Mandatory A", "89101", Requirement::OBLIGATORY));
    courses.push_back(makeCourse("Mandatory B", "89102", Requirement::OBLIGATORY));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 0, Requirement::OBLIGATORY)
    };

    SchedulingBlock block = makeBlock(runtimeCourses);

    ScheduleGenerationResult compactSchedule =
        makeResultWithCalculatedMetrics(
            {
                ExamAssignment{&courses[0], Date(1, 1, 2026), true},
                ExamAssignment{&courses[1], Date(2, 1, 2026), true}
            },
            block
        );

    ScheduleGenerationResult spreadSchedule =
        makeResultWithCalculatedMetrics(
            {
                ExamAssignment{&courses[0], Date(1, 1, 2026), true},
                ExamAssignment{&courses[1], Date(8, 1, 2026), true}
            },
            block
        );

    TEST_EXPECT_TRUE(
        spreadSchedule.metrics.avgDaysBetweenObligatory >
        compactSchedule.metrics.avgDaysBetweenObligatory
    );

    ScheduleOutputManager manager;

    prepareManager(
        manager,
        { compactSchedule, spreadSchedule },
        courses
    );

    manager.sortSchedules({ "metric_avgDaysObligatory" });

    TEST_EXPECT_EQ(manager.getTotalSchedulesCount(), 2);
    TEST_EXPECT_EQ(manager.getCurrentScheduleIndex(), 1);

    expectDoubleNear(
        currentMetricDouble(manager, "avgDaysBetweenObligatory"),
        7.0
    );

    TEST_EXPECT_TRUE(hasAnyDisplayedExam(manager));
}

void testMetricsSortingPrefersFewerElectiveConflicts() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Elective A", "89201", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective B", "89202", Requirement::ELECTIVE));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(1, courses[1], 0, Requirement::ELECTIVE)
    };

    SchedulingBlock block = makeBlock(runtimeCourses);

    ScheduleGenerationResult conflictSchedule =
        makeResultWithCalculatedMetrics(
            {
                ExamAssignment{&courses[0], Date(4, 1, 2026), false},
                ExamAssignment{&courses[1], Date(4, 1, 2026), false}
            },
            block
        );

    ScheduleGenerationResult cleanSchedule =
        makeResultWithCalculatedMetrics(
            {
                ExamAssignment{&courses[0], Date(4, 1, 2026), false},
                ExamAssignment{&courses[1], Date(5, 1, 2026), false}
            },
            block
        );

    TEST_EXPECT_TRUE(
        cleanSchedule.metrics.totalElectiveConflicts <
        conflictSchedule.metrics.totalElectiveConflicts
    );

    ScheduleOutputManager manager;

    prepareManager(
        manager,
        { conflictSchedule, cleanSchedule },
        courses
    );

    manager.sortSchedules({ "metric_electiveConflicts" });

    TEST_EXPECT_EQ(manager.getTotalSchedulesCount(), 2);
    TEST_EXPECT_EQ(manager.getCurrentScheduleIndex(), 1);

    TEST_EXPECT_EQ(
        currentMetricInt(manager, "totalElectiveConflicts"),
        0
    );

    TEST_EXPECT_EQ(
        currentMetricInt(manager, "maxExamsInSingleDay"),
        1
    );

    TEST_EXPECT_TRUE(hasAnyDisplayedExam(manager));
}

void testPriorityOrderUsesSecondMetricWhenFirstMetricTies() {
    std::vector<Course> courses;
    courses.reserve(4);

    courses.push_back(makeCourse("Mandatory A", "89301", Requirement::OBLIGATORY));
    courses.push_back(makeCourse("Mandatory B", "89302", Requirement::OBLIGATORY));
    courses.push_back(makeCourse("Elective A", "89303", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective B", "89304", Requirement::ELECTIVE));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(2, courses[2], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(3, courses[3], 0, Requirement::ELECTIVE)
    };

    SchedulingBlock block = makeBlock(runtimeCourses);

    ScheduleGenerationResult sameFirstMetricButWorseSecond =
        makeResultWithCalculatedMetrics(
            {
                ExamAssignment{&courses[0], Date(1, 1, 2026), true},
                ExamAssignment{&courses[1], Date(6, 1, 2026), true},
                ExamAssignment{&courses[2], Date(4, 1, 2026), false},
                ExamAssignment{&courses[3], Date(4, 1, 2026), false}
            },
            block
        );

    ScheduleGenerationResult sameFirstMetricButBetterSecond =
        makeResultWithCalculatedMetrics(
            {
                ExamAssignment{&courses[0], Date(1, 1, 2026), true},
                ExamAssignment{&courses[1], Date(6, 1, 2026), true},
                ExamAssignment{&courses[2], Date(4, 1, 2026), false},
                ExamAssignment{&courses[3], Date(5, 1, 2026), false}
            },
            block
        );

    expectDoubleNear(
        sameFirstMetricButWorseSecond.metrics.avgDaysBetweenObligatory,
        sameFirstMetricButBetterSecond.metrics.avgDaysBetweenObligatory
    );

    TEST_EXPECT_TRUE(
        sameFirstMetricButBetterSecond.metrics.totalElectiveConflicts <
        sameFirstMetricButWorseSecond.metrics.totalElectiveConflicts
    );

    ScheduleOutputManager manager;

    prepareManager(
        manager,
        {
            sameFirstMetricButWorseSecond,
            sameFirstMetricButBetterSecond
        },
        courses
    );

    manager.sortSchedules({
        "metric_avgDaysObligatory",
        "metric_electiveConflicts"
    });

    TEST_EXPECT_EQ(manager.getTotalSchedulesCount(), 2);
    TEST_EXPECT_EQ(manager.getCurrentScheduleIndex(), 1);

    expectDoubleNear(
        currentMetricDouble(manager, "avgDaysBetweenObligatory"),
        5.0
    );

    TEST_EXPECT_EQ(
        currentMetricInt(manager, "totalElectiveConflicts"),
        0
    );

    TEST_EXPECT_TRUE(hasAnyDisplayedExam(manager));
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    testMetricsCalculatedBeforeSortingAndBestObligatoryAverageWins();
    testMetricsSortingPrefersFewerElectiveConflicts();
    testPriorityOrderUsesSecondMetricWhenFirstMetricTies();

    std::cout << "MetricsSortingIntegrationTest passed." << std::endl;
    return EXIT_SUCCESS;
}