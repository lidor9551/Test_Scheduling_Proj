#pragma once

#include "scheduling/Preprocessor.h"
#include "domain/ScheduleGenerationResult.h" 
#include <vector>
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
    struct SchedulerState {
        std::vector<int> assignedDate; 
        std::vector<std::vector<int>> obligatoryCount; 
        std::vector<std::vector<int>> electiveCount;   
    };

    int computeGroupCount() const;
    bool canAssign(const SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    void assign(SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    void unassign(SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    int datePressure(const SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    std::vector<int> orderedCandidateDates(const SchedulerState& state, const RuntimeCourse& course) const;
    int selectNextCourse(const SchedulerState& state, const std::vector<bool>& remaining) const;
    
    std::vector<ScheduleGenerationResult> generateAll(int limitPerBlock) const;

    SchedulingBlock block_;
    int groupCount_;
    double maxRuntimeSeconds_;
};