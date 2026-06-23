#pragma once

#include "domain/Course.h"
#include "domain/Date.h"
#include "domain/ExamPeriod.h"

#include <string>
#include <vector>

namespace TestFactory {

inline Course examCourse(
    const std::string& name = "Algorithms",
    const std::string& number = "89101",
    const std::string& instructor = "Dr. Test",
    const std::string& programId = "83108",
    Year year = Year::SECOND,
    Semester semester = Semester::FALL,
    Requirement requirement = Requirement::OBLIGATORY
) {
    Course course(name, number, instructor, Evaluation::EXAM);
    course.addProgram(programId, year, semester, requirement);
    return course;
}

inline Course electiveExamCourse(
    const std::string& name = "Elective Course",
    const std::string& number = "89222",
    const std::string& instructor = "Dr. Elective",
    const std::string& programId = "83108",
    Year year = Year::SECOND,
    Semester semester = Semester::FALL
) {
    Course course(name, number, instructor, Evaluation::EXAM);
    course.addProgram(programId, year, semester, Requirement::ELECTIVE);
    return course;
}

inline Course projectCourse(
    const std::string& name = "Software Project",
    const std::string& number = "83533",
    const std::string& instructor = "Dr. Project"
) {
    return Course(name, number, instructor, Evaluation::PROJECT);
}

inline Course attendanceCourse(
    const std::string& name = "Seminar Attendance",
    const std::string& number = "83999",
    const std::string& instructor = "Dr. Attendance"
) {
    return Course(name, number, instructor, Evaluation::ATTENDANCE);
}

inline ExcludedRange excludedRange(
    const Date& start,
    const Date& end,
    const std::string& reason = "test exclusion"
) {
    ExcludedRange range;
    range.start = start;
    range.end = end;
    range.reason = reason;
    return range;
}

inline ExamPeriod examPeriod(
    Semester semester = Semester::FALL,
    Moed moed = Moed::ALEPH,
    const Date& start = Date(1, 1, 2026),
    const Date& end = Date(10, 1, 2026),
    const std::vector<ExcludedRange>& exclusions = {}
) {
    return ExamPeriod(semester, moed, start, end, exclusions);
}

} // namespace TestFactory