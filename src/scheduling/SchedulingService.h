#pragma once

#include <QObject>
#include <QString>
#include <vector>
#include "scheduling/Preprocessor.h" 
#include "domain/ScheduleGenerationResult.h"
#include "application/SchedulingSession.h"

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
     * Tracks whether a scheduling operation is already running.
     *
     * This prevents starting multiple solver threads at the same time.
     */
    bool m_isRunning; 
};