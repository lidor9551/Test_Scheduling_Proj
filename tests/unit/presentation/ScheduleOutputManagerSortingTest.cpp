#include "presentation/ScheduleOutputManager.h"

#include <QCoreApplication>
#include <QVariantList>
#include <QVariantMap>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace {

Course makeCourse(const std::string& name,
                  const std::string& number,
                  Requirement requirement = Requirement::OBLIGATORY) {
    Course course(name, number, "Dr. Sorting", Evaluation::EXAM);
    course.addProgram("83108", Year::FIRST, Semester::FALL, requirement);
    return course;
}

ScheduleGenerationResult makeResult(const Course& course,
                                    const Date& date,
                                    const ScheduleMetrics& metrics) {
    ScheduleGenerationResult result({
        {&course, date, true}
    });

    result.metrics = metrics;
    return result;
}

QString firstDisplayedCourseId(const ScheduleOutputManager& manager) {
    const QVariantList calendarData = manager.getCurrentCalendarData();

    for (const QVariant& item : calendarData) {
        const QVariantMap day = item.toMap();

        if (day.value("hasExam").toBool()) {
            return day.value("courseId").toString();
        }
    }

    return "";
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

void prepareManager(ScheduleOutputManager& manager,
                    const std::vector<ScheduleGenerationResult>& solutions,
                    const std::vector<Course>& courses) {
    manager.setAvailablePeriods({"FALL"}, {"Aleph"});
    manager.setPeriodFilter("FALL", "Aleph");
    manager.setSchedulingData(solutions, courses, makePeriods());
}

void testSortByAvgDaysObligatoryDescending() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Low Average", "89101"));
    courses.push_back(makeCourse("High Average", "89102"));

    ScheduleMetrics lowMetrics;
    lowMetrics.avgDaysBetweenObligatory = 2.0;

    ScheduleMetrics highMetrics;
    highMetrics.avgDaysBetweenObligatory = 8.0;

    std::vector<ScheduleGenerationResult> solutions = {
        makeResult(courses[0], Date(2, 1, 2026), lowMetrics),
        makeResult(courses[1], Date(4, 1, 2026), highMetrics)
    };

    ScheduleOutputManager manager;
    prepareManager(manager, solutions, courses);

    manager.sortSchedules({"metric_avgDaysObligatory"});

    assert(firstDisplayedCourseId(manager) == "89102");
    assert(manager.getCurrentScheduleIndex() == 1);
}

void testSortByAvgDaysAllDescending() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Low All Average", "89101"));
    courses.push_back(makeCourse("High All Average", "89102"));

    ScheduleMetrics lowMetrics;
    lowMetrics.avgDaysBetweenAll = 1.5;

    ScheduleMetrics highMetrics;
    highMetrics.avgDaysBetweenAll = 6.5;

    std::vector<ScheduleGenerationResult> solutions = {
        makeResult(courses[0], Date(2, 1, 2026), lowMetrics),
        makeResult(courses[1], Date(4, 1, 2026), highMetrics)
    };

    ScheduleOutputManager manager;
    prepareManager(manager, solutions, courses);

    manager.sortSchedules({"metric_avgDaysAll"});

    assert(firstDisplayedCourseId(manager) == "89102");
}

void testSortByElectiveConflictsAscending() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Many Conflicts", "89101"));
    courses.push_back(makeCourse("Few Conflicts", "89102"));

    ScheduleMetrics manyConflicts;
    manyConflicts.totalElectiveConflicts = 5;

    ScheduleMetrics fewConflicts;
    fewConflicts.totalElectiveConflicts = 1;

    std::vector<ScheduleGenerationResult> solutions = {
        makeResult(courses[0], Date(2, 1, 2026), manyConflicts),
        makeResult(courses[1], Date(4, 1, 2026), fewConflicts)
    };

    ScheduleOutputManager manager;
    prepareManager(manager, solutions, courses);

    manager.sortSchedules({"metric_electiveConflicts"});

    assert(firstDisplayedCourseId(manager) == "89102");
}

void testSortByObligatorySpanDescending() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Small Span", "89101"));
    courses.push_back(makeCourse("Large Span", "89102"));

    ScheduleMetrics smallSpan;
    smallSpan.obligatorySpan = 2;

    ScheduleMetrics largeSpan;
    largeSpan.obligatorySpan = 9;

    std::vector<ScheduleGenerationResult> solutions = {
        makeResult(courses[0], Date(2, 1, 2026), smallSpan),
        makeResult(courses[1], Date(4, 1, 2026), largeSpan)
    };

    ScheduleOutputManager manager;
    prepareManager(manager, solutions, courses);

    manager.sortSchedules({"metric_obligatorySpan"});

    assert(firstDisplayedCourseId(manager) == "89102");
}

void testSortByMaxExamsPerDayAscending() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Heavy Day", "89101"));
    courses.push_back(makeCourse("Light Day", "89102"));

    ScheduleMetrics heavyDay;
    heavyDay.maxExamsInSingleDay = 4;

    ScheduleMetrics lightDay;
    lightDay.maxExamsInSingleDay = 1;

    std::vector<ScheduleGenerationResult> solutions = {
        makeResult(courses[0], Date(2, 1, 2026), heavyDay),
        makeResult(courses[1], Date(4, 1, 2026), lightDay)
    };

    ScheduleOutputManager manager;
    prepareManager(manager, solutions, courses);

    manager.sortSchedules({"metric_maxExamsPerDay"});

    assert(firstDisplayedCourseId(manager) == "89102");
}

void testSortUsesSecondPriorityWhenFirstMetricTies() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Tie But Worse Conflicts", "89101"));
    courses.push_back(makeCourse("Tie But Better Conflicts", "89102"));

    ScheduleMetrics worse;
    worse.avgDaysBetweenObligatory = 5.0;
    worse.totalElectiveConflicts = 4;

    ScheduleMetrics better;
    better.avgDaysBetweenObligatory = 5.0;
    better.totalElectiveConflicts = 0;

    std::vector<ScheduleGenerationResult> solutions = {
        makeResult(courses[0], Date(2, 1, 2026), worse),
        makeResult(courses[1], Date(4, 1, 2026), better)
    };

    ScheduleOutputManager manager;
    prepareManager(manager, solutions, courses);

    manager.sortSchedules({
        "metric_avgDaysObligatory",
        "metric_electiveConflicts"
    });

    assert(firstDisplayedCourseId(manager) == "89102");
}

void testEmptyPriorityListDoesNotCrash() {
    std::vector<Course> courses;
    courses.reserve(1);

    courses.push_back(makeCourse("Only Schedule", "89101"));

    ScheduleMetrics metrics;
    metrics.avgDaysBetweenObligatory = 10.0;

    std::vector<ScheduleGenerationResult> solutions = {
        makeResult(courses[0], Date(2, 1, 2026), metrics)
    };

    ScheduleOutputManager manager;
    prepareManager(manager, solutions, courses);

    manager.sortSchedules({});

    assert(firstDisplayedCourseId(manager) == "89101");
}

} // namespace

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    testSortByAvgDaysObligatoryDescending();
    testSortByAvgDaysAllDescending();
    testSortByElectiveConflictsAscending();
    testSortByObligatorySpanDescending();
    testSortByMaxExamsPerDayAscending();
    testSortUsesSecondPriorityWhenFirstMetricTies();
    testEmptyPriorityListDoesNotCrash();

    std::cout << "ScheduleOutputManagerSortingTest passed." << std::endl;
    return 0;
}