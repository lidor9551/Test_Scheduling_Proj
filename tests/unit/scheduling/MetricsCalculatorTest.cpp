#include "scheduling/MetricsCalculator.h"
#include "TestMacros.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool almostEqual(double left, double right, double epsilon = 0.0001) {
    return std::fabs(left - right) < epsilon;
}

Course makeCourse(
    const std::string& name,
    const std::string& number,
    Requirement requirement = Requirement::OBLIGATORY
) {
    Course course(name, number, "Dr. Metrics", Evaluation::EXAM);
    course.addProgram("83108", Year::FIRST, Semester::FALL, requirement);
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
        { groupId, requirement }
    };
    return runtimeCourse;
}

SchedulingBlock makeBlock(const std::vector<RuntimeCourse>& runtimeCourses) {
    SchedulingBlock block;
    block.semester = "FALL";
    block.moed = "Aleph";
    block.allowedDates = {
        Date(1, 1, 2026),
        Date(3, 1, 2026),
        Date(6, 1, 2026),
        Date(10, 1, 2026)
    };
    block.runtimeCourses = runtimeCourses;
    return block;
}

ScheduleGenerationResult makeResult(const std::vector<ExamAssignment>& assignments) {
    return ScheduleGenerationResult(assignments);
}

void testEmptyScheduleReturnsZeroMetrics() {
    std::vector<RuntimeCourse> runtimeCourses;
    SchedulingBlock block = makeBlock(runtimeCourses);
    ScheduleGenerationResult result;

    ScheduleMetrics metrics = MetricsCalculator::calculate(result, block);

    TEST_EXPECT_TRUE(almostEqual(metrics.avgDaysBetweenObligatory, 0.0));
    TEST_EXPECT_TRUE(almostEqual(metrics.avgDaysBetweenAll, 0.0));
    TEST_EXPECT_EQ(metrics.totalElectiveConflicts, 0);
    TEST_EXPECT_EQ(metrics.obligatorySpan, 0);
    TEST_EXPECT_EQ(metrics.maxExamsInSingleDay, 0);
}

void testObligatoryAverageAndSpan() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Mandatory A", "89101"));
    courses.push_back(makeCourse("Mandatory B", "89102"));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 0, Requirement::OBLIGATORY)
    };

    SchedulingBlock block = makeBlock(runtimeCourses);

    ScheduleGenerationResult result = makeResult({
        { &courses[0], Date(1, 1, 2026), true },
        { &courses[1], Date(6, 1, 2026), true }
    });

    ScheduleMetrics metrics = MetricsCalculator::calculate(result, block);

    TEST_EXPECT_TRUE(almostEqual(metrics.avgDaysBetweenObligatory, 5.0));
    TEST_EXPECT_EQ(metrics.obligatorySpan, 5);
    TEST_EXPECT_TRUE(almostEqual(metrics.avgDaysBetweenAll, 5.0));
    TEST_EXPECT_EQ(metrics.totalElectiveConflicts, 0);
    TEST_EXPECT_EQ(metrics.maxExamsInSingleDay, 1);
}

void testAverageDaysBetweenAllExams() {
    std::vector<Course> courses;
    courses.reserve(3);

    courses.push_back(makeCourse("Mandatory A", "89101", Requirement::OBLIGATORY));
    courses.push_back(makeCourse("Elective A", "89102", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective B", "89103", Requirement::ELECTIVE));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(2, courses[2], 0, Requirement::ELECTIVE)
    };

    SchedulingBlock block = makeBlock(runtimeCourses);

    /*
     * Group 0 has exams on:
     * 01-01-2026, 03-01-2026, 06-01-2026
     *
     * Gaps:
     * 2 days, 3 days
     *
     * Average = 2.5
     */
    ScheduleGenerationResult result = makeResult({
        { &courses[0], Date(1, 1, 2026), true },
        { &courses[1], Date(3, 1, 2026), false },
        { &courses[2], Date(6, 1, 2026), false }
    });

    ScheduleMetrics metrics = MetricsCalculator::calculate(result, block);

    TEST_EXPECT_TRUE(almostEqual(metrics.avgDaysBetweenAll, 2.5));
    TEST_EXPECT_TRUE(almostEqual(metrics.avgDaysBetweenObligatory, 0.0));
    TEST_EXPECT_EQ(metrics.obligatorySpan, 0);
    TEST_EXPECT_EQ(metrics.maxExamsInSingleDay, 1);
}

void testTotalElectiveConflicts() {
    std::vector<Course> courses;
    courses.reserve(5);

    courses.push_back(makeCourse("Elective G0 A", "89101", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective G0 B", "89102", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective G1 A", "89103", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective G1 B", "89104", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective G1 C", "89105", Requirement::ELECTIVE));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(1, courses[1], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(2, courses[2], 1, Requirement::ELECTIVE),
        makeRuntimeCourse(3, courses[3], 1, Requirement::ELECTIVE),
        makeRuntimeCourse(4, courses[4], 1, Requirement::ELECTIVE)
    };

    SchedulingBlock block = makeBlock(runtimeCourses);

    /*
     * Group 0:
     * 2 elective exams on the same day => 1 conflict
     *
     * Group 1:
     * 3 elective exams on the same day => 2 conflicts
     *
     * Total = 3 conflicts
     */
    ScheduleGenerationResult result = makeResult({
        { &courses[0], Date(1, 1, 2026), false },
        { &courses[1], Date(1, 1, 2026), false },
        { &courses[2], Date(3, 1, 2026), false },
        { &courses[3], Date(3, 1, 2026), false },
        { &courses[4], Date(3, 1, 2026), false }
    });

    ScheduleMetrics metrics = MetricsCalculator::calculate(result, block);

    TEST_EXPECT_EQ(metrics.totalElectiveConflicts, 3);
    TEST_EXPECT_EQ(metrics.maxExamsInSingleDay, 3);
}

void testMaxExamsInSingleDay() {
    std::vector<Course> courses;
    courses.reserve(4);

    courses.push_back(makeCourse("Course A", "89101"));
    courses.push_back(makeCourse("Course B", "89102"));
    courses.push_back(makeCourse("Course C", "89103"));
    courses.push_back(makeCourse("Course D", "89104"));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 1, Requirement::OBLIGATORY),
        makeRuntimeCourse(2, courses[2], 2, Requirement::OBLIGATORY),
        makeRuntimeCourse(3, courses[3], 0, Requirement::OBLIGATORY)
    };

    SchedulingBlock block = makeBlock(runtimeCourses);

    ScheduleGenerationResult result = makeResult({
        { &courses[0], Date(1, 1, 2026), true },
        { &courses[1], Date(1, 1, 2026), true },
        { &courses[2], Date(1, 1, 2026), true },
        { &courses[3], Date(6, 1, 2026), true }
    });

    ScheduleMetrics metrics = MetricsCalculator::calculate(result, block);

    TEST_EXPECT_EQ(metrics.maxExamsInSingleDay, 3);
}

} // namespace

int main() {
    testEmptyScheduleReturnsZeroMetrics();
    testObligatoryAverageAndSpan();
    testAverageDaysBetweenAllExams();
    testTotalElectiveConflicts();
    testMaxExamsInSingleDay();

    std::cout << "MetricsCalculatorTest passed." << std::endl;
    return EXIT_SUCCESS;
}