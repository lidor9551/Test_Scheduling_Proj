#include "SchedulingService.h"
#include "ScheduleGenerator.h"
#include "SchedulingWorker.h"

#include <QThread>
#include <QDebug>

SchedulingService::SchedulingService(QObject* parent)
    : QObject(parent), m_isRunning(false) {
}

SchedulingService::~SchedulingService() {
    // Destructor: Ensure that if the service is destroyed while a scheduling operation is running, we handle it gracefully.
}

void SchedulingService::startAsyncGeneration(const SchedulingBlock& block, int limitPerBlock) {
    // Safety check to prevent multiple concurrent scheduling operations
    if (m_isRunning) {
        emit generationFailed("Scheduling is already running. Please wait.");
        return;
    }

    m_isRunning = true;

    // create a new QThread and a new ScheduleGenerator for this operation
    auto* thread = new QThread();
    auto* generator = new ScheduleGenerator(block); // המנוע הטהור החדש שניקינו
    auto* worker = new SchedulingWorker(generator, limitPerBlock); // השליח שיודע להחזיר ScheduleGenerationResult

    // Move the worker to the new thread
    worker->moveToThread(thread);

    // Connect signals and slots for proper cleanup and result handling
    // When the thread starts, invoke the worker's run method
    connect(thread, &QThread::started, worker, &SchedulingWorker::run);

    // When the worker finishes successfully
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
    connect(worker, &SchedulingWorker::failed, this, [this, thread, worker, generator](QString message) {
        m_isRunning = false;
        
        emit generationFailed(message);
        
        thread->quit();
        worker->deleteLater();
        delete generator;
    });

    // Ensure the thread is deleted when it finishes
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}