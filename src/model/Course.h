#pragma once
#include <string>
#include <vector>

enum class Semester { FALL, SPRI, SUMM };
enum class Requirement { OBLIGATORY, ELECTIVE };
enum class Evaluation { EXAM, PROJECT, ATTENDANCE };

struct ProgramDetails {
    std::string programID; 
    int year;             
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
    // Constructor
    Course(const std::string& name, const std::string& number, 
           const std::string& instructor, Evaluation eval);

    void addProgram(const std::string& progID, int year, Semester sem, Requirement req);

    // Getters
    const std::string& getCourseName() const;
    const std::string& getCourseNumber() const;
    const std::string& getInstructorName() const;
    const std::vector<ProgramDetails>& getPrograms() const;
    Evaluation getEvaluationMethod() const;

    bool requiresExam() const; 
};