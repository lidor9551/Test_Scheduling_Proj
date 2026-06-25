#include "scheduling/SameGroupConflictRule.h"
#include "scheduling/SchedulingState.h"
#include "TestMacros.h"

#include <iostream>
#include <vector>

int main() {
    SameGroupConflictRule rule;

    SchedulingState state;
    state.assignedDate = { -1 };
    state.obligatoryCount = {
        { 0, 1 },
        { 0, 0 }
    };
    state.electiveCount = {
        { 0, 1 },
        { 0, 0 }
    };

    RuntimeCourse obligatoryCourse;
    obligatoryCourse.id = 0;
    obligatoryCourse.course = nullptr;
    obligatoryCourse.memberships = {
        { 0, Requirement::OBLIGATORY }
    };

    RuntimeCourse electiveCourse;
    electiveCourse.id = 0;
    electiveCourse.course = nullptr;
    electiveCourse.memberships = {
        { 0, Requirement::ELECTIVE }
    };

    TEST_EXPECT_TRUE(rule.isSatisfied(state, obligatoryCourse, 0));
    TEST_EXPECT_FALSE(rule.isSatisfied(state, obligatoryCourse, 1));

    TEST_EXPECT_TRUE(rule.isSatisfied(state, electiveCourse, 0));
    TEST_EXPECT_FALSE(rule.isSatisfied(state, electiveCourse, 1));

    SchedulingState electiveOnlyState;
    electiveOnlyState.assignedDate = { -1 };
    electiveOnlyState.obligatoryCount = {
        { 0, 0 }
    };
    electiveOnlyState.electiveCount = {
        { 0, 1 }
    };

    TEST_EXPECT_TRUE(rule.isSatisfied(electiveOnlyState, electiveCourse, 1));
    TEST_EXPECT_FALSE(rule.isSatisfied(electiveOnlyState, obligatoryCourse, 1));

    std::cout << "SameGroupConflictRule test passed." << std::endl;
    return EXIT_SUCCESS;
}