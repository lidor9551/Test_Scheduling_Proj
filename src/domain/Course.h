#pragma once

#include <string>
#include <vector>
#include <QMetaType>

/*
 * This file defines the Course domain model.
 *
 * The Course model represents one academic course in the system.
 * It stores the course identity, instructor, evaluation method,
 * and the academic programs that include this course.
 *
 * This file belongs to the domain layer.
 * It should stay independent from UI screens, file parsing, and scheduling logic.
 */

// Domain enums represent closed sets of valid academic values.
// Using enum class prevents invalid strings/integers from spreading across the codebase.

// Represents the semester in which a course is taught.
enum class Semester {
    FALL,
    SPRI,
    SUMM
};

// Represents the exam period, also known as Moed.
enum class Moed {
    ALEPH,
    BET,
    GIMEL
};

// Represents the academic year of a program.
// The explicit numeric values make conversion to integers safe and simple.
enum class Year {
    FIRST = 1,
    SECOND = 2,
    THIRD = 3,
    FOURTH = 4
};

// Represents whether a course is mandatory or elective for a specific program.
enum class Requirement {
    OBLIGATORY,
    ELECTIVE
};

// Represents how the course is evaluated.
// Only courses with Evaluation::EXAM are relevant for exam scheduling.
enum class Evaluation {
    EXAM,
    PROJECT,
    ATTENDANCE
};

// Conversion helpers are kept close to the domain model for now.
// Later, these can move into DomainText.h / DomainText.cpp.

// Converts a Semester enum value into its string representation.
std::string semesterToString(Semester semester);

// Converts a Moed enum value into its string representation.
std::string moedToString(Moed moed);

// Converts a Requirement enum value into its string representation.
std::string requirementToString(Requirement requirement);

// Converts an Evaluation enum value into its string representation.
std::string evaluationToString(Evaluation evaluation);

// Converts a strongly typed Year enum into its numeric value.
int yearToInt(Year year);

// Converts an integer into a strongly typed Year enum.
// Invalid year values are rejected in the implementation.
Year yearFromInt(int year);

/*
 * ProgramDetails connects a course to a specific academic program.
 *
 * A single course can appear in more than one program.
 * For each program membership, we keep:
 * - program ID
 * - academic year
 * - semester
 * - whether the course is obligatory or elective
 */
struct ProgramDetails {
    std::string programID;
    Year year;
    Semester semester;
    Requirement requirement;
};

/*
 * Course is the central domain object for academic courses.
 *
 * It contains only course-related data.
 * Other layers decide how to parse it, display it, or schedule it.
 */
class Course {
private:
    // Human-readable course name.
    std::string courseName;

    // Unique course number from the input data.
    std::string courseNumber;

    // Instructor name for display and export purposes.
    std::string instructorName;

    // All academic program memberships for this course.
    std::vector<ProgramDetails> programs;

    // The evaluation method determines whether this course requires an exam.
    Evaluation evaluationMethod;

public:
    // Default constructor is required for Qt QVariant and QMetaType support.
    Course() {}

    // Creates a course with its basic identity and evaluation method.
    Course(const std::string& name,
           const std::string& number,
           const std::string& instructor,
           Evaluation eval);

    /*
     * Adds a program membership to this course.
     *
     * This does not create a new course.
     * It only states that this course belongs to another program/year/semester.
     */
    void addProgram(const std::string& progID,
                    Year year,
                    Semester sem,
                    Requirement req);

    // Returns the course name.
    const std::string& getCourseName() const;

    // Returns the unique course number.
    const std::string& getCourseNumber() const;

    // Returns the instructor name.
    const std::string& getInstructorName() const;

    // Returns all program memberships connected to this course.
    const std::vector<ProgramDetails>& getPrograms() const;

    // Returns the evaluation method of the course.
    Evaluation getEvaluationMethod() const;

    // Returns true only if this course should be included in exam scheduling.
    bool requiresExam() const;
};

// Registers Course as a Qt metatype so it can be stored in QVariant.
Q_DECLARE_METATYPE(Course)