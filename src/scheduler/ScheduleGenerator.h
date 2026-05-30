#pragma once

#include "../preprocessing/Preprocessor.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <QObject>
#include <QString>

class SolverTimeoutException : public std::runtime_error {
public:
    explicit SolverTimeoutException(const std::string& message);
};

struct SchedulerState {
    std::vector<int> assignedDate;
    std::vector<std::vector<int>> obligatoryCount;
    std::vector<std::vector<int>> electiveCount;
};

class ScheduleGenerator : public QObject {
    Q_OBJECT

private:
    SchedulingBlock block_;
    int groupCount_;
    double maxRuntimeSeconds_;
    std::vector<std::vector<int>> cachedSolutions_;
    bool hasCachedResult_ = false;

    int computeGroupCount() const;
    bool canAssign(const SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    void assign(SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    void unassign(SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    int datePressure(const SchedulerState& state, const RuntimeCourse& course, int dateIndex) const;
    std::vector<int> orderedCandidateDates(const SchedulerState& state, const RuntimeCourse& course) const;
    int selectNextCourse(const SchedulerState& state, const std::vector<bool>& remaining) const;
    std::vector<std::vector<int>> generateAll(int limitPerBlock) const;

public:
    explicit ScheduleGenerator(SchedulingBlock block, double maxRuntimeSeconds = 30.0);

    Q_INVOKABLE void startScheduling(int limitPerBlock = -1);

signals:
    void schedulingFinished(const std::vector<std::vector<int>>& solutions);
    void errorOccurred(QString message);
};