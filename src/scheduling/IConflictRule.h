#pragma once

#include "scheduling/Preprocessor.h"
#include "scheduling/SchedulingState.h"

/**
 * @brief Interface for scheduling conflict rules.
 *
 * Each rule checks whether assigning a runtime course to a specific date
 * is allowed under the current partial scheduling state.
 */
class IConflictRule {
public:
    virtual ~IConflictRule() = default;

    virtual bool isSatisfied(const SchedulingState& state,
                             const RuntimeCourse& course,
                             int dateIndex) const = 0;
};