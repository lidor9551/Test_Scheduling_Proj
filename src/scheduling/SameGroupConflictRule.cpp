#include "scheduling/SameGroupConflictRule.h"

/*
 * Checks whether a course can be placed on the requested date
 * without creating an illegal same-group conflict.
 *
 * The method loops over all memberships of the course because one course can
 * belong to more than one academic program/year group.
 */
bool SameGroupConflictRule::isSatisfied(const SchedulingState& state,
                                        const RuntimeCourse& course,
                                        int dateIndex) const {
    /*
     * Check the proposed assignment against every academic group
     * that takes this course.
     */
    for (const CourseMembership& membership : course.memberships) {
        const int groupId = membership.groupId;

        /*
         * Obligatory courses are strict:
         * they cannot share a date with any other obligatory or elective course
         * from the same academic group.
         */
        if (membership.requirement == Requirement::OBLIGATORY) {
            if (state.obligatoryCount[groupId][dateIndex] > 0) {
                return false;
            }

            if (state.electiveCount[groupId][dateIndex] > 0) {
                return false;
            }
        } else {
            /*
             * Elective courses are more flexible:
             * they may overlap with other elective courses,
             * but they cannot overlap with an obligatory course
             * from the same academic group.
             */
            if (state.obligatoryCount[groupId][dateIndex] > 0) {
                return false;
            }
        }
    }

    /*
     * If no membership created a conflict, the assignment is allowed.
     */
    return true;
}