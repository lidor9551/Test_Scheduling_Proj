#include "presentation/ScheduleOutputManager.h"

#include <QCoreApplication>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace {

Course makeCourse(const std::string& name,
                  const std::string& number,
                  Requirement requirement = Requirement::OBLIGATORY) {
    Course course(name, number, "Dr. Export", Evaluation::EXAM);
    course.addProgram("83108", Year::FIRST, Semester::FALL, requirement);
    return course;
}

std::vector<ExamPeriod> makePeriods() {
    return {
        ExamPeriod(
            Semester::FALL,
            Moed::ALEPH,
            Date(1, 1, 2026),
            Date(10, 1, 2026),
            {}
        )
    };
}

QString readTextFile(const QString& path) {
    QFile file(path);
    const bool opened = file.open(QIODevice::ReadOnly | QIODevice::Text);
    assert(opened);

    QTextStream in(&file);
    return in.readAll();
}

void testExportWritesCurrentScheduleToReadableTextFile() {
    std::vector<Course> courses;
    courses.reserve(2);

    courses.push_back(makeCourse("Algorithms", "89101", Requirement::OBLIGATORY));
    courses.push_back(makeCourse("Databases", "89102", Requirement::ELECTIVE));

    ScheduleGenerationResult result({
        {&courses[0], Date(1, 1, 2026), true},
        {&courses[1], Date(3, 1, 2026), false}
    });

    ScheduleOutputManager manager;
    manager.setAvailablePeriods({"FALL"}, {"Aleph"});
    manager.setPeriodFilter("FALL", "Aleph");
    manager.setSchedulingData({result}, courses, makePeriods());

    QTemporaryDir tempDir;
    assert(tempDir.isValid());

    const QString exportPath = tempDir.path() + "/exported_schedule.txt";

    const bool saved = manager.saveCurrentScheduleToFile(exportPath);
    assert(saved);

    const QString content = readTextFile(exportPath);

    assert(content.contains("Schedule Export"));
    assert(content.contains("FALL"));
    assert(content.contains("Aleph"));

    assert(content.contains("Algorithms"));
    assert(content.contains("89101"));
    assert(content.contains("01-01-2026"));
    assert(content.contains("Obligatory"));

    assert(content.contains("Databases"));
    assert(content.contains("89102"));
    assert(content.contains("03-01-2026"));
    assert(content.contains("Elective"));
}

void testExportRejectsInvalidStateWithNoSolutions() {
    ScheduleOutputManager manager;

    QTemporaryDir tempDir;
    assert(tempDir.isValid());

    const QString exportPath = tempDir.path() + "/should_not_exist.txt";

    const bool saved = manager.saveCurrentScheduleToFile(exportPath);
    assert(!saved);

    QFile outputFile(exportPath);
    assert(!outputFile.exists());
}

void testExportRejectsDirectoryPath() {
    std::vector<Course> courses;
    courses.reserve(1);

    courses.push_back(makeCourse("Algorithms", "89101"));

    ScheduleGenerationResult result({
        {&courses[0], Date(1, 1, 2026), true}
    });

    ScheduleOutputManager manager;
    manager.setAvailablePeriods({"FALL"}, {"Aleph"});
    manager.setPeriodFilter("FALL", "Aleph");
    manager.setSchedulingData({result}, courses, makePeriods());

    QTemporaryDir tempDir;
    assert(tempDir.isValid());

    const bool saved = manager.saveCurrentScheduleToFile(tempDir.path());
    assert(!saved);
}

} // namespace

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    testExportWritesCurrentScheduleToReadableTextFile();
    testExportRejectsInvalidStateWithNoSolutions();
    testExportRejectsDirectoryPath();

    std::cout << "ExportFlowIntegrationTest passed." << std::endl;
    return 0;
}