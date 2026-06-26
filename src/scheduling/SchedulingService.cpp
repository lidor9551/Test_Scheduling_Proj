#include "SchedulingService.h"
#include "ScheduleGenerator.h"
#include "SchedulingWorker.h"

#include <QMetaType>
#include <QThread>
#include <exception>
#include <memory>

/*
 * Initializes the service in an idle state.
 */
SchedulingService::SchedulingService(QObject* parent)
    : QObject(parent) {
    qRegisterMetaType<std::vector<ScheduleGenerationResult>>("std::vector<ScheduleGenerationResult>");
}

/*
 * Destructor for the scheduling service.
 *
 * This is the right place for future cleanup logic if the service ever owns
 * long-living resources.
 */
SchedulingService::~SchedulingService() {
    shutdownActiveJob();
}

/*
 * Marks the active job as running after its QThread has started.
 */
void SchedulingService::markActiveJobRunning(QThread* expectedThread) {
    if (!m_activeJob.has_value()) {
        return;
    }

    if (m_activeJob->thread != expectedThread) {
        return;
    }

    m_activeJob->state = ActiveSchedulingJob::State::Running;
}

/*
 * Registers one signal connection under the active job.
 */
bool SchedulingService::registerActiveJobConnection(const QMetaObject::Connection& connection) {
    if (!connection || !m_activeJob.has_value()) {
        return false;
    }

    m_activeJob->connections.push_back(connection);
    return true;
}

/*
 * Disconnects every signal connection recorded for a job.
 */
void SchedulingService::disconnectJobConnections(const ActiveSchedulingJob& job) {
    for (const QMetaObject::Connection& connection : job.connections) {
        QObject::disconnect(connection);
    }
}

/*
 * Cleans the currently active scheduling job after the worker reports a final
 * outcome.
 */
void SchedulingService::cleanupActiveJob(SchedulingWorker* expectedWorker) {
    if (!m_activeJob.has_value()) {
        return;
    }

    if (expectedWorker != nullptr && m_activeJob->worker != expectedWorker) {
        return;
    }

    m_activeJob->state = ActiveSchedulingJob::State::Finishing;

    ActiveSchedulingJob job = *m_activeJob;
    m_activeJob.reset();

    if (job.thread != nullptr) {
        job.thread->quit();
    }

    if (job.worker != nullptr) {
        job.worker->deleteLater();
    }

    delete job.generator;
}

/*
 * Safely tears down a running job while SchedulingService is being destroyed.
 */
void SchedulingService::shutdownActiveJob() {
    if (!m_activeJob.has_value()) {
        return;
    }

    m_activeJob->state = ActiveSchedulingJob::State::ShuttingDown;

    ActiveSchedulingJob job = *m_activeJob;
    m_activeJob.reset();
    disconnectJobConnections(job);

    if (job.worker != nullptr) {
        job.worker->requestCancellation();
    }

    if (job.thread != nullptr) {
        if (job.thread->isRunning()) {
            job.thread->requestInterruption();
            job.thread->quit();
            job.thread->wait();
        }
    }

    delete job.worker;
    delete job.generator;
    delete job.thread;
}

/*
 * Requests cooperative cancellation of the active scheduling run.
 */
void SchedulingService::cancelActiveGeneration() {
    if (!m_activeJob.has_value()) {
        return;
    }

    m_activeJob->state = ActiveSchedulingJob::State::Cancelling;

    if (m_activeJob->worker != nullptr) {
        m_activeJob->worker->requestCancellation();
    }

    if (m_activeJob->thread != nullptr) {
        m_activeJob->thread->requestInterruption();
    }
}

/*
 * Starts asynchronous schedule generation.
 *
 * This method creates:
 * - a new QThread
 * - a ScheduleGenerator for the given block
 * - a SchedulingWorker that runs the generator inside the thread
 */
void SchedulingService::startAsyncGeneration(const SchedulingBlock& block, const ScheduleSettings& settings, int limitPerBlock) {
    // Safety check to prevent multiple concurrent scheduling operations
    /*
     * Reject a new request if a previous scheduling operation is still running.
     */
    if (m_activeJob.has_value()) {
        emit generationFailed("Scheduling is already running. Please wait.");
        return;
    }

    // create a new QThread and a new ScheduleGenerator for this operation
    /*
     * Create the thread, generator, and worker for this single operation.
     *
     * The generator performs the actual algorithm.
     * The worker is responsible for running it inside the QThread.
     */
    std::unique_ptr<QThread> threadOwner;
    std::unique_ptr<ScheduleGenerator> generatorOwner;
    std::unique_ptr<SchedulingWorker> workerOwner;

    try {
        threadOwner = std::make_unique<QThread>();
        generatorOwner = std::make_unique<ScheduleGenerator>(block, settings, 30.0); // 30 seconds max runtime
        workerOwner = std::make_unique<SchedulingWorker>(generatorOwner.get(), limitPerBlock);
    } catch (const std::exception& ex) {
        emit generationFailed(QString("Failed to start scheduling job: %1").arg(QString::fromStdString(ex.what())));
        return;
    } catch (...) {
        emit generationFailed("Failed to start scheduling job due to an unknown error.");
        return;
    }

    auto* thread = threadOwner.release();
    auto* generator = generatorOwner.release();
    auto* worker = workerOwner.release();

    /*
     * Register the active job before wiring and starting the thread.
     * From this point on, the service has one concrete scheduling run to track.
     */
    m_activeJob = ActiveSchedulingJob{
        thread,
        generator,
        worker,
        limitPerBlock,
        ActiveSchedulingJob::State::Starting
    };

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
    bool connectionsReady = true;

    connectionsReady = connectionsReady && registerActiveJobConnection(
        connect(thread, &QThread::started, this, [this, thread]() {
            markActiveJobRunning(thread);
        }, Qt::QueuedConnection)
    );

    connectionsReady = connectionsReady && registerActiveJobConnection(
        connect(thread, &QThread::started, worker, &SchedulingWorker::run)
    );

    // When the worker finishes successfully
    /*
     * Handle successful generation.
     *
     * The service resets its running flag, emits the result,
     * then shuts down and cleans the worker resources.
     */
    connectionsReady = connectionsReady && registerActiveJobConnection(
        connect(worker, &SchedulingWorker::finished, this, [this, worker](const std::vector<ScheduleGenerationResult>& solutions) {
            cleanupActiveJob(worker);
            
            // Emit the successful result back to the AppController
            emit generationFinished(solutions);
        }, Qt::QueuedConnection)
    );

    // When the worker fails with an error
    /*
     * Handle generation failure.
     *
     * The service resets its running flag, emits the error message,
     * then shuts down and cleans the worker resources.
     */
    connectionsReady = connectionsReady && registerActiveJobConnection(
        connect(worker, &SchedulingWorker::failed, this, [this, worker](QString message) {
            cleanupActiveJob(worker);
            
            emit generationFailed(message);
        }, Qt::QueuedConnection)
    );

    // Ensure the thread is deleted when it finishes
    /*
     * Delete the QThread object after it finishes.
     */
    connectionsReady = connectionsReady && registerActiveJobConnection(
        connect(thread, &QThread::finished, thread, &QObject::deleteLater)
    );

    if (!connectionsReady) {
        shutdownActiveJob();
        emit generationFailed("Failed to connect scheduling thread signals.");
        return;
    }

    /*
     * Start the background thread.
     *
     * This triggers QThread::started, which then calls SchedulingWorker::run.
     */
    thread->start();

    if (!thread->isRunning()) {
        shutdownActiveJob();
        emit generationFailed("Failed to start scheduling thread.");
    }
}
