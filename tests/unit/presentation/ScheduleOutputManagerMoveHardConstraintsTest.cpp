#include "presentation/ScheduleOutputManager.h"
#include "TestMacros.h"

#include <QCoreApplication>
#include <QVariantMap>

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
    Course course(name, number, "Dr. Constraints", Evaluation::EXAM);

    course.addProgram(
        "83108",
        Year::FIRST,
        Semester::FALL,
        requirement
    );

    return course;
}

ExamPeriod makePeriod() {
    return ExamPeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(9, 1, 2026),
        {}
    );
}

ScheduleGenerationResult makeResult(
    const Course& firstCourse,
    const Date& firstDate,
    const Course& secondCourse,
    const Date& secondDate
) {
    return ScheduleGenerationResult({
        ExamAssignment{&firstCourse, firstDate, true},
        ExamAssignment{&secondCourse, secondDate, true}
    });
}

void configureManager(
    ScheduleOutputManager& manager,
    std::vector<Course>& courses
) {
    ScheduleGenerationResult result = makeResult(
        courses[0],
        Date(1, 1, 2026),
        courses[1],
        Date(4, 1, 2026)
    );

    manager.setAvailablePeriods({"FALL"}, {"Aleph"});
    manager.setPeriodFilter("FALL", "Aleph");
    manager.setSchedulingData(
        { result },
        courses,
        { makePeriod() }
    );
}

Date assignedDate(
    const ScheduleOutputManager& manager,
    const std::string& courseId
) {
    const std::vector<ScheduleGenerationResult>& solutions = manager.getSolutions();

    TEST_EXPECT_EQ(solutions.size(), static_cast<std::size_t>(1));

    for (const ExamAssignment& assignment : solutions[0].getAssignments()) {
        if (assignment.course &&
            assignment.course->getCourseNumber() == courseId) {
            return assignment.examDate;
        }
    }

    TEST_FAIL("Assignment not found for course " << courseId);
    return Date(1, 1, 1970);
}

bool scheduleContainsDateForCourse(
    const ScheduleGenerationResult& schedule,
    const std::string& courseId,
    const Date& expectedDate
) {
    for (const ExamAssignment& assignment : schedule.getAssignments()) {
        if (assignment.course &&
            assignment.course->getCourseNumber() == courseId &&
            assignment.examDate == expectedDate) {
            return true;
        }
    }

    return false;
}

void testMoveRejectedByHardConstraintDoesNotMutateSchedule() {
    std::vector<Course> courses;
    courses.push_back(makeCourse("Algorithms", "89101", Requirement::OBLIGATORY));
    courses.push_back(makeCourse("Databases", "89102", Requirement::OBLIGATORY));

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    bool validatorWasCalled = false;

    manager.setMoveValidator(
        [&](const ScheduleGenerationResult& proposedSchedule,
            const std::string& movedCourseId,
            const Date& targetDate) {
            validatorWasCalled = true;

            TEST_EXPECT_EQ(movedCourseId, std::string("89101"));
            TEST_EXPECT_EQ(targetDate, Date(2, 1, 2026));

            TEST_EXPECT_TRUE(
                scheduleContainsDateForCourse(
                    proposedSchedule,
                    "89101",
                    Date(2, 1, 2026)
                )
            );

            return false;
        }
    );

    QVariantMap response = manager.requestMove("89101", "02-01-2026");

    TEST_EXPECT_TRUE(validatorWasCalled);
    TEST_EXPECT_EQ(response.value("status").toInt(), 0);
    TEST_EXPECT_EQ(
        response.value("error").toString(),
        QString("Conflict detected. Move violates scheduling rules.")
    );

    TEST_EXPECT_EQ(assignedDate(manager, "89101"), Date(1, 1, 2026));
    TEST_EXPECT_EQ(assignedDate(manager, "89102"), Date(4, 1, 2026));
}

void testMoveAcceptedWhenHardConstraintValidatorApproves() {
    std::vector<Course> courses;
    courses.push_back(makeCourse("Algorithms", "89101", Requirement::OBLIGATORY));
    courses.push_back(makeCourse("Databases", "89102", Requirement::OBLIGATORY));

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    bool validatorWasCalled = false;

    manager.setMoveValidator(
        [&](const ScheduleGenerationResult& proposedSchedule,
            const std::string& movedCourseId,
            const Date& targetDate) {
            validatorWasCalled = true;

            TEST_EXPECT_EQ(movedCourseId, std::string("89101"));
            TEST_EXPECT_EQ(targetDate, Date(2, 1, 2026));

            return scheduleContainsDateForCourse(
                proposedSchedule,
                "89101",
                Date(2, 1, 2026)
            );
        }
    );

    QVariantMap response = manager.requestMove("89101", "02-01-2026");

    TEST_EXPECT_TRUE(validatorWasCalled);
    TEST_EXPECT_EQ(response.value("status").toInt(), 1);

    TEST_EXPECT_EQ(assignedDate(manager, "89101"), Date(2, 1, 2026));
    TEST_EXPECT_EQ(assignedDate(manager, "89102"), Date(4, 1, 2026));
}

void testValidDatesMapUsesHardConstraintValidator() {
    std::vector<Course> courses;
    courses.push_back(makeCourse("Algorithms", "89101", Requirement::OBLIGATORY));
    courses.push_back(makeCourse("Databases", "89102", Requirement::OBLIGATORY));

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    manager.setMoveValidator(
        [](const ScheduleGenerationResult&,
           const std::string&,
           const Date& targetDate) {
            return targetDate != Date(2, 1, 2026);
        }
    );

    QVariantMap validDates = manager.getValidDatesForCourse("89101");

    TEST_EXPECT_TRUE(validDates.contains("01-01-2026"));
    TEST_EXPECT_TRUE(validDates.contains("02-01-2026"));
    TEST_EXPECT_TRUE(validDates.contains("04-01-2026"));

    TEST_EXPECT_EQ(validDates.value("01-01-2026").toBool(), true);
    TEST_EXPECT_EQ(validDates.value("02-01-2026").toBool(), false);
    TEST_EXPECT_EQ(validDates.value("04-01-2026").toBool(), true);
}

void testRejectedMoveDoesNotRefreshMetricsWithInvalidState() {
    std::vector<Course> courses;
    courses.push_back(makeCourse("Algorithms", "89101", Requirement::OBLIGATORY));
    courses.push_back(makeCourse("Databases", "89102", Requirement::OBLIGATORY));

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    manager.setMoveValidator(
        [](const ScheduleGenerationResult&,
           const std::string&,
           const Date&) {
            return false;
        }
    );

    manager.setMetricsUpdater(
        [](const ScheduleGenerationResult&) {
            ScheduleMetrics metrics;
            metrics.maxExamsInSingleDay = 99;
            return metrics;
        }
    );

    QVariantMap response = manager.requestMove("89101", "02-01-2026");

    TEST_EXPECT_EQ(response.value("status").toInt(), 0);
    TEST_EXPECT_EQ(
        manager.getCurrentMetrics().value("maxExamsInSingleDay").toInt(),
        0
    );

    TEST_EXPECT_EQ(assignedDate(manager, "89101"), Date(1, 1, 2026));
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    testMoveRejectedByHardConstraintDoesNotMutateSchedule();
    testMoveAcceptedWhenHardConstraintValidatorApproves();
    testValidDatesMapUsesHardConstraintValidator();
    testRejectedMoveDoesNotRefreshMetricsWithInvalidState();

    std::cout << "ScheduleOutputManagerMoveHardConstraintsTest passed." << std::endl;
    return EXIT_SUCCESS;
}