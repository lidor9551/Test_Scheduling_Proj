#pragma once

#include <vector>

/**
 * @brief Mutable state used by the backtracking scheduler.
 *
 * The state is separated from ScheduleGenerator so conflict rules can inspect
 * the current partial schedule without depending on ScheduleGenerator internals.
 */
struct SchedulingState {
    std::vector<int> assignedDate;
    std::vector<std::vector<int>> obligatoryCount;
    std::vector<std::vector<int>> electiveCount;
};