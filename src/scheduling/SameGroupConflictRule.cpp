#include "scheduling/SameGroupConflictRule.h"

bool SameGroupConflictRule::isSatisfied(const SchedulingState& state,
                                        const RuntimeCourse& course,
                                        int dateIndex) const {
    for (const CourseMembership& membership : course.memberships) {
        const int groupId = membership.groupId;

        if (membership.requirement == Requirement::OBLIGATORY) {
            if (state.obligatoryCount[groupId][dateIndex] > 0) {
                return false;
            }

            if (state.electiveCount[groupId][dateIndex] > 0) {
                return false;
            }
        } else {
            if (state.obligatoryCount[groupId][dateIndex] > 0) {
                return false;
            }
        }
    }

    return true;
}