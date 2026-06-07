#pragma once

#include "../preprocessing/Preprocessor.h"

#include <QObject>
#include <QString>
#include <stdexcept>
#include <string>
#include <vector>

class QThread;

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

    QThread* workerThread_ = nullptr;

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
    ~ScheduleGenerator() override;

    std::vector<std::vector<int>> runBacktracking(int limitPerBlock) const;

    Q_INVOKABLE void startScheduling(int limitPerBlock = -1);

signals:
    void schedulingFinished(const std::vector<std::vector<int>>& solutions);
    void errorOccurred(QString message);
};