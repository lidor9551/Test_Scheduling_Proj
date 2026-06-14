#pragma once

#include "scheduling/IConflictRule.h"

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
    bool isSatisfied(const SchedulingState& state,
                     const RuntimeCourse& course,
                     int dateIndex) const override;
};