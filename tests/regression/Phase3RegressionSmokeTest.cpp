#include "scheduling/ScheduleGenerator.h"
#include "presentation/ScheduleOutputManager.h"

#include <QCoreApplication>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace {

Course makeCourse(const std::string& name,
                  const std::string& number,
                  Requirement domainRequirement = Requirement::OBLIGATORY) {
    Course course(name, number, "Dr. Regression", Evaluation::EXAM);
    course.addProgram("83108", Year::FIRST, Semester::FALL, domainRequirement);
    return course;
}

RuntimeCourse makeRuntimeCourse(int id,
                                const Course& course,
                                int groupId,
                                Requirement requirement) {
    RuntimeCourse runtimeCourse;
    runtimeCourse.id = id;
    runtimeCourse.course = &course;
    runtimeCourse.memberships = {
        {groupId, requirement}
    };
    return runtimeCourse;
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

QString firstDisplayedCourseId(const ScheduleOutputManager& manager) {
    const QVariantList calendarData = manager.getCurrentCalendarData();

    for (const QVariant& item : calendarData) {
        const QVariantMap day = item.toMap();

        const QVariantList exams = day.value("exams").toList();

        if (!exams.isEmpty()) {
            const QVariantMap firstExam = exams.first().toMap();
            return firstExam.value("courseId").toString();
        }
    }

    return "";
}

void testPhase3SmokeFlowGenerateSortAndDisplay() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Mandatory A", "89101"));
    courses.push_back(makeCourse("Mandatory B", "89102"));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 0, Requirement::OBLIGATORY)
    };

    SchedulingBlock block;
    block.semester = "FALL";
    block.moed = "Aleph";
    block.allowedDates = {
        Date(1, 1, 2026),
        Date(3, 1, 2026),
        Date(6, 1, 2026)
    };
    block.runtimeCourses = runtimeCourses;

    ScheduleSettings settings;
    settings.minDaysObligatory.isActive = true;
    settings.minDaysObligatory.k = 2;
    settings.maxExamsPerDay.isActive = true;
    settings.maxExamsPerDay.k = 1;

    ScheduleGenerator generator(block, settings, 5.0);
    std::vector<ScheduleGenerationResult> solutions = generator.runBacktracking(20);

    assert(!solutions.empty());

    ScheduleOutputManager manager;
    manager.setAvailablePeriods({"FALL"}, {"Aleph"});
    manager.setPeriodFilter("FALL", "Aleph");
    manager.setSchedulingData(solutions, courses, makePeriods());

    assert(manager.getTotalSchedulesCount() > 0);
    assert(manager.getCurrentScheduleIndex() == 1);

    /*
     * Sorting should not crash and should keep a valid first schedule visible.
     */
    manager.sortSchedules({
        "metric_avgDaysObligatory",
        "metric_maxExamsPerDay"
    });

    assert(manager.getCurrentScheduleIndex() == 1);
    assert(!firstDisplayedCourseId(manager).isEmpty());
}

} // namespace

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    testPhase3SmokeFlowGenerateSortAndDisplay();

    std::cout << "Phase3RegressionSmokeTest passed." << std::endl;
    return 0;
}