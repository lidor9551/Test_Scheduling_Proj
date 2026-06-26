#pragma once

#include "scheduling/IConflictRule.h"
#include "scheduling/IReadOnlySchedule.h"


/*
 * SameGroupConflictRule implements the main same-day conflict logic.
 *
 * The rule checks whether students from the same academic group would receive
 * conflicting exams on the same date.
 *
 * An academic group is usually defined by a pair such as:
 * program ID + study year.
 */

/**
 * @brief Prevents invalid same-group exam conflicts.
 *
 * Rules:
 * - Obligatory exams cannot overlap with any obligatory or elective exam
 *   for the same academic group.
 * - Elective exams cannot overlap with an obligatory exam for the same group.
 * - Elective exams may overlap with other elective exams for the same group.
 */
class SameGroupConflictRule : public IConflictRule {
public:
    /*
     * Returns true if placing the given course on the given date does not
     * violate same-group exam conflict rules.
     */
    bool isSatisfied(const IReadOnlySchedule& schedule,
                     const RuntimeCourse& course,
                     int dateIndex) const override;
};