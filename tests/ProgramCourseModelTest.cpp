#include <QCoreApplication>

#include <iostream>
#include <vector>

#include "gui/ProgramCourseModel.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    int failures = 0;

    auto expect = [&failures](bool condition, const char* message) {
        if (!condition) {
            std::cerr << "FAILED: " << message << '\n';
            ++failures;
        }
    };

    Course algorithms(
        "Algorithms",
        "83123",
        "Prof. A",
        Evaluation::EXAM
    );
    algorithms.addProgram(
        "83108",
        2,
        Semester::FALL,
        Requirement::OBLIGATORY
    );
    algorithms.addProgram(
        "83102",
        2,
        Semester::FALL,
        Requirement::ELECTIVE
    );

    Course softwareProject(
        "Software Project",
        "83124",
        "Prof. B",
        Evaluation::PROJECT
    );
    softwareProject.addProgram(
        "83108",
        2,
        Semester::SPRI,
        Requirement::ELECTIVE
    );

    Course databases(
        "Databases",
        "83125",
        "Prof. C",
        Evaluation::EXAM
    );
    databases.addProgram(
        "83108",
        3,
        Semester::FALL,
        Requirement::OBLIGATORY
    );

    ProgramCourseModel model;
    model.setCourses({algorithms, softwareProject, databases});

    expect(
        model.rowCount() == 4,
        "The unfiltered model should contain four program-course associations."
    );

    model.setFilters("83108", 2, "FALL");

    expect(
        model.rowCount() == 1,
        "Filtering by program, year and semester should return one course."
    );

    QModelIndex firstIndex = model.index(0, 0);

    expect(
        model.data(
            firstIndex,
            ProgramCourseModel::CourseNumberRole
        ).toString() == "83123",
        "The filtered course number should be 83123."
    );

    expect(
        model.data(
            firstIndex,
            ProgramCourseModel::RequirementRole
        ).toString() == "Obligatory",
        "The requirement should be Obligatory."
    );

    expect(
        model.data(
            firstIndex,
            ProgramCourseModel::EvaluationRole
        ).toString() == "Exam",
        "The evaluation method should be Exam."
    );

    model.setFilters("83108", 2, "SPRING");

    expect(
        model.rowCount() == 1,
        "SPRING should be normalized to SPRI."
    );

    firstIndex = model.index(0, 0);

    expect(
        model.data(
            firstIndex,
            ProgramCourseModel::EvaluationRole
        ).toString() == "Project",
        "The SPRI course evaluation should be Project."
    );

    model.clearFilters();

    expect(
        model.rowCount() == 4,
        "Clearing filters should restore all rows."
    );

    if (failures == 0) {
        std::cout << "ProgramCourseModel test passed.\n";
        return 0;
    }

    std::cerr << failures << " test assertion(s) failed.\n";
    return 1;
}