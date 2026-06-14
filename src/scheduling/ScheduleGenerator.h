#pragma once

#include "scheduling/Preprocessor.h"
#include "scheduling/IConflictRule.h"
#include "scheduling/SchedulingState.h"
#include "domain/ScheduleGenerationResult.h" 
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

class SolverTimeoutException : public std::runtime_error {
public:
    explicit SolverTimeoutException(const std::string& message);
};

class ScheduleGenerator {
public:
    ScheduleGenerator(SchedulingBlock block, double maxRuntimeSeconds = 30.0);

    
    std::vector<ScheduleGenerationResult> runBacktracking(int limitPerBlock = -1) const;

private:

    int computeGroupCount() const;
    bool canAssign(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const;
    void assign(SchedulingState& state, const RuntimeCourse& course, int dateIndex) const;
    void unassign(SchedulingState& state, const RuntimeCourse& course, int dateIndex) const;
    int datePressure(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const;
    std::vector<int> orderedCandidateDates(const SchedulingState& state, const RuntimeCourse& course) const;
    int selectNextCourse(const SchedulingState& state, const std::vector<bool>& remaining) const;
    
    std::vector<ScheduleGenerationResult> generateAll(int limitPerBlock) const;

    SchedulingBlock block_;
    int groupCount_;
    double maxRuntimeSeconds_;
    std::vector<std::shared_ptr<IConflictRule>> conflictRules_;

    static std::vector<std::shared_ptr<IConflictRule>> createDefaultConflictRules();
};