#include "Course.h"

#include <stdexcept>

/*
 * Creates a Course object with its basic identity.
 *
 * Program membership is intentionally not passed here.
 * A course can belong to several academic programs, so memberships are added
 * separately using addProgram().
 */
Course::Course(const std::string& name,
               const std::string& number,
               const std::string& instructor,
               Evaluation eval)
    : courseName(name),
      courseNumber(number),
      instructorName(instructor),
      evaluationMethod(eval) {
}

/*
 * Adds one academic program membership to the course.
 *
 * Example:
 * The same course can be obligatory for one program and elective for another.
 */
void Course::addProgram(const std::string& progID,
                        Year year,
                        Semester sem,
                        Requirement req) {
    ProgramDetails newProgram = {progID, year, sem, req};
    programs.push_back(newProgram);
}

// === Domain enum conversion helpers ===

/*
 * Converts Semester into the string format used by the UI,
 * preprocessing layer, output screen, and logs.
 */
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

/*
 * Converts Moed into the string format used by filters and display logic.
 */
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

/*
 * Converts Requirement into a readable string.
 */
std::string requirementToString(Requirement requirement) {
    switch (requirement) {
        case Requirement::OBLIGATORY:
            return "Obligatory";
        case Requirement::ELECTIVE:
            return "Elective";
    }

    return "Unknown";
}

/*
 * Converts Evaluation into a readable string.
 */
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

/*
 * Converts a Year enum into its integer value.
 *
 * This is useful when the UI needs to compare or display academic years.
 */
int yearToInt(Year year) {
    return static_cast<int>(year);
}

/*
 * Converts an integer into a Year enum.
 *
 * This protects the domain model from invalid academic years.
 */
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

/*
 * Returns the course name exactly as loaded from the input data.
 */
const std::string& Course::getCourseName() const {
    return courseName;
}

/*
 * Returns the unique course number.
 */
const std::string& Course::getCourseNumber() const {
    return courseNumber;
}

/*
 * Returns the instructor name.
 */
const std::string& Course::getInstructorName() const {
    return instructorName;
}

/*
 * Returns all program memberships connected to this course.
 */
const std::vector<ProgramDetails>& Course::getPrograms() const {
    return programs;
}

/*
 * Returns the evaluation method.
 */
Evaluation Course::getEvaluationMethod() const {
    return evaluationMethod;
}

/*
 * Returns whether the course requires an exam date.
 *
 * Courses evaluated by project or attendance are ignored by the scheduler.
 */
bool Course::requiresExam() const {
    return evaluationMethod == Evaluation::EXAM;
}