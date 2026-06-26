#include "ScheduleGenerator.h"
#include "scheduling/SameGroupConflictRule.h"
#include "scheduling/AdvancedConflictRules.h"
#include "scheduling/MetricsCalculator.h"
#include <memory>
#include <algorithm>
#include <chrono>
#include <functional>
#include <limits>

/*
 * Creates a timeout exception.
 *
 * This exception is used to stop the recursive solver when it runs longer
 * than the configured time limit.
 */
SolverTimeoutException::SolverTimeoutException(const std::string& message)
    : std::runtime_error(message) {}

/*
 * Creates a cancellation exception.
 */
SolverCancelledException::SolverCancelledException(const std::string& message)
    : std::runtime_error(message) {}

/*
 * Creates a ScheduleGenerator for one scheduling block.
 *
 * The generator stores:
 * - the block to solve
 * - the number of academic groups
 * - the maximum runtime
 * - the default conflict rules
 */
ScheduleGenerator::ScheduleGenerator(SchedulingBlock block, const ScheduleSettings& settings, double maxRuntimeSeconds)
    : block_(std::move(block)),
      groupCount_(computeGroupCount()),
      maxRuntimeSeconds_(maxRuntimeSeconds), 
      conflictRules_(createDefaultConflictRules()) {

    // 2.1 minimum days between obligatory exams
    if (settings.minDaysObligatory.isActive) {
        conflictRules_.push_back(std::make_shared<MinDaysObligatoryRule>(
            settings.minDaysObligatory.k, block_.allowedDates, block_.runtimeCourses));
    }

    // 2.2 minimum days between all exams
    if (settings.minDaysAll.isActive) {
        conflictRules_.push_back(std::make_shared<MinDaysAllRule>(
            settings.minDaysAll.k, block_.allowedDates, block_.runtimeCourses));
    }

    // 2.3 maximum conflicts for electives
    if (settings.maxElectiveConflicts.isActive) {
        conflictRules_.push_back(std::make_shared<MaxElectiveConflictsRule>(
            settings.maxElectiveConflicts.k, block_.runtimeCourses));
    }

    // 2.4 time frame for obligatory exams (the filter that runs at the end of each schedule)
    if (settings.obligatorySpan.isActive) {
        conflictRules_.push_back(std::make_shared<ObligatorySpanRule>(
            settings.obligatorySpan.k, block_.allowedDates, block_.runtimeCourses));
    }

    // 2.5 maximum exams per day
    if (settings.maxExamsPerDay.isActive) {
        conflictRules_.push_back(std::make_shared<MaxExamsPerDayRule>(
            settings.maxExamsPerDay.k));
    }
}

/*
 * Public entry point for generating schedules.
 *
 * The actual recursive implementation is kept inside generateAll().
 */
std::vector<ScheduleGenerationResult> ScheduleGenerator::runBacktracking(
    int limitPerBlock,
    const std::function<bool()>& shouldCancel
) const {
    return generateAll(limitPerBlock, shouldCancel);
}

/*
 * Creates the default conflict rule list.
 *
 * Currently the solver uses the same-group rule, which protects students
 * from illegal exam conflicts within the same academic program/year group.
 */
std::vector<std::shared_ptr<IConflictRule>> ScheduleGenerator::createDefaultConflictRules() {
    return {
        std::make_shared<SameGroupConflictRule>()
    };
}

/*
 * Finds the number of academic groups used in this scheduling block.
 *
 * Group IDs are zero-based, so the total count is maxGroup + 1.
 */
int ScheduleGenerator::computeGroupCount() const {
    int maxGroup = -1;
    for (const RuntimeCourse& course : block_.runtimeCourses) {
        for (const CourseMembership& membership : course.memberships) {
            maxGroup = std::max(maxGroup, membership.groupId);
        }
    }
    return maxGroup + 1;
}

/*
 * Checks whether assigning a course to a date satisfies all conflict rules.
 *
 * The generator itself does not hardcode the conflict logic here.
 * It simply asks every rule whether the assignment is legal.
 */
bool ScheduleGenerator::canAssign(const SchedulingState& state,
                                  const RuntimeCourse& course,
                                  int dateIndex) const {
    for (const std::shared_ptr<IConflictRule>& rule : conflictRules_) {
        if (!rule->isSatisfied(state, course, dateIndex)) {
            return false;
        }
    }

    return true;
}

/*
 * Assigns a runtime course to a date inside the current state.
 *
 * This method updates:
 * - assignedDate for the course
 * - obligatory counters for obligatory memberships
 * - elective counters for elective memberships
 * - total exam counter for the assigned date
 */
void ScheduleGenerator::assign(SchedulingState& state, const RuntimeCourse& course, int dateIndex) const {
    state.assignedDate[course.id] = dateIndex;
    state.totalExamsPerDate[dateIndex]++;
    for (const CourseMembership& membership : course.memberships) {
        if (membership.requirement == Requirement::OBLIGATORY) {
            state.obligatoryCount[membership.groupId][dateIndex]++;
        } else {
            state.electiveCount[membership.groupId][dateIndex]++;
        }
    }
}

/*
 * Removes a previous assignment from the current state.
 *
 * This is the "undo" operation used by the backtracking algorithm.
 */
void ScheduleGenerator::unassign(SchedulingState& state, const RuntimeCourse& course, int dateIndex) const {
    state.assignedDate[course.id] = -1;
    state.totalExamsPerDate[dateIndex]--;
    for (const CourseMembership& membership : course.memberships) {
        if (membership.requirement == Requirement::OBLIGATORY) {
            state.obligatoryCount[membership.groupId][dateIndex]--;
        } else {
            state.electiveCount[membership.groupId][dateIndex]--;
        }
    }
}

/*
 * Calculates how much activity already exists on a candidate date
 * for all groups that take this course.
 *
 * Lower pressure means the date is less crowded for the relevant groups.
 */
int ScheduleGenerator::datePressure(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const {
    int pressure = 0;
    for (const CourseMembership& membership : course.memberships) {
        pressure += state.obligatoryCount[membership.groupId][dateIndex];
        pressure += state.electiveCount[membership.groupId][dateIndex];
    }
    return pressure;
}

/*
 * Builds and orders the list of legal date candidates for one course.
 *
 * First, the method filters out dates that violate conflict rules.
 * Then it sorts the remaining dates by pressure, so the solver tries
 * less crowded dates before more crowded dates.
 */
std::vector<int> ScheduleGenerator::orderedCandidateDates(const SchedulingState& state, const RuntimeCourse& course) const {
    std::vector<int> candidates;
    for (int dateIndex = 0; dateIndex < static_cast<int>(block_.allowedDates.size()); ++dateIndex) {
        if (canAssign(state, course, dateIndex)) {
            candidates.push_back(dateIndex);
        }
    }

    /*
     * Sort candidates by least pressure first.
     *
     * If two dates have the same pressure, the earlier date is tried first.
     */
    std::sort(candidates.begin(), candidates.end(), [&](int left, int right) {
        int leftPressure = datePressure(state, course, left);
        int rightPressure = datePressure(state, course, right);
        if (leftPressure != rightPressure) {
            return leftPressure < rightPressure;
        }
        return block_.allowedDates[left] < block_.allowedDates[right];
    });

    return candidates;
}

/*
 * Selects the next course to assign during backtracking.
 *
 * The method uses a heuristic similar to MRV:
 * choose the course with the fewest currently legal dates.
 *
 * Tie breakers:
 * - obligatory courses before elective-only courses
 * - courses with more memberships before courses with fewer memberships
 * - lower course number for stable deterministic behavior
 */
int ScheduleGenerator::selectNextCourse(const SchedulingState& state, const std::vector<bool>& remaining) const {
    int bestIndex = -1;
    int bestDomainSize = std::numeric_limits<int>::max();
    int bestHasOnlyElective = std::numeric_limits<int>::max();
    int bestMembershipScore = std::numeric_limits<int>::min();
    std::string bestCourseNumber;

    for (int i = 0; i < static_cast<int>(remaining.size()); ++i) {
        if (!remaining[i]) {
            continue;
        }

        const RuntimeCourse& course = block_.runtimeCourses[i];

        /*
         * Count how many legal dates are still available for this course.
         */
        int domainSize = 0;
        for (int dateIndex = 0; dateIndex < static_cast<int>(block_.allowedDates.size()); ++dateIndex) {
            if (canAssign(state, course, dateIndex)) {
                domainSize++;
            }
        }

        /*
         * Check whether this course is obligatory in at least one membership.
         */
        bool hasObligatory = false;
        for (const CourseMembership& membership : course.memberships) {
            if (membership.requirement == Requirement::OBLIGATORY) {
                hasObligatory = true;
                break;
            }
        }

        int onlyElectiveFlag = hasObligatory ? 0 : 1;
        int membershipScore = -static_cast<int>(course.memberships.size());
        
        // Using the new getter from the OOP Model
        const std::string& number = course.course->getCourseNumber();

        /*
         * Decide whether the current course is a better next choice
         * than the best course found so far.
         */
        bool better = false;
        if (domainSize < bestDomainSize) {
            better = true;
        } else if (domainSize == bestDomainSize && onlyElectiveFlag < bestHasOnlyElective) {
            better = true;
        } else if (domainSize == bestDomainSize &&
                   onlyElectiveFlag == bestHasOnlyElective &&
                   membershipScore < bestMembershipScore) {
            better = true;
        } else if (domainSize == bestDomainSize &&
                   onlyElectiveFlag == bestHasOnlyElective &&
                   membershipScore == bestMembershipScore &&
                   (bestIndex == -1 || number < bestCourseNumber)) {
            better = true;
        }

        if (better) {
            bestIndex = i;
            bestDomainSize = domainSize;
            bestHasOnlyElective = onlyElectiveFlag;
            bestMembershipScore = membershipScore;
            bestCourseNumber = number;
        }
    }

    return bestIndex;
}

/*
 * Generates all valid schedules for the current block.
 *
 * This method creates the initial solver state and defines the recursive
 * backtracking function that explores all legal assignments.
 */
std::vector<ScheduleGenerationResult> ScheduleGenerator::generateAll(
    int limitPerBlock,
    const std::function<bool()>& shouldCancel
) const {
    /*
     * Initial scheduling state:
     * - every course starts unassigned with date index -1
     * - all group/date counters start at zero
     */
    SchedulingState state{
        std::vector<int>(block_.runtimeCourses.size(), -1),
        std::vector<std::vector<int>>(groupCount_, std::vector<int>(block_.allowedDates.size(), 0)),
        std::vector<std::vector<int>>(groupCount_, std::vector<int>(block_.allowedDates.size(), 0)),
        std::vector<int>(block_.allowedDates.size(), 0)
    };

    /*
     * remaining[i] tells whether runtimeCourses[i] still needs an assigned date.
     */
    std::vector<bool> remaining(block_.runtimeCourses.size(), true);

    /*
     * All valid solutions found by the recursive search.
     */
    std::vector<ScheduleGenerationResult> solutions;

    /*
     * The start time is used to enforce the runtime timeout.
     */
    const auto startTime = std::chrono::steady_clock::now();

    // Define the internal recursive backtracking function
    /*
     * Recursive backtracking function.
     *
     * It assigns one course at a time, continues recursively,
     * and then undoes the assignment before trying the next option.
     */
    std::function<void()> backtrack = [&]() {
        if (shouldCancel && shouldCancel()) {
            throw SolverCancelledException("Scheduling was cancelled.");
        }

        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - startTime).count();
        
        // Stop if we exceeded the time limit (e.g., 30 seconds)
        /*
         * Stop the solver if it passed the maximum allowed runtime.
         */
        if (elapsed > maxRuntimeSeconds_) {
            throw SolverTimeoutException(
                "Solver exceeded " + std::to_string(maxRuntimeSeconds_) +
                " seconds for block " + block_.semester + "/" + block_.moed
            );
        }

        /*
         * Stop exploring if the requested solution limit was already reached.
         */
        if (limitPerBlock >= 0 && static_cast<int>(solutions.size()) >= limitPerBlock) {
            return;
        }

        /*
         * Check whether all courses have already been assigned.
         */
        bool done = true;
        for (bool flag : remaining) {
            if (flag) {
                done = false;
                break;
            }
        }

        /*
         * When no courses remain, the current state is a complete solution.
         */
        if (done) {
            // We've found a complete solution. Let's convert it to the new ScheduleGenerationResult format.
            /*
             * Convert the internal date-index representation into domain assignments.
             */
            std::vector<ExamAssignment> assignments;
            
            for (size_t i = 0; i < state.assignedDate.size(); ++i) {
                int dateIndex = state.assignedDate[i];
                if (dateIndex == -1) continue; 

                const RuntimeCourse& runtimeCourse = block_.runtimeCourses[i];
                
                // Determine if the course is obligatory or elective based on its memberships
                /*
                 * A course is considered obligatory in the result if any of its
                 * relevant memberships is obligatory.
                 */
                bool isObligatory = false;
                for (const CourseMembership& membership : runtimeCourse.memberships) {
                    if (membership.requirement == Requirement::OBLIGATORY) {
                        isObligatory = true;
                        break;
                    }
                }

                // Create an ExamAssignment for this course and add it to the list of assignments for this solution
                /*
                 * Store the original course pointer together with the selected exam date.
                 */
                assignments.push_back({
                    runtimeCourse.course,                // Pointer to the original Course object
                    block_.allowedDates[dateIndex],      // The exact date
                    isObligatory                         // Obligatory or elective
                });
            }

            // Save the final cleaned result
            /*
             * Save this complete schedule as a ScheduleGenerationResult.
             */
            solutions.emplace_back(ScheduleGenerationResult(assignments));
            return;
        }

        /*
         * Pick the next course using the selection heuristic.
         */
        int nextCourseIndex = selectNextCourse(state, remaining);
        remaining[nextCourseIndex] = false;

        /*
         * Try every legal date candidate for the selected course.
         */
        std::vector<int> candidates = orderedCandidateDates(state, block_.runtimeCourses[nextCourseIndex]);
        for (int dateIndex : candidates) {
            assign(state, block_.runtimeCourses[nextCourseIndex], dateIndex);
            backtrack();
            unassign(state, block_.runtimeCourses[nextCourseIndex], dateIndex);

            /*
             * Stop the loop early if the solution limit was reached during recursion.
             */
            if (limitPerBlock >= 0 && static_cast<int>(solutions.size()) >= limitPerBlock) {
                break;
            }
        }

        /*
         * Mark the course as remaining again before returning to the previous level.
         */
        remaining[nextCourseIndex] = true;
    };

    /*
     * Start the recursive search.
     */
    backtrack();

    // POST-PROCESSING: Calculate metrics for all valid schedules
    /*
     * Now that the algorithm has found all valid solutions, we evaluate them
     * using the soft constraints to generate a score (Metrics) for each.
     * This avoids slowing down the backtracking process itself.
     */
    for (auto& result : solutions) {
        // We will create the MetricsCalculator class next to handle this logic
        result.metrics = MetricsCalculator::calculate(result, block_); 
    }
    /*
     * Return all solutions found for this block.
     */
    return solutions;
}
