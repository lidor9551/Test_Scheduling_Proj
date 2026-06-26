#include "presentation/ScheduleOutputManager.h"
#include "TestMacros.h"

#include <QCoreApplication>
#include <QVariantList>
#include <QVariantMap>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

Course makeCourse() {
    Course course("Algorithms", "89101", "Dr. Boundary", Evaluation::EXAM);

    course.addProgram(
        "83108",
        Year::FIRST,
        Semester::FALL,
        Requirement::OBLIGATORY
    );

    return course;
}

ExcludedRange makeExcludedDate(const Date& date) {
    ExcludedRange range;
    range.start = date;
    range.end = date;
    range.reason = "manual exclusion";
    return range;
}

ExamPeriod makePeriod() {
    return ExamPeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(10, 1, 2026),
        {
            makeExcludedDate(Date(5, 1, 2026))
        }
    );
}

ScheduleGenerationResult makeResult(const Course& course, const Date& date) {
    return ScheduleGenerationResult({
        ExamAssignment{&course, date, true}
    });
}

void configureManager(
    ScheduleOutputManager& manager,
    std::vector<Course>& courses,
    const Date& initialDate = Date(1, 1, 2026)
) {
    ScheduleGenerationResult result = makeResult(courses[0], initialDate);

    manager.setAvailablePeriods({"FALL"}, {"Aleph"});
    manager.setPeriodFilter("FALL", "Aleph");
    manager.setSchedulingData(
        { result },
        courses,
        { makePeriod() }
    );
}

Date assignedDate(const ScheduleOutputManager& manager) {
    const std::vector<ScheduleGenerationResult>& solutions = manager.getSolutions();

    TEST_EXPECT_EQ(solutions.size(), static_cast<std::size_t>(1));
    TEST_EXPECT_EQ(solutions[0].getAssignments().size(), static_cast<std::size_t>(1));

    return solutions[0].getAssignments()[0].examDate;
}

void testValidMoveInsidePeriodIsAccepted() {
    std::vector<Course> courses;
    courses.push_back(makeCourse());

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    QVariantMap response = manager.requestMove("89101", "02-01-2026");

    TEST_EXPECT_EQ(response.value("status").toInt(), 1);
    TEST_EXPECT_EQ(assignedDate(manager), Date(2, 1, 2026));
}

void testMoveOutsidePeriodIsRejected() {
    std::vector<Course> courses;
    courses.push_back(makeCourse());

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    QVariantMap response = manager.requestMove("89101", "11-01-2026");

    TEST_EXPECT_EQ(response.value("status").toInt(), 0);
    TEST_EXPECT_EQ(response.value("error").toString(), QString("Invalid target date"));
    TEST_EXPECT_EQ(assignedDate(manager), Date(1, 1, 2026));
}

void testMoveToExcludedDateIsRejected() {
    std::vector<Course> courses;
    courses.push_back(makeCourse());

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    QVariantMap response = manager.requestMove("89101", "05-01-2026");

    TEST_EXPECT_EQ(response.value("status").toInt(), 0);
    TEST_EXPECT_EQ(response.value("error").toString(), QString("Invalid target date"));
    TEST_EXPECT_EQ(assignedDate(manager), Date(1, 1, 2026));
}

void testMoveToSaturdayIsRejected() {
    std::vector<Course> courses;
    courses.push_back(makeCourse());

    ScheduleOutputManager manager;
    configureManager(manager, courses);

    // 03-01-2026 is Saturday and should not appear in allowedDates().
    QVariantMap response = manager.requestMove("89101", "03-01-2026");

    TEST_EXPECT_EQ(response.value("status").toInt(), 0);
    TEST_EXPECT_EQ(response.value("error").toString(), QString("Invalid target date"));
    TEST_EXPECT_EQ(assignedDate(manager), Date(1, 1, 2026));
}

void testValidDatesMapContainsOnlyAllowedPeriodDates() {
    std::vector<Course> courses;
    courses.push_back(makeCourse());

    ScheduleOutputManager manager;
    configureManager(manager, courses);
    
    QVariantMap validDates = manager.getValidDatesForCourse("89101");

    TEST_EXPECT_TRUE(validDates.contains("01-01-2026"));
    TEST_EXPECT_TRUE(validDates.contains("02-01-2026"));
    TEST_EXPECT_TRUE(validDates.contains("04-01-2026"));
    TEST_EXPECT_TRUE(validDates.contains("06-01-2026"));
    TEST_EXPECT_TRUE(validDates.contains("07-01-2026"));
    TEST_EXPECT_TRUE(validDates.contains("08-01-2026"));
    TEST_EXPECT_TRUE(validDates.contains("09-01-2026"));

    // Saturday and manually excluded date should not be offered as move targets.
    TEST_EXPECT_FALSE(validDates.contains("03-01-2026"));
    TEST_EXPECT_FALSE(validDates.contains("05-01-2026"));

    // Outside the exam period should not be offered.
    TEST_EXPECT_FALSE(validDates.contains("11-01-2026"));
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    testValidMoveInsidePeriodIsAccepted();
    testMoveOutsidePeriodIsRejected();
    testMoveToExcludedDateIsRejected();
    testMoveToSaturdayIsRejected();
    testValidDatesMapContainsOnlyAllowedPeriodDates();

    std::cout << "ScheduleOutputManagerMoveDateBoundaryTest passed." << std::endl;
    return EXIT_SUCCESS;
}