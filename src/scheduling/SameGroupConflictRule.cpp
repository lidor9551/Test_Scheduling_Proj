#include "scheduling/SameGroupConflictRule.h"

/*
 * Checks whether a course can be placed on the requested date
 * without creating an illegal same-group conflict.
 *
 * The method loops over all memberships of the course because one course can
 * belong to more than one academic program/year group.
 */
bool SameGroupConflictRule::isSatisfied(const IReadOnlySchedule& schedule, // Using abstract interface instead of concrete state
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
            // Fetching from the abstract interface in O(1)
            if (schedule.getObligatoryCount(groupId, dateIndex) > 0) {
                return false;
            }

            // Fetching from the abstract interface in O(1)
            if (schedule.getElectiveCount(groupId, dateIndex) > 0) {
                return false;
            }
        } else {
            /*
             * Elective courses are more flexible:
             * they may overlap with other elective courses,
             * but they cannot overlap with an obligatory course
             * from the same academic group.
             */
            if (schedule.getObligatoryCount(groupId, dateIndex) > 0) {
                return false;
            }
        }
    }

    /*
     * If no membership created a conflict, the assignment is allowed.
     */
    return true;
}