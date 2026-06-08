#include "scheduler/SchedulingWorker.h"
#include "scheduler/ScheduleGenerator.h"

#include <exception>

SchedulingWorker::SchedulingWorker(const ScheduleGenerator* generator, int limitPerBlock)
    : generator_(generator),
      limitPerBlock_(limitPerBlock) {
}

void SchedulingWorker::run() {
    try {
        std::vector<std::vector<int>> solutions =
            generator_->runBacktracking(limitPerBlock_);

        emit finished(solutions);
    } catch (const std::exception& ex) {
        emit failed(QString::fromStdString(ex.what()));
    } catch (...) {
        emit failed("Unknown scheduling error occurred.");
    }
}