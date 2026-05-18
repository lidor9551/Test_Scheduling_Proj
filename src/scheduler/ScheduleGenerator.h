#pragma once

#include "../preprocessing/Preprocessor.h"
#include <stdexcept>
#include <vector>
#include <string>

// Exception thrown if the solver exceeds the maximum runtime limit
class SolverTimeoutException : public std::runtime_error {
public:
    explicit SolverTimeoutException(const std::string& message);
};

// Internal solver state passed between recursive calls
struct SchedulerState {
    std::vector<int> assignedDate;
    std::vector<std::vector<int>> obligatoryCount;
    std::vector<std::vector<int>> electiveCount;
};

class ScheduleGenerator {
private:
    SchedulingBlock block_;
    int groupCount_;
    double maxRuntimeSeconds_;

    int computeGroupCount() const;
    bool canAssign(const SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    void assign(SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    void unassign(SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    int datePressure(const SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    
    // Heuristic: LCV (Least Constraining Value) - orders dates by lowest impact on other exams
    std::vector<int> orderedCandidateDates(const SchedulerState& state, const RuntimeCourse& course) const;
    
    // Heuristic: MRV (Minimum Remaining Values) - selects the course with the fewest available dates
    int selectNextCourse(const SchedulerState& state, const std::vector<bool>& remaining) const;

public:
    explicit ScheduleGenerator(SchedulingBlock block, double maxRuntimeSeconds = 30.0);

    // Executes the algorithm and returns all valid assignments (up to the limit)
    std::vector<std::vector<int>> generateAll(int limitPerBlock = -1) const;
};