#include "domain/ScheduleGenerationResult.h"
#include "TestMacros.h"

#include <iostream>
#include <string>
#include <vector>

int main() {
    Course algorithms("Algorithms", "89101", "Dr. A", Evaluation::EXAM);
    algorithms.addProgram("83108", Year::SECOND, Semester::FALL, Requirement::OBLIGATORY);

    Course operatingSystems("Operating Systems", "89202", "Dr. B", Evaluation::EXAM);
    operatingSystems.addProgram("83108", Year::SECOND, Semester::FALL, Requirement::OBLIGATORY);

    ExamAssignment firstAssignment{
        &algorithms,
        Date(4, 1, 2026),
        true
    };

    ExamAssignment secondAssignment{
        &operatingSystems,
        Date(5, 1, 2026),
        true
    };

    ScheduleGenerationResult result({
        firstAssignment,
        secondAssignment
    });

    const std::vector<ExamAssignment>& assignments = result.getAssignments();

    TEST_EXPECT_EQ(assignments.size(), static_cast<std::size_t>(2));

    // The result must preserve the exact Course object identity.
    TEST_EXPECT_TRUE(assignments[0].course == &algorithms);
    TEST_EXPECT_TRUE(assignments[1].course == &operatingSystems);

    // The preserved pointers should still expose the original course data.
    TEST_EXPECT_EQ(assignments[0].course->getCourseNumber(), std::string("89101"));
    TEST_EXPECT_EQ(assignments[0].course->getCourseName(), std::string("Algorithms"));

    TEST_EXPECT_EQ(assignments[1].course->getCourseNumber(), std::string("89202"));
    TEST_EXPECT_EQ(assignments[1].course->getCourseName(), std::string("Operating Systems"));

    // Dates and requirement flags should also remain unchanged.
    TEST_EXPECT_EQ(assignments[0].examDate, Date(4, 1, 2026));
    TEST_EXPECT_EQ(assignments[1].examDate, Date(5, 1, 2026));

    TEST_EXPECT_TRUE(assignments[0].isObligatory);
    TEST_EXPECT_TRUE(assignments[1].isObligatory);

    // V3 metrics currently default to zero.
    TEST_EXPECT_EQ(result.getElectiveConflicts(), 0);
    TEST_EXPECT_EQ(result.getMinDaysBetweenObligatory(), 0);

    std::cout << "ScheduleGenerationResult test passed." << std::endl;
    return EXIT_SUCCESS;
}