#include "domain/ScheduleGenerationResult.h"

#include <cassert>
#include <iostream>

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

    assert(assignments.size() == 2);

    // The result must preserve the exact Course object identity.
    assert(assignments[0].course == &algorithms);
    assert(assignments[1].course == &operatingSystems);

    // The preserved pointers should still expose the original course data.
    assert(assignments[0].course->getCourseNumber() == "89101");
    assert(assignments[0].course->getCourseName() == "Algorithms");

    assert(assignments[1].course->getCourseNumber() == "89202");
    assert(assignments[1].course->getCourseName() == "Operating Systems");

    // Dates and requirement flags should also remain unchanged.
    assert(assignments[0].examDate == Date(4, 1, 2026));
    assert(assignments[1].examDate == Date(5, 1, 2026));

    assert(assignments[0].isObligatory);
    assert(assignments[1].isObligatory);

    // V3 metrics currently default to zero.
    assert(result.getElectiveConflicts() == 0);
    assert(result.getMinDaysBetweenObligatory() == 0);

    std::cout << "ScheduleGenerationResult test passed." << std::endl;
    return 0;
}