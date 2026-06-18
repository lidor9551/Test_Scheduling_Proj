#pragma once

#include "scheduling/Preprocessor.h"
#include "scheduling/IConflictRule.h"
#include "scheduling/SchedulingState.h"
#include "domain/ScheduleGenerationResult.h" 
#include "application/SchedulingSession.h" 
#include "scheduling/AdvancedConflictRules.h"
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

/*
 * ScheduleGenerator is the core backtracking solver.
 *
 * It receives one SchedulingBlock and tries to generate valid exam schedules
 * for that block. The generator does not know about QML screens or user input.
 * It works only with scheduling data prepared by the preprocessor.
 */

/*
 * SolverTimeoutException is thrown when the solver exceeds its allowed runtime.
 *
 * This protects the application from very long scheduling runs.
 */
class SolverTimeoutException : public std::runtime_error {
public:
    // Creates a timeout exception with a readable error message.
    explicit SolverTimeoutException(const std::string& message);
};

/*
 * ScheduleGenerator runs the constraint-satisfaction algorithm.
 *
 * Main responsibilities:
 * - keep the SchedulingBlock being solved
 * - maintain default conflict rules
 * - run recursive backtracking
 * - return domain-level ScheduleGenerationResult objects
 */
class ScheduleGenerator {
public:
    /*
     * Creates a generator for one scheduling block.
     *
     * maxRuntimeSeconds defines how long the solver may run before timing out.
     */
    ScheduleGenerator(SchedulingBlock block, const ScheduleSettings& settings, double maxRuntimeSeconds = 30.0);

    
    /*
     * Starts the backtracking algorithm.
     *
     * limitPerBlock limits the number of solutions returned for this block.
     * A negative value means there is no explicit solution limit.
     */
    std::vector<ScheduleGenerationResult> runBacktracking(int limitPerBlock = -1) const;

private:

    /*
     * Computes how many academic groups exist in the current block.
     *
     * This is used to size the conflict-count matrices inside SchedulingState.
     */
    int computeGroupCount() const;

    /*
     * Checks whether the given course can be assigned to the given date.
     *
     * The method delegates the actual rule checking to the configured conflict rules.
     */
    bool canAssign(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const;

    /*
     * Applies a course-date assignment to the mutable scheduling state.
     *
     * This updates both the assigned date and the relevant conflict counters.
     */
    void assign(SchedulingState& state, const RuntimeCourse& course, int dateIndex) const;

    /*
     * Reverts a previous course-date assignment.
     *
     * This is required by backtracking when the solver returns from recursion.
     */
    void unassign(SchedulingState& state, const RuntimeCourse& course, int dateIndex) const;

    /*
     * Calculates how crowded a date already is for the course's groups.
     *
     * Lower pressure is preferred when ordering candidate dates.
     */
    int datePressure(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const;

    /*
     * Returns all valid date candidates for a course.
     *
     * The candidates are sorted so the solver tries less constrained dates first.
     */
    std::vector<int> orderedCandidateDates(const SchedulingState& state, const RuntimeCourse& course) const;

    /*
     * Chooses the next course to assign.
     *
     * This method implements the MRV-style heuristic:
     * prefer the course with the fewest legal dates remaining.
     */
    int selectNextCourse(const SchedulingState& state, const std::vector<bool>& remaining) const;
    
    /*
     * Internal implementation of the full backtracking search.
     *
     * This method creates the initial state and runs the recursive solver.
     */
    std::vector<ScheduleGenerationResult> generateAll(int limitPerBlock) const;

    // The scheduling block currently being solved.
    SchedulingBlock block_;

    // Number of academic groups used by the conflict-count matrices.
    int groupCount_;

    // Maximum allowed runtime for the solver, in seconds.
    double maxRuntimeSeconds_;

    // Conflict rules applied before assigning a course to a date.
    std::vector<std::shared_ptr<IConflictRule>> conflictRules_;

    // Creates the default rule set used by the generator.
    static std::vector<std::shared_ptr<IConflictRule>> createDefaultConflictRules();
};