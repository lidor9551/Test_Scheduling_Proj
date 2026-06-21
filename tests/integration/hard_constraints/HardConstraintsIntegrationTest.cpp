#include "scheduling/ScheduleGenerator.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace {

Course makeCourse(const std::string& name,
                  const std::string& number,
                  Requirement domainRequirement = Requirement::OBLIGATORY) {
    Course course(name, number, "Dr. Integration", Evaluation::EXAM);
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

SchedulingBlock makeBlock(const std::vector<RuntimeCourse>& runtimeCourses,
                          const std::vector<Date>& dates) {
    SchedulingBlock block;
    block.semester = "FALL";
    block.moed = "Aleph";
    block.allowedDates = dates;
    block.runtimeCourses = runtimeCourses;
    return block;
}

int maxExamsInSingleDay(const ScheduleGenerationResult& result) {
    int maxCount = 0;

    for (const ExamAssignment& outer : result.getAssignments()) {
        int count = 0;

        for (const ExamAssignment& inner : result.getAssignments()) {
            if (inner.examDate == outer.examDate) {
                count++;
            }
        }

        if (count > maxCount) {
            maxCount = count;
        }
    }

    return maxCount;
}

bool everyObligatoryPairHasAtLeastDays(const ScheduleGenerationResult& result, int minDays) {
    const auto& assignments = result.getAssignments();

    for (std::size_t i = 0; i < assignments.size(); ++i) {
        if (!assignments[i].isObligatory) {
            continue;
        }

        for (std::size_t j = i + 1; j < assignments.size(); ++j) {
            if (!assignments[j].isObligatory) {
                continue;
            }

            if (assignments[i].examDate.daysTo(assignments[j].examDate) < minDays) {
                return false;
            }
        }
    }

    return true;
}

void testDisabledConstraintsProduceSchedules() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Elective A", "89101", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective B", "89102", Requirement::ELECTIVE));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(1, courses[1], 0, Requirement::ELECTIVE)
    };

    SchedulingBlock block = makeBlock(
        runtimeCourses,
        {
            Date(1, 1, 2026),
            Date(2, 1, 2026)
        }
    );

    ScheduleSettings settings;
    ScheduleGenerator generator(block, settings, 5.0);

    std::vector<ScheduleGenerationResult> solutions = generator.runBacktracking(100);

    assert(!solutions.empty());
}

void testMinDaysObligatoryConstraintFiltersGeneratedSchedules() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Mandatory A", "89101"));
    courses.push_back(makeCourse("Mandatory B", "89102"));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 0, Requirement::OBLIGATORY)
    };

    SchedulingBlock block = makeBlock(
        runtimeCourses,
        {
            Date(1, 1, 2026),
            Date(2, 1, 2026),
            Date(5, 1, 2026)
        }
    );

    ScheduleSettings settings;
    settings.minDaysObligatory.isActive = true;
    settings.minDaysObligatory.k = 3;

    ScheduleGenerator generator(block, settings, 5.0);

    std::vector<ScheduleGenerationResult> solutions = generator.runBacktracking(100);

    assert(!solutions.empty());

    for (const ScheduleGenerationResult& solution : solutions) {
        assert(everyObligatoryPairHasAtLeastDays(solution, 3));
    }
}

void testMaxExamsPerDayCanMakeScheduleImpossible() {
    std::vector<Course> courses;
    courses.reserve(3);

    courses.push_back(makeCourse("Elective A", "89101", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective B", "89102", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective C", "89103", Requirement::ELECTIVE));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(1, courses[1], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(2, courses[2], 0, Requirement::ELECTIVE)
    };

    SchedulingBlock block = makeBlock(
        runtimeCourses,
        {
            Date(1, 1, 2026),
            Date(2, 1, 2026)
        }
    );

    ScheduleSettings impossibleSettings;
    impossibleSettings.maxExamsPerDay.isActive = true;
    impossibleSettings.maxExamsPerDay.k = 1;

    ScheduleGenerator impossibleGenerator(block, impossibleSettings, 5.0);
    std::vector<ScheduleGenerationResult> impossibleSolutions =
        impossibleGenerator.runBacktracking(100);

    assert(impossibleSolutions.empty());

    ScheduleSettings possibleSettings;
    possibleSettings.maxExamsPerDay.isActive = true;
    possibleSettings.maxExamsPerDay.k = 2;

    ScheduleGenerator possibleGenerator(block, possibleSettings, 5.0);
    std::vector<ScheduleGenerationResult> possibleSolutions =
        possibleGenerator.runBacktracking(100);

    assert(!possibleSolutions.empty());

    for (const ScheduleGenerationResult& solution : possibleSolutions) {
        assert(maxExamsInSingleDay(solution) <= 2);
    }
}

void testMaxElectiveConflictsCanMakeScheduleImpossible() {
    std::vector<Course> courses;
    courses.reserve(3);

    courses.push_back(makeCourse("Elective A", "89101", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective B", "89102", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective C", "89103", Requirement::ELECTIVE));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(1, courses[1], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(2, courses[2], 0, Requirement::ELECTIVE)
    };

    SchedulingBlock block = makeBlock(
        runtimeCourses,
        {
            Date(1, 1, 2026)
        }
    );

    ScheduleSettings strictSettings;
    strictSettings.maxElectiveConflicts.isActive = true;
    strictSettings.maxElectiveConflicts.k = 0;

    ScheduleGenerator strictGenerator(block, strictSettings, 5.0);
    std::vector<ScheduleGenerationResult> strictSolutions =
        strictGenerator.runBacktracking(100);

    assert(strictSolutions.empty());

    ScheduleSettings relaxedSettings;
    relaxedSettings.maxElectiveConflicts.isActive = true;
    relaxedSettings.maxElectiveConflicts.k = 2;

    ScheduleGenerator relaxedGenerator(block, relaxedSettings, 5.0);
    std::vector<ScheduleGenerationResult> relaxedSolutions =
        relaxedGenerator.runBacktracking(100);

    assert(!relaxedSolutions.empty());
}

void testObligatorySpanCanMakeScheduleImpossible() {
    std::vector<Course> courses;
    courses.reserve(3);

    courses.push_back(makeCourse("Mandatory A", "89101"));
    courses.push_back(makeCourse("Mandatory B", "89102"));
    courses.push_back(makeCourse("Mandatory C", "89103"));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(2, courses[2], 0, Requirement::OBLIGATORY)
    };

    SchedulingBlock block = makeBlock(
        runtimeCourses,
        {
            Date(1, 1, 2026),
            Date(3, 1, 2026),
            Date(6, 1, 2026)
        }
    );

    ScheduleSettings possibleSettings;
    possibleSettings.obligatorySpan.isActive = true;
    possibleSettings.obligatorySpan.k = 5;

    ScheduleGenerator possibleGenerator(block, possibleSettings, 5.0);
    std::vector<ScheduleGenerationResult> possibleSolutions =
        possibleGenerator.runBacktracking(100);

    assert(!possibleSolutions.empty());

    ScheduleSettings impossibleSettings;
    impossibleSettings.obligatorySpan.isActive = true;
    impossibleSettings.obligatorySpan.k = 6;

    ScheduleGenerator impossibleGenerator(block, impossibleSettings, 5.0);
    std::vector<ScheduleGenerationResult> impossibleSolutions =
        impossibleGenerator.runBacktracking(100);

    assert(impossibleSolutions.empty());
}

} // namespace

int main() {
    testDisabledConstraintsProduceSchedules();
    testMinDaysObligatoryConstraintFiltersGeneratedSchedules();
    testMaxExamsPerDayCanMakeScheduleImpossible();
    testMaxElectiveConflictsCanMakeScheduleImpossible();
    testObligatorySpanCanMakeScheduleImpossible();

    std::cout << "HardConstraintsIntegrationTest passed." << std::endl;
    return 0;
}