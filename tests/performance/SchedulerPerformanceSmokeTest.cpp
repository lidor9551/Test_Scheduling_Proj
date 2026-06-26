#include "scheduling/ScheduleGenerator.h"
#include "TestMacros.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr int CourseCount = 10;
constexpr int SolutionLimit = 25;
constexpr double MaxAllowedSeconds = 30.0;

Course makeCourse(
    const std::string& name,
    const std::string& number,
    Requirement requirement
) {
    Course course(name, number, "Dr. Performance", Evaluation::EXAM);

    course.addProgram(
        "83108",
        Year::FIRST,
        Semester::FALL,
        requirement
    );

    return course;
}

std::vector<Course> makeCourses() {
    std::vector<Course> courses;
    courses.reserve(CourseCount);

    for (int i = 0; i < CourseCount; ++i) {
        const std::string number = "89" + std::to_string(100 + i);
        const std::string name = "Performance Course " + std::to_string(i + 1);

        courses.push_back(
            makeCourse(
                name,
                number,
                Requirement::OBLIGATORY
            )
        );
    }

    return courses;
}

std::vector<Date> makeAllowedDates() {
    return {
        Date(1, 1, 2026),
        Date(2, 1, 2026),
        Date(4, 1, 2026),
        Date(5, 1, 2026),
        Date(6, 1, 2026),
        Date(7, 1, 2026),
        Date(8, 1, 2026),
        Date(9, 1, 2026),
        Date(11, 1, 2026),
        Date(12, 1, 2026),
        Date(13, 1, 2026),
        Date(14, 1, 2026)
    };
}

std::vector<RuntimeCourse> makeRuntimeCourses(
    const std::vector<Course>& courses
) {
    std::vector<RuntimeCourse> runtimeCourses;
    runtimeCourses.reserve(courses.size());

    for (std::size_t i = 0; i < courses.size(); ++i) {
        RuntimeCourse runtimeCourse;
        runtimeCourse.id = static_cast<int>(i);
        runtimeCourse.course = &courses[i];

        /*
         * All courses are obligatory for the same group.
         * This keeps the test realistic because the SameGroupConflictRule
         * must prevent two mandatory exams for the same group on the same day.
         */
        runtimeCourse.memberships = {
            CourseMembership{0, Requirement::OBLIGATORY}
        };

        runtimeCourses.push_back(runtimeCourse);
    }

    return runtimeCourses;
}

SchedulingBlock makeSchedulingBlock(
    const std::vector<RuntimeCourse>& runtimeCourses
) {
    SchedulingBlock block;

    block.semester = "FALL";
    block.moed = "Aleph";
    block.allowedDates = makeAllowedDates();
    block.runtimeCourses = runtimeCourses;

    return block;
}

ScheduleSettings makeSettings() {
    ScheduleSettings settings;

    /*
     * Keep all optional Phase 3 hard constraints disabled.
     * This performance smoke test validates the core scheduling path
     * with the default conflict rules and metrics calculation.
     */
    settings.minDaysObligatory = RuleConfig{false, 0};
    settings.minDaysAll = RuleConfig{false, 0};
    settings.maxElectiveConflicts = RuleConfig{false, 0};
    settings.obligatorySpan = RuleConfig{false, 0};
    settings.maxExamsPerDay = RuleConfig{false, 0};

    return settings;
}

void assertSolutionsAreComplete(
    const std::vector<ScheduleGenerationResult>& solutions
) {
    TEST_EXPECT_TRUE(!solutions.empty());
    TEST_EXPECT_TRUE(
        static_cast<int>(solutions.size()) <= SolutionLimit
    );

    for (const ScheduleGenerationResult& solution : solutions) {
        TEST_EXPECT_EQ(
            solution.getAssignments().size(),
            static_cast<std::size_t>(CourseCount)
        );

        for (const ExamAssignment& assignment : solution.getAssignments()) {
            TEST_EXPECT_TRUE(assignment.course != nullptr);
        }
    }
}

void testSchedulerCompletesWithinPerformanceBudget() {
    std::vector<Course> courses = makeCourses();
    std::vector<RuntimeCourse> runtimeCourses = makeRuntimeCourses(courses);
    SchedulingBlock block = makeSchedulingBlock(runtimeCourses);

    ScheduleGenerator generator(
        block,
        makeSettings(),
        MaxAllowedSeconds
    );

    const auto start = std::chrono::steady_clock::now();

    std::vector<ScheduleGenerationResult> solutions =
        generator.runBacktracking(SolutionLimit);

    const auto end = std::chrono::steady_clock::now();

    const double elapsedSeconds =
        std::chrono::duration<double>(end - start).count();

    std::cout << "SchedulerPerformanceSmokeTest elapsed seconds: "
              << elapsedSeconds
              << std::endl;

    assertSolutionsAreComplete(solutions);

    TEST_EXPECT_TRUE(elapsedSeconds < MaxAllowedSeconds);
}

} // namespace

int main() {
    testSchedulerCompletesWithinPerformanceBudget();

    std::cout << "SchedulerPerformanceSmokeTest passed." << std::endl;
    return EXIT_SUCCESS;
}