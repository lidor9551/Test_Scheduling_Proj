#pragma once

#include <QObject>
#include <QString>
#include <vector>
#include "scheduling/Preprocessor.h" 
#include "domain/ScheduleGenerationResult.h"

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
    explicit SchedulingService(QObject* parent = nullptr);
    ~SchedulingService() override;

    // Starts the scheduling process asynchronously. The results will be emitted via signals.
    void startAsyncGeneration(const SchedulingBlock& block, int limitPerBlock = -1);

signals:
    // Emitted when the scheduling process completes successfully with the generated solutions.
    void generationFinished(const std::vector<ScheduleGenerationResult>& solutions);
    void generationFailed(QString message);

private:
    bool m_isRunning; 
};