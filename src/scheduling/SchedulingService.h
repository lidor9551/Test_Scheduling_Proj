#pragma once

#include <QObject>
#include <QMetaObject>
#include <QString>
#include <optional>
#include <vector>
#include "scheduling/Preprocessor.h" 
#include "domain/ScheduleGenerationResult.h"
#include "application/SchedulingSession.h"

class QThread;
class ScheduleGenerator;
class SchedulingWorker;

/*
 * SchedulingService is the high-level entry point for asynchronous scheduling.
 *
 * It hides thread management from AppController.
 * The controller asks this service to start generation, and the service emits
 * signals when the work succeeds or fails.
 */

/**
 * @class SchedulingService
 * @brief Orchestrates the async execution of the schedule generation.
 * This service acts as the entry point for the scheduling module. It manages 
 * the creation of the QThread, the ScheduleGenerator, and the SchedulingWorker.
 * It prevents the UI thread from freezing and completely isolates the AppController 
 * from thread management.
 */
class SchedulingService : public QObject {
    Q_OBJECT

public:
    /*
     * Creates the scheduling service.
     *
     * QObject parent ownership is supported through the standard Qt parent parameter.
     */
    explicit SchedulingService(QObject* parent = nullptr);

    /*
     * Destroys the service.
     *
     * The implementation currently contains only a safety comment,
     * but the destructor is virtual through QObject inheritance.
     */
    ~SchedulingService() override;

    // Starts the scheduling process asynchronously. The results will be emitted via signals.
    /*
     * Starts schedule generation on a dedicated worker thread.
     *
     * The method returns immediately.
     * Results are delivered later through generationFinished or generationFailed.
     */
    void startAsyncGeneration(const SchedulingBlock& block, const ScheduleSettings& settings, int limitPerBlock = -1);

    /*
     * Requests cooperative cancellation of the active scheduling run.
     *
     * If no run is active, the method does nothing.
     */
    void cancelActiveGeneration();

signals:
    // Emitted when the scheduling process completes successfully with the generated solutions.
    /*
     * Emitted when the worker successfully generated schedule results.
     */
    void generationFinished(const std::vector<ScheduleGenerationResult>& solutions);

    /*
     * Emitted when generation fails or when another generation is already running.
     */
    void generationFailed(QString message);

private:
    /*
     * Represents the single scheduling operation currently owned by the service.
     *
     * Keeping these objects together makes the lifecycle explicit: a scheduling
     * run is not just a boolean flag, it is a coordinated thread, worker, and
     * generator that must be cleaned up as one unit.
     */
    struct ActiveSchedulingJob {
        enum class State {
            Starting,
            Running,
            Cancelling,
            Finishing,
            ShuttingDown
        };

        QThread* thread = nullptr;
        ScheduleGenerator* generator = nullptr;
        SchedulingWorker* worker = nullptr;
        int limitPerBlock = -1;
        State state = State::Starting;
        std::vector<QMetaObject::Connection> connections;
    };

    /*
     * Updates the state of the current job if the signal belongs to it.
     */
    void markActiveJobRunning(QThread* expectedThread);

    /*
     * Stores a signal connection owned by the active job.
     */
    bool registerActiveJobConnection(const QMetaObject::Connection& connection);

    /*
     * Disconnects all signal connections owned by a job snapshot.
     */
    void disconnectJobConnections(const ActiveSchedulingJob& job);

    /*
     * Releases the resources owned by the active scheduling job.
     *
     * expectedWorker protects against accidentally cleaning a stale job if an
     * old signal is delivered after a newer job has already been registered.
     */
    void cleanupActiveJob(SchedulingWorker* expectedWorker);

    /*
     * Stops and releases an active job while the service itself is being
     * destroyed. This path waits for the worker thread before deleting objects.
     */
    void shutdownActiveJob();

    /*
     * Empty when the service is idle.
     * Populated while a background scheduling run is active.
     */
    std::optional<ActiveSchedulingJob> m_activeJob;
};
