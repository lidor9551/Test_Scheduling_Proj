#pragma once

#include <QObject>
#include <QString>
#include <vector>

class ScheduleGenerator;

class SchedulingWorker : public QObject {
    Q_OBJECT

public:
    SchedulingWorker(const ScheduleGenerator* generator, int limitPerBlock);

public slots:
    void run();

signals:
    void finished(const std::vector<std::vector<int>>& solutions);
    void failed(QString message);

private:
    const ScheduleGenerator* generator_;
    int limitPerBlock_;
};