#pragma once

#include <QObject>
#include <QString>
#include <vector>
#include "domain/ScheduleGenerationResult.h"

/*
 * SchedulingWorker is the object that actually runs the scheduling algorithm
 * inside a background thread.
 *
 * It does not own the scheduling logic itself.
 * It receives a ScheduleGenerator pointer and calls it from run().
 */

class ScheduleGenerator;

/*
 * SchedulingWorker is separated from SchedulingService because Qt recommends
 * using a worker object when running long operations on a QThread.
 *
 * This keeps the UI responsive while the backtracking algorithm is running.
 */
class SchedulingWorker : public QObject {
    Q_OBJECT

public:
    /*
     * Creates a worker for one scheduling operation.
     *
     * generator points to the ScheduleGenerator that should be executed.
     * limitPerBlock controls how many solutions should be generated.
     */
    SchedulingWorker(const ScheduleGenerator* generator, int limitPerBlock);

public slots:
    /*
     * Runs the scheduling algorithm.
     *
     * This slot is connected to QThread::started, so it executes inside
     * the worker thread instead of the UI thread.
     */
    void run();

signals:
    /*
     * Emitted when schedule generation completes successfully.
     *
     * The generated solutions are sent back to the service/controller layer.
     */
    void finished(const std::vector<ScheduleGenerationResult>& solutions);

    /*
     * Emitted when schedule generation fails.
     *
     * The message is suitable for displaying to the user or logging.
     */
    void failed(QString message);

private:
    /*
     * Pointer to the generator used by this worker.
     *
     * The worker does not create the generator.
     * Lifetime is managed by SchedulingService.
     */
    const ScheduleGenerator* generator_;

    /*
     * Maximum number of solutions to generate for the scheduling block.
     */
    int limitPerBlock_;
};