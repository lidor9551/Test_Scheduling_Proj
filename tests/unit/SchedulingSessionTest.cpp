#include "application/SchedulingSession.h"

#include <cassert>
#include <iostream>

int main() {
    SchedulingSession session;

    SchedulingSession::ValidationResult emptyValidation = session.validateBeforeGeneration();

    assert(!emptyValidation.isValid());
    assert(emptyValidation.errors.size() == 3);

    assert(session.selectedProgramCount() == 0);
    assert(session.selectedPrograms().empty());

    assert(session.selectProgram("83101"));
    assert(session.isProgramSelected("83101"));
    assert(session.selectedProgramCount() == 1);

    assert(!session.selectProgram("83101"));
    assert(session.selectedProgramCount() == 1);

    assert(session.selectProgram("83102"));
    assert(session.selectProgram("83103"));
    assert(session.selectProgram("83104"));
    assert(session.selectProgram("83105"));

    assert(session.selectedProgramCount() == 5);
    assert(!session.selectProgram("83107"));
    assert(session.selectedProgramCount() == 5);

    assert(session.deselectProgram("83103"));
    assert(!session.isProgramSelected("83103"));
    assert(session.selectedProgramCount() == 4);

    assert(session.selectProgram("83107"));
    assert(session.isProgramSelected("83107"));
    assert(session.selectedProgramCount() == 5);

    assert(!session.selectProgram(""));
    assert(!session.deselectProgram("99999"));

    Course course1("Algorithms", "89101", "Dr. A", Evaluation::EXAM);
    course1.addProgram("83108", Year::SECOND, Semester::FALL, Requirement::OBLIGATORY);

    Course course1Duplicate("Algorithms Duplicate", "89101", "Dr. B", Evaluation::EXAM);
    course1Duplicate.addProgram("83108", Year::SECOND, Semester::FALL, Requirement::OBLIGATORY);

    Course course2("Operating Systems", "89202", "Dr. C", Evaluation::EXAM);
    course2.addProgram("83108", Year::SECOND, Semester::SPRI, Requirement::OBLIGATORY);

    ExamPeriod period1(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(10, 1, 2026),
        {}
    );

    ExamPeriod period1Duplicate(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(10, 1, 2026),
        {}
    );

    ExamPeriod period2(
        Semester::SPRI,
        Moed::BET,
        Date(1, 2, 2026),
        Date(10, 2, 2026),
        {}
    );

    SchedulingSession dataSession;

    dataSession.replaceData({course1}, {period1});
    assert(dataSession.courseCount() == 1);
    assert(dataSession.examPeriodCount() == 1);
    assert(dataSession.hasData());

    SchedulingSession::ValidationResult missingProgramValidation = dataSession.validateBeforeGeneration();

    assert(!missingProgramValidation.isValid());
    assert(missingProgramValidation.errors.size() == 1);
    assert(missingProgramValidation.message().find("At least one academic program") != std::string::npos);

    assert(dataSession.selectProgram("83108"));

    SchedulingSession::ValidationResult validGenerationValidation =
        dataSession.validateBeforeGeneration();

    assert(validGenerationValidation.isValid());
    assert(validGenerationValidation.errors.empty());

    SchedulingSession::AppendResult appendResult =
        dataSession.appendData({course1Duplicate, course2}, {period1Duplicate, period2});

    assert(appendResult.addedCourses == 1);
    assert(appendResult.skippedDuplicateCourses == 1);
    assert(appendResult.addedExamPeriods == 1);
    assert(appendResult.skippedDuplicateExamPeriods == 1);

    assert(dataSession.courseCount() == 2);
    assert(dataSession.examPeriodCount() == 2);

    dataSession.replaceExamPeriods({period2});
    assert(dataSession.courseCount() == 2);
    assert(dataSession.examPeriodCount() == 1);

    dataSession.clear();
    assert(dataSession.courseCount() == 0);
    assert(dataSession.examPeriodCount() == 0);
    assert(dataSession.selectedProgramCount() == 0);
    assert(!dataSession.hasData());

    std::cout << "SchedulingSession test passed." << std::endl;
    return 0;
}