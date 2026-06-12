#include "Course.h"

#include <stdexcept>

Course::Course(const std::string& name,
               const std::string& number,
               const std::string& instructor,
               Evaluation eval)
    : courseName(name),
      courseNumber(number),
      instructorName(instructor),
      evaluationMethod(eval) {
}

void Course::addProgram(const std::string& progID,
                        Year year,
                        Semester sem,
                        Requirement req) {
    ProgramDetails newProgram = {progID, year, sem, req};
    programs.push_back(newProgram);
}

// === Domain enum conversion helpers ===

std::string semesterToString(Semester semester) {
    switch (semester) {
        case Semester::FALL:
            return "FALL";
        case Semester::SPRI:
            return "SPRI";
        case Semester::SUMM:
            return "SUMM";
    }

    return "UNKNOWN";
}

std::string moedToString(Moed moed) {
    switch (moed) {
        case Moed::ALEPH:
            return "Aleph";
        case Moed::BET:
            return "Bet";
        case Moed::GIMEL:
            return "Gimel";
    }

    return "Unknown";
}

std::string requirementToString(Requirement requirement) {
    switch (requirement) {
        case Requirement::OBLIGATORY:
            return "Obligatory";
        case Requirement::ELECTIVE:
            return "Elective";
    }

    return "Unknown";
}

std::string evaluationToString(Evaluation evaluation) {
    switch (evaluation) {
        case Evaluation::EXAM:
            return "Exam";
        case Evaluation::PROJECT:
            return "Project";
        case Evaluation::ATTENDANCE:
            return "Attendance";
    }

    return "Unknown";
}

int yearToInt(Year year) {
    return static_cast<int>(year);
}

Year yearFromInt(int year) {
    switch (year) {
        case 1:
            return Year::FIRST;
        case 2:
            return Year::SECOND;
        case 3:
            return Year::THIRD;
        case 4:
            return Year::FOURTH;
        default:
            throw std::invalid_argument("Invalid academic year: " + std::to_string(year));
    }
}

// === Getters ===

const std::string& Course::getCourseName() const {
    return courseName;
}

const std::string& Course::getCourseNumber() const {
    return courseNumber;
}

const std::string& Course::getInstructorName() const {
    return instructorName;
}

const std::vector<ProgramDetails>& Course::getPrograms() const {
    return programs;
}

Evaluation Course::getEvaluationMethod() const {
    return evaluationMethod;
}

bool Course::requiresExam() const {
    return evaluationMethod == Evaluation::EXAM;
}