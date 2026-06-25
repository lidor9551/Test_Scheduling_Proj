#include "presentation/ScheduleOutputManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QVariantMap>
#include <cassert>
#include <iostream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    ScheduleOutputManager manager;

    // No schedules were generated yet, so export must be blocked.
    assert(!manager.saveCurrentScheduleToFile(
        QDir::temp().filePath("exam_scheduler_should_not_export_empty.txt")
    ));

    Course course("Algorithms", "89101", "Dr. A", Evaluation::EXAM);
    course.addProgram("83108", Year::SECOND, Semester::FALL, Requirement::OBLIGATORY);

    ExamAssignment assignment{
        &course,
        Date(4, 1, 2026),
        true
    };

    ScheduleGenerationResult result({assignment});

    ExamPeriod period(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(10, 1, 2026),
        {}
    );

    manager.setSchedulingData({result}, {course}, {period});

    // Empty export path must be blocked.
    assert(!manager.saveCurrentScheduleToFile(""));

    // Directory path must be blocked.
    assert(!manager.saveCurrentScheduleToFile(QDir::tempPath()));

    // Non-existing parent directory must be blocked.
    const QString invalidParentPath =
        QDir::temp().filePath("folder_that_should_not_exist_12345/export.txt");
    assert(!manager.saveCurrentScheduleToFile(invalidParentPath));

    // Valid path should export successfully.
    const QString validPath =
        QDir::temp().filePath("exam_scheduler_export_safety_test.txt");

    QFile::remove(validPath);

    assert(manager.saveCurrentScheduleToFile(validPath));
    assert(QFile::exists(validPath));

    QFile::remove(validPath);

    int metricsChangedCount = 0;
    QObject::connect(&manager,
                     &ScheduleOutputManager::currentMetricsChanged,
                     [&metricsChangedCount]() {
                         ++metricsChangedCount;
                     });

    manager.setMetricsUpdater([](const ScheduleGenerationResult& updatedSchedule) {
        ScheduleMetrics metrics;
        metrics.avgDaysBetweenAll = 42.0;
        metrics.maxExamsInSingleDay =
            static_cast<int>(updatedSchedule.getAssignments().size());
        return metrics;
    });

    metricsChangedCount = 0;

    const QVariantMap moveResult =
        manager.requestMove("89101", "05-01-2026");

    assert(moveResult.value("status").toInt() == 1);
    assert(metricsChangedCount == 1);

    const QVariantMap currentMetrics = manager.getCurrentMetrics();
    assert(currentMetrics.value("avgDaysBetweenAll").toDouble() == 42.0);
    assert(currentMetrics.value("maxExamsInSingleDay").toInt() == 1);

    // After clearData(), current index is invalid and export must be blocked.
    manager.clearData();
    assert(!manager.saveCurrentScheduleToFile(validPath));

    std::cout << "ScheduleOutputManager export safety test passed." << std::endl;
    return 0;
}
