#pragma once

#include <string>
#include <vector>
#include <QMetaType>

// Domain enums represent closed sets of valid academic values.
// Using enum class prevents invalid strings/integers from spreading across the codebase.
enum class Semester {
    FALL,
    SPRI,
    SUMM
};

enum class Moed {
    ALEPH,
    BET,
    GIMEL
};

enum class Year {
    FIRST = 1,
    SECOND = 2,
    THIRD = 3,
    FOURTH = 4
};

enum class Requirement {
    OBLIGATORY,
    ELECTIVE
};

enum class Evaluation {
    EXAM,
    PROJECT,
    ATTENDANCE
};

// Conversion helpers are kept close to the domain model for now.
// Later, these can move into DomainText.h / DomainText.cpp.
std::string semesterToString(Semester semester);
std::string moedToString(Moed moed);
std::string requirementToString(Requirement requirement);
std::string evaluationToString(Evaluation evaluation);

int yearToInt(Year year);
Year yearFromInt(int year);

struct ProgramDetails {
    std::string programID;
    Year year;
    Semester semester;
    Requirement requirement;
};

class Course {
private:
    std::string courseName;
    std::string courseNumber;
    std::string instructorName;
    std::vector<ProgramDetails> programs;
    Evaluation evaluationMethod;

public:
    Course() {}

    Course(const std::string& name,
           const std::string& number,
           const std::string& instructor,
           Evaluation eval);

    void addProgram(const std::string& progID,
                    Year year,
                    Semester sem,
                    Requirement req);

    const std::string& getCourseName() const;
    const std::string& getCourseNumber() const;
    const std::string& getInstructorName() const;
    const std::vector<ProgramDetails>& getPrograms() const;
    Evaluation getEvaluationMethod() const;

    bool requiresExam() const;
};

Q_DECLARE_METATYPE(Course)