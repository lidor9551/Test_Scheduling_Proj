#pragma once

#include <QObject>
#include <QString>
#include <vector>
#include "domain/ScheduleGenerationResult.h"

class ScheduleGenerator;

class SchedulingWorker : public QObject {
    Q_OBJECT

public:
    SchedulingWorker(const ScheduleGenerator* generator, int limitPerBlock);

public slots:
    void run();

signals:
    void finished(const std::vector<ScheduleGenerationResult>& solutions);
    void failed(QString message);

private:
    const ScheduleGenerator* generator_;
    int limitPerBlock_;
};