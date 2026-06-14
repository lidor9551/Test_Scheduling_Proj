#pragma once

#include <vector>

/*
 * SchedulingState is the mutable working memory of the backtracking algorithm.
 *
 * It represents the current partial schedule while the solver is running.
 * The solver changes this state when it assigns or removes courses from dates.
 *
 * Conflict rules read this state to decide whether a new assignment is legal.
 */

/**
 * @brief Mutable state used by the backtracking scheduler.
 *
 * The state is separated from ScheduleGenerator so conflict rules can inspect
 * the current partial schedule without depending on ScheduleGenerator internals.
 */
struct SchedulingState {
    /*
     * Stores the assigned date index for each runtime course.
     *
     * The vector index represents the course ID.
     * The stored value represents the selected date index.
     * A value of -1 usually means the course has not been assigned yet.
     */
    std::vector<int> assignedDate;

    /*
     * Counts obligatory exams per academic group and per date.
     *
     * First index: group ID.
     * Second index: date index.
     *
     * This allows conflict rules to quickly check whether a group already has
     * an obligatory exam on a specific date.
     */
    std::vector<std::vector<int>> obligatoryCount;

    /*
     * Counts elective exams per academic group and per date.
     *
     * First index: group ID.
     * Second index: date index.
     *
     * This is used together with obligatoryCount to enforce the conflict rules
     * between obligatory and elective exams.
     */
    std::vector<std::vector<int>> electiveCount;
};