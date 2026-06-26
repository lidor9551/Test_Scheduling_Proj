#include "presentation/ScheduleOutputManager.h"
#include "TestMacros.h"

#include <QCoreApplication>
#include <QVariantMap>

#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace {

Course makeCourse(
    const std::string& name,
    const std::string& number
) {
    Course course(name, number, "Dr. Integrity", Evaluation::EXAM);

    course.addProgram(
        "83108",
        Year::FIRST,
        Semester::FALL,
        Requirement::OBLIGATORY
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

ScheduleGenerationResult makeResult(std::vector<Course>& courses) {
    return ScheduleGenerationResult({
        ExamAssignment{&courses[0], Date(1, 1, 2026), true},
        ExamAssignment{&courses[1], Date(4, 1, 2026), true},
        ExamAssignment{&courses[2], Date(5, 1, 2026), true}
    });
}

void configureManager(
    ScheduleOutputManager& manager,
    std::vector<Course>& courses
) {
    ScheduleGenerationResult result = makeResult(courses);

    manager.setAvailablePeriods({"FALL"}, {"Aleph"});
    manager.setPeriodFilter("FALL", "Aleph");
    manager.setSchedulingData(
        { result },
        courses,
        { makePeriod() }
    );
}

const std::vector<ExamAssignment>& assignments(
    const ScheduleOutputManager& manager
) {
    const std::vector<ScheduleGenerationResult>& solutions = manager.getSolutions();

    TEST_EXPECT_EQ(solutions.size(), static_cast<std::size_t>(1));

    return solutions[0].getAssignments();
}

std::map<std::string, Date> assignmentDatesByCourseId(
    const ScheduleOutputManager& manager
) {
    std::map<std::string, Date> result;

    for (const ExamAssignment& assignment : assignments(manager)) {
        TEST_EXPECT_TRUE(assignment.course != nullptr);

        result[assignment.course->getCourseNumber()] = assignment.examDate;
    }

    return result;
}

std::map<std::string, const Course*> assignmentPointersByCourseId(
    const ScheduleOutputManager& manager
) {
    std::map<std::string, const Course*> result;

    for (const ExamAssignment& assignment : assignments(manager)) {
        TEST_EXPECT_TRUE(assignment.course != nullptr);

        result[assignment.course->getCourseNumber()] = assignment.course;
    }

    return result;
}

void expectNoNullOrDuplicateCoursePointers(
    const ScheduleOutputManager& manager
) {
    std::set<const Course*> seenPointers;
    std::set<std::string> seenCourseIds;

    for (const ExamAssignment& assignment : assignments(manager)) {
        TEST_EXPECT_TRUE(assignment.course != nullptr);

        const Course* coursePointer = assignment.course;
        const std::string courseId = coursePointer->getCourseNumber();

        TEST_EXPECT_TRUE(!courseId.empty());
        TEST_EXPECT_TRUE(seenPointers.insert(coursePointer).second);
        TEST_EXPECT_TRUE(seenCourseIds.insert(courseId).second);
    }
}

void expectAssignmentCount(
    const ScheduleOutputManager& manager,
    std::size_t expectedCount
) {
    TEST_EXPECT_EQ(assignments(manager).size(), expectedCount);
}

void testSuccessfulMovePreservesAssignmentCountAndCoursePointers() {
    std::vector<Course> courses;
    courses.reserve(3);

    courses.push_back(makeCourse("Algorithms", "89101"));
    courses.push_back(makeCourse("Databases", "89102"));
    courses.push_back(makeCourse("Networks", "89103"));

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    const std::map<std::string, const Course*> pointersBefore =
        assignmentPointersByCourseId(manager);

    expectAssignmentCount(manager, 3);
    expectNoNullOrDuplicateCoursePointers(manager);

    QVariantMap response = manager.requestMove("89102", "02-01-2026");

    TEST_EXPECT_EQ(response.value("status").toInt(), 1);

    expectAssignmentCount(manager, 3);
    expectNoNullOrDuplicateCoursePointers(manager);

    const std::map<std::string, const Course*> pointersAfter =
        assignmentPointersByCourseId(manager);

    TEST_EXPECT_EQ(pointersAfter.at("89101"), pointersBefore.at("89101"));
    TEST_EXPECT_EQ(pointersAfter.at("89102"), pointersBefore.at("89102"));
    TEST_EXPECT_EQ(pointersAfter.at("89103"), pointersBefore.at("89103"));

    const std::map<std::string, Date> datesAfter =
        assignmentDatesByCourseId(manager);

    TEST_EXPECT_EQ(datesAfter.at("89101"), Date(1, 1, 2026));
    TEST_EXPECT_EQ(datesAfter.at("89102"), Date(2, 1, 2026));
    TEST_EXPECT_EQ(datesAfter.at("89103"), Date(5, 1, 2026));
}

void testRejectedMovePreservesOriginalAssignmentsExactly() {
    std::vector<Course> courses;
    courses.reserve(3);

    courses.push_back(makeCourse("Algorithms", "89101"));
    courses.push_back(makeCourse("Databases", "89102"));
    courses.push_back(makeCourse("Networks", "89103"));

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    const std::map<std::string, Date> datesBefore =
        assignmentDatesByCourseId(manager);

    const std::map<std::string, const Course*> pointersBefore =
        assignmentPointersByCourseId(manager);

    manager.setMoveValidator(
        [](const ScheduleGenerationResult&,
           const std::string&,
           const Date&) {
            return false;
        }
    );

    QVariantMap response = manager.requestMove("89102", "02-01-2026");

    TEST_EXPECT_EQ(response.value("status").toInt(), 0);

    expectAssignmentCount(manager, 3);
    expectNoNullOrDuplicateCoursePointers(manager);

    const std::map<std::string, Date> datesAfter =
        assignmentDatesByCourseId(manager);

    const std::map<std::string, const Course*> pointersAfter =
        assignmentPointersByCourseId(manager);

    TEST_EXPECT_EQ(datesAfter.at("89101"), datesBefore.at("89101"));
    TEST_EXPECT_EQ(datesAfter.at("89102"), datesBefore.at("89102"));
    TEST_EXPECT_EQ(datesAfter.at("89103"), datesBefore.at("89103"));

    TEST_EXPECT_EQ(pointersAfter.at("89101"), pointersBefore.at("89101"));
    TEST_EXPECT_EQ(pointersAfter.at("89102"), pointersBefore.at("89102"));
    TEST_EXPECT_EQ(pointersAfter.at("89103"), pointersBefore.at("89103"));
}

void testMetricsUpdaterReceivesValidPostMoveSchedule() {
    std::vector<Course> courses;
    courses.reserve(3);

    courses.push_back(makeCourse("Algorithms", "89101"));
    courses.push_back(makeCourse("Databases", "89102"));
    courses.push_back(makeCourse("Networks", "89103"));

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    bool metricsUpdaterWasCalled = false;

    manager.setMetricsUpdater(
        [&](const ScheduleGenerationResult& updatedSchedule) {
            metricsUpdaterWasCalled = true;

            TEST_EXPECT_EQ(
                updatedSchedule.getAssignments().size(),
                static_cast<std::size_t>(3)
            );

            std::set<std::string> courseIds;

            for (const ExamAssignment& assignment : updatedSchedule.getAssignments()) {
                TEST_EXPECT_TRUE(assignment.course != nullptr);
                TEST_EXPECT_TRUE(
                    courseIds.insert(
                        assignment.course->getCourseNumber()
                    ).second
                );
            }

            TEST_EXPECT_TRUE(courseIds.find("89101") != courseIds.end());
            TEST_EXPECT_TRUE(courseIds.find("89102") != courseIds.end());
            TEST_EXPECT_TRUE(courseIds.find("89103") != courseIds.end());

            ScheduleMetrics metrics;
            metrics.maxExamsInSingleDay = 3;
            return metrics;
        }
    );

    QVariantMap response = manager.requestMove("89102", "02-01-2026");

    TEST_EXPECT_EQ(response.value("status").toInt(), 1);
    TEST_EXPECT_TRUE(metricsUpdaterWasCalled);

    TEST_EXPECT_EQ(
        manager.getCurrentMetrics().value("maxExamsInSingleDay").toInt(),
        3
    );

    expectAssignmentCount(manager, 3);
    expectNoNullOrDuplicateCoursePointers(manager);
}

void testMoveWithUnknownCourseDoesNotLoseAssignments() {
    std::vector<Course> courses;
    courses.reserve(3);

    courses.push_back(makeCourse("Algorithms", "89101"));
    courses.push_back(makeCourse("Databases", "89102"));
    courses.push_back(makeCourse("Networks", "89103"));

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    const std::map<std::string, Date> datesBefore =
        assignmentDatesByCourseId(manager);

    QVariantMap response = manager.requestMove("99999", "02-01-2026");

    TEST_EXPECT_EQ(response.value("status").toInt(), 0);

    expectAssignmentCount(manager, 3);
    expectNoNullOrDuplicateCoursePointers(manager);

    const std::map<std::string, Date> datesAfter =
        assignmentDatesByCourseId(manager);

    TEST_EXPECT_EQ(datesAfter.at("89101"), datesBefore.at("89101"));
    TEST_EXPECT_EQ(datesAfter.at("89102"), datesBefore.at("89102"));
    TEST_EXPECT_EQ(datesAfter.at("89103"), datesBefore.at("89103"));
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    testSuccessfulMovePreservesAssignmentCountAndCoursePointers();
    testRejectedMovePreservesOriginalAssignmentsExactly();
    testMetricsUpdaterReceivesValidPostMoveSchedule();
    testMoveWithUnknownCourseDoesNotLoseAssignments();

    std::cout << "ScheduleOutputManagerMoveMemoryIntegrityTest passed." << std::endl;
    return EXIT_SUCCESS;
}