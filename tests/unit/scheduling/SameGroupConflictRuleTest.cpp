#include "scheduling/SameGroupConflictRule.h"
#include "scheduling/SchedulingState.h"

#include <cassert>
#include <iostream>

int main() {
    SameGroupConflictRule rule;

    SchedulingState state;
    state.assignedDate = {-1};
    state.obligatoryCount = {
        {0, 1},
        {0, 0}
    };
    state.electiveCount = {
        {0, 1},
        {0, 0}
    };

    RuntimeCourse obligatoryCourse;
    obligatoryCourse.id = 0;
    obligatoryCourse.course = nullptr;
    obligatoryCourse.memberships = {
        {0, Requirement::OBLIGATORY}
    };

    RuntimeCourse electiveCourse;
    electiveCourse.id = 0;
    electiveCourse.course = nullptr;
    electiveCourse.memberships = {
        {0, Requirement::ELECTIVE}
    };

    assert(rule.isSatisfied(state, obligatoryCourse, 0));
    assert(!rule.isSatisfied(state, obligatoryCourse, 1));

    assert(rule.isSatisfied(state, electiveCourse, 0));
    assert(!rule.isSatisfied(state, electiveCourse, 1));

    SchedulingState electiveOnlyState;
    electiveOnlyState.assignedDate = {-1};
    electiveOnlyState.obligatoryCount = {
        {0, 0}
    };
    electiveOnlyState.electiveCount = {
        {0, 1}
    };

    assert(rule.isSatisfied(electiveOnlyState, electiveCourse, 1));
    assert(!rule.isSatisfied(electiveOnlyState, obligatoryCourse, 1));

    std::cout << "SameGroupConflictRule test passed." << std::endl;
    return 0;
}