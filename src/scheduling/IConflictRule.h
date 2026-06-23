#pragma once

#include "scheduling/Preprocessor.h"
#include "scheduling/IReadOnlySchedule.h"

/*
 * IConflictRule defines the common interface for all scheduling constraints.
 *
 * The scheduling engine does not need to know the internal logic of each rule.
 * It only asks each rule whether a proposed assignment is allowed.
 *
 * This makes the scheduler easier to extend:
 * new rules can be added without rewriting the main backtracking algorithm.
 */

/**
 * @brief Interface for scheduling conflict rules.
 *
 * Each rule checks whether assigning a runtime course to a specific date
 * is allowed under the current partial scheduling state.
 */
class IConflictRule {
public:
    /*
     * Virtual destructor allows derived rule objects to be deleted safely
     * through an IConflictRule pointer.
     */
    virtual ~IConflictRule() = default;

    /*
     * Checks whether a specific course can be assigned to a specific date.
     *
     * Parameters:
     * - schedule: the abstract read-only interface of the current schedule.     
     * - course: the runtime course the solver wants to place.
     * - dateIndex: the candidate date index inside the scheduling block.
     *
     * Returns true if the assignment is allowed by this rule.
     */
    virtual bool isSatisfied(const IReadOnlySchedule& schedule,
                             const RuntimeCourse& course,
                             int dateIndex) const = 0;
};