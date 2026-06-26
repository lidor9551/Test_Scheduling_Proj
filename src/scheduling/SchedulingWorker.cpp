#include "scheduling/SchedulingWorker.h"
#include "scheduling/ScheduleGenerator.h"

#include <exception>

/*
 * Stores the generator and the solution limit for this worker.
 *
 * The actual scheduling work is not started in the constructor.
 * It starts only when run() is called by the worker thread.
 */
SchedulingWorker::SchedulingWorker(const ScheduleGenerator* generator, int limitPerBlock)
    : generator_(generator),
      limitPerBlock_(limitPerBlock),
      cancellationRequested_(false) {
}

void SchedulingWorker::requestCancellation() {
    cancellationRequested_.store(true);
}

/*
 * Executes the scheduling algorithm and reports the result through Qt signals.
 *
 * Any exception thrown by the solver is caught here so it does not crash
 * the worker thread or the application.
 */
void SchedulingWorker::run() {
    try {
        /*
         * Run the backtracking solver using the configured solution limit.
         */
        std::vector<ScheduleGenerationResult> solutions =
            generator_->runBacktracking(limitPerBlock_, [this]() {
                return cancellationRequested_.load();
            });

        /*
         * Send successful results back to the scheduling service.
         */
        emit finished(solutions);
    } catch (const std::exception& ex) {
        /*
         * Convert standard C++ exceptions into a QString error message.
         */
        emit failed(QString::fromStdString(ex.what()));
    } catch (...) {
        /*
         * Catch any non-standard exception as a final safety net.
         */
        emit failed("Unknown scheduling error occurred.");
    }
}
