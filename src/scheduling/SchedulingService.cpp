#include "SchedulingService.h"
#include "ScheduleGenerator.h"
#include "SchedulingWorker.h"

#include <QThread>
#include <QDebug>

/*
 * Initializes the service in an idle state.
 */
SchedulingService::SchedulingService(QObject* parent)
    : QObject(parent), m_isRunning(false) {
}

/*
 * Destructor for the scheduling service.
 *
 * This is the right place for future cleanup logic if the service ever owns
 * long-living resources.
 */
SchedulingService::~SchedulingService() {
    // Destructor: Ensure that if the service is destroyed while a scheduling operation is running, we handle it gracefully.
}

/*
 * Starts asynchronous schedule generation.
 *
 * This method creates:
 * - a new QThread
 * - a ScheduleGenerator for the given block
 * - a SchedulingWorker that runs the generator inside the thread
 */
void SchedulingService::startAsyncGeneration(const SchedulingBlock& block, int limitPerBlock) {
    // Safety check to prevent multiple concurrent scheduling operations
    /*
     * Reject a new request if a previous scheduling operation is still running.
     */
    if (m_isRunning) {
        emit generationFailed("Scheduling is already running. Please wait.");
        return;
    }

    /*
     * Mark the service as busy before creating the worker thread.
     */
    m_isRunning = true;

    // create a new QThread and a new ScheduleGenerator for this operation
    /*
     * Create the thread, generator, and worker for this single operation.
     *
     * The generator performs the actual algorithm.
     * The worker is responsible for running it inside the QThread.
     */
    auto* thread = new QThread();
    auto* generator = new ScheduleGenerator(block); // המנוע הטהור החדש שניקינו
    auto* worker = new SchedulingWorker(generator, limitPerBlock); // השליח שיודע להחזיר ScheduleGenerationResult

    // Move the worker to the new thread
    /*
     * Move the worker object to the background thread.
     *
     * After this, worker slots connected to thread signals will run
     * outside the UI thread.
     */
    worker->moveToThread(thread);

    // Connect signals and slots for proper cleanup and result handling
    // When the thread starts, invoke the worker's run method
    /*
     * Start the worker when the QThread begins execution.
     */
    connect(thread, &QThread::started, worker, &SchedulingWorker::run);

    // When the worker finishes successfully
    /*
     * Handle successful generation.
     *
     * The service resets its running flag, emits the result,
     * then shuts down and cleans the worker resources.
     */
    connect(worker, &SchedulingWorker::finished, this, [this, thread, worker, generator](const std::vector<ScheduleGenerationResult>& solutions) {
        m_isRunning = false;
        
        // Emit the successful result back to the AppController
        emit generationFinished(solutions);
        
        // Cleanup: Stop the thread and delete the worker and generator
        thread->quit();          
        worker->deleteLater();  
        delete generator;       
    });

    // When the worker fails with an error
    /*
     * Handle generation failure.
     *
     * The service resets its running flag, emits the error message,
     * then shuts down and cleans the worker resources.
     */
    connect(worker, &SchedulingWorker::failed, this, [this, thread, worker, generator](QString message) {
        m_isRunning = false;
        
        emit generationFailed(message);
        
        thread->quit();
        worker->deleteLater();
        delete generator;
    });

    // Ensure the thread is deleted when it finishes
    /*
     * Delete the QThread object after it finishes.
     */
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    /*
     * Start the background thread.
     *
     * This triggers QThread::started, which then calls SchedulingWorker::run.
     */
    thread->start();
}