#include "scheduling/AdvancedConflictRules.h"
#include "TestMacros.h"

#include <iostream>
#include <string>
#include <vector>

namespace {

Course makeCourse(
    const std::string& name,
    const std::string& number,
    Requirement domainRequirement = Requirement::OBLIGATORY
) {
    Course course(name, number, "Dr. Test", Evaluation::EXAM);
    course.addProgram("83108", Year::FIRST, Semester::FALL, domainRequirement);
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

SchedulingState makeState(
    std::size_t courseCount,
    std::size_t dateCount,
    std::size_t groupCount = 3
) {
    SchedulingState state;
    state.assignedDate = std::vector<int>(courseCount, -1);
    state.obligatoryCount =
        std::vector<std::vector<int>>(groupCount, std::vector<int>(dateCount, 0));
    state.electiveCount =
        std::vector<std::vector<int>>(groupCount, std::vector<int>(dateCount, 0));
    state.totalExamsPerDate = std::vector<int>(dateCount, 0);
    return state;
}

void assignCourse(
    SchedulingState& state,
    const RuntimeCourse& course,
    int dateIndex
) {
    state.assignedDate[course.id] = dateIndex;
    state.totalExamsPerDate[dateIndex]++;

    for (const CourseMembership& membership : course.memberships) {
        if (membership.requirement == Requirement::OBLIGATORY) {
            state.obligatoryCount[membership.groupId][dateIndex]++;
        } else {
            state.electiveCount[membership.groupId][dateIndex]++;
        }
    }
}

std::vector<Date> standardDates() {
    return {
        Date(1, 1, 2026),
        Date(2, 1, 2026),
        Date(3, 1, 2026),
        Date(6, 1, 2026)
    };
}

void testMinDaysObligatoryRule() {
    std::vector<Date> dates = standardDates();

    std::vector<Course> courses;
    courses.push_back(makeCourse("Algorithms", "89101"));
    courses.push_back(makeCourse("Operating Systems", "89102"));
    courses.push_back(makeCourse("Databases", "89103", Requirement::ELECTIVE));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(2, courses[2], 0, Requirement::ELECTIVE)
    };

    SchedulingState state = makeState(runtimeCourses.size(), dates.size());
    assignCourse(state, runtimeCourses[0], 0);

    MinDaysObligatoryRule rule(3, dates, runtimeCourses);

    TEST_EXPECT_FALSE(rule.isSatisfied(state, runtimeCourses[1], 1));
    TEST_EXPECT_TRUE(rule.isSatisfied(state, runtimeCourses[1], 3));
    TEST_EXPECT_TRUE(rule.isSatisfied(state, runtimeCourses[2], 1));
}

void testMinDaysObligatoryUsesRuntimeMembershipsOnly() {
    std::vector<Date> dates = standardDates();

    std::vector<Course> courses;
    courses.push_back(makeCourse("Algorithms", "89101"));
    courses.push_back(makeCourse("Operating Systems", "89102"));

    /*
     * Both domain Course objects contain the same program/year.
     * The runtime memberships intentionally use different group IDs.
     *
     * This guards against using Course::getPrograms() inside scheduling rules.
     * Rules must rely on RuntimeCourse::memberships, because the preprocessor
     * already filtered the data according to the selected programs and period.
     */
    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 1, Requirement::OBLIGATORY)
    };

    SchedulingState state = makeState(runtimeCourses.size(), dates.size());
    assignCourse(state, runtimeCourses[0], 0);

    MinDaysObligatoryRule rule(3, dates, runtimeCourses);
    TEST_EXPECT_TRUE(rule.isSatisfied(state, runtimeCourses[1], 1));
}

void testMinDaysAllRule() {
    std::vector<Date> dates = standardDates();

    std::vector<Course> courses;
    courses.push_back(makeCourse("Algorithms", "89101"));
    courses.push_back(makeCourse("Elective A", "89102", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective B", "89103", Requirement::ELECTIVE));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(2, courses[2], 1, Requirement::ELECTIVE)
    };

    SchedulingState state = makeState(runtimeCourses.size(), dates.size());
    assignCourse(state, runtimeCourses[0], 0);

    MinDaysAllRule rule(3, dates, runtimeCourses);

    TEST_EXPECT_FALSE(rule.isSatisfied(state, runtimeCourses[1], 1));
    TEST_EXPECT_TRUE(rule.isSatisfied(state, runtimeCourses[1], 3));
    TEST_EXPECT_TRUE(rule.isSatisfied(state, runtimeCourses[2], 1));
}

void testMaxElectiveConflictsRule() {
    std::vector<Date> dates = standardDates();

    std::vector<Course> courses;
    courses.push_back(makeCourse("Elective A", "89101", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective B", "89102", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Elective C", "89103", Requirement::ELECTIVE));
    courses.push_back(makeCourse("Other Group Elective", "89104", Requirement::ELECTIVE));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(1, courses[1], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(2, courses[2], 0, Requirement::ELECTIVE),
        makeRuntimeCourse(3, courses[3], 1, Requirement::ELECTIVE)
    };

    SchedulingState state = makeState(runtimeCourses.size(), dates.size());
    assignCourse(state, runtimeCourses[0], 0);

    MaxElectiveConflictsRule zeroConflictsAllowed(0, runtimeCourses);
    TEST_EXPECT_FALSE(zeroConflictsAllowed.isSatisfied(state, runtimeCourses[1], 0));
    TEST_EXPECT_TRUE(zeroConflictsAllowed.isSatisfied(state, runtimeCourses[1], 1));
    TEST_EXPECT_TRUE(zeroConflictsAllowed.isSatisfied(state, runtimeCourses[3], 0));

    MaxElectiveConflictsRule oneConflictAllowed(1, runtimeCourses);
    TEST_EXPECT_TRUE(oneConflictAllowed.isSatisfied(state, runtimeCourses[1], 0));

    assignCourse(state, runtimeCourses[1], 0);
    TEST_EXPECT_FALSE(oneConflictAllowed.isSatisfied(state, runtimeCourses[2], 0));
}

void testObligatorySpanRule() {
    std::vector<Date> dates = standardDates();

    std::vector<Course> courses;
    courses.push_back(makeCourse("Mandatory A", "89101"));
    courses.push_back(makeCourse("Mandatory B", "89102"));
    courses.push_back(makeCourse("Mandatory C", "89103"));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(2, courses[2], 0, Requirement::OBLIGATORY)
    };

    ObligatorySpanRule rule(4, dates, runtimeCourses);

    SchedulingState partialState = makeState(runtimeCourses.size(), dates.size());
    assignCourse(partialState, runtimeCourses[0], 0);
    TEST_EXPECT_TRUE(rule.isSatisfied(partialState, runtimeCourses[1], 1));

    SchedulingState completeState = makeState(runtimeCourses.size(), dates.size());
    assignCourse(completeState, runtimeCourses[0], 0);
    assignCourse(completeState, runtimeCourses[1], 1);

    TEST_EXPECT_FALSE(rule.isSatisfied(completeState, runtimeCourses[2], 2));
    TEST_EXPECT_TRUE(rule.isSatisfied(completeState, runtimeCourses[2], 3));
}

void testMaxExamsPerDayRule() {
    std::vector<Date> dates = standardDates();

    std::vector<Course> courses;
    courses.push_back(makeCourse("Course A", "89101"));
    courses.push_back(makeCourse("Course B", "89102"));
    courses.push_back(makeCourse("Course C", "89103"));

    std::vector<RuntimeCourse> runtimeCourses = {
        makeRuntimeCourse(0, courses[0], 0, Requirement::OBLIGATORY),
        makeRuntimeCourse(1, courses[1], 1, Requirement::OBLIGATORY),
        makeRuntimeCourse(2, courses[2], 2, Requirement::OBLIGATORY)
    };

    SchedulingState state = makeState(runtimeCourses.size(), dates.size());
    assignCourse(state, runtimeCourses[0], 0);
    assignCourse(state, runtimeCourses[1], 0);

    MaxExamsPerDayRule maxTwo(2);
    TEST_EXPECT_FALSE(maxTwo.isSatisfied(state, runtimeCourses[2], 0));
    TEST_EXPECT_TRUE(maxTwo.isSatisfied(state, runtimeCourses[2], 1));

    MaxExamsPerDayRule maxThree(3);
    TEST_EXPECT_TRUE(maxThree.isSatisfied(state, runtimeCourses[2], 0));
}

} // namespace

int main() {
    testMinDaysObligatoryRule();
    testMinDaysObligatoryUsesRuntimeMembershipsOnly();
    testMinDaysAllRule();
    testMaxElectiveConflictsRule();
    testObligatorySpanRule();
    testMaxExamsPerDayRule();

    std::cout << "AdvancedConflictRulesTest passed." << std::endl;
    return EXIT_SUCCESS;
}
