#include "Course.h"

Course::Course(const std::string& name, const std::string& number, 
               const std::string& instructor, Evaluation eval)
    : courseName(name), courseNumber(number), 
      instructorName(instructor), evaluationMethod(eval) {
}

void Course::addProgram(const std::string& progID, int year, Semester sem, Requirement req) {
    ProgramDetails newProgram = {progID, year, sem, req};
    programs.push_back(newProgram);
}

// === Getters ===

std::string Course::getCourseName() const {
    return courseName;
}

std::string Course::getCourseNumber() const {
    return courseNumber;
}

std::string Course::getInstructorName() const {
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