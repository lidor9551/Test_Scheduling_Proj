#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Domain.h"
#include <stdexcept>
#include <vector>

class SolverTimeoutException : public std::runtime_error {
public:
    explicit SolverTimeoutException(const std::string& message);
};

struct SchedulerState {
    std::vector<int> assignedDate;
    std::vector<std::vector<int>> obligatoryCount;
    std::vector<std::vector<int>> electiveCount;
};

class ExamScheduler {
private:
    SchedulingBlock block_;
    int groupCount_;
    double maxRuntimeSeconds_;

    int computeGroupCount() const;
    bool canAssign(const SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    void assign(SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    void unassign(SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    int datePressure(const SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    std::vector<int> orderedCandidateDates(const SchedulerState& state, const RuntimeCourse& course) const;
    int selectNextCourse(const SchedulerState& state, const std::vector<bool>& remaining) const;

public:
    explicit ExamScheduler(SchedulingBlock block, double maxRuntimeSeconds = 30.0);

    std::vector<std::vector<int>> generateAll(int limitPerBlock = -1) const;
};

#endif
