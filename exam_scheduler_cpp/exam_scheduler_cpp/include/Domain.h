#ifndef DOMAIN_H
#define DOMAIN_H

#include "Date.h"
#include <string>
#include <utility>
#include <vector>

enum class RequirementType {
    Obligatory,
    Elective
};

enum class EvaluationType {
    Exam,
    Project,
    Attendance
};

struct ProgramOffering {
    std::string programId;
    int year;
    std::string semester;
    RequirementType requirement;
};

class Course {
private:
    std::string name_;
    std::string number_;
    std::string instructor_;
    std::vector<ProgramOffering> offerings_;
    EvaluationType evaluation_;

public:
    Course(
        std::string name,
        std::string number,
        std::string instructor,
        std::vector<ProgramOffering> offerings,
        EvaluationType evaluation
    );

    const std::string& name() const;
    const std::string& number() const;
    const std::string& instructor() const;
    const std::vector<ProgramOffering>& offerings() const;
    EvaluationType evaluation() const;
};

struct ExcludedRange {
    Date start;
    Date end;
    std::string comment;

    bool contains(const Date& date) const;
};

class ExamPeriod {
private:
    std::string semester_;
    std::string moed_;
    Date startDate_;
    Date endDate_;
    std::vector<ExcludedRange> excludedRanges_;

public:
    ExamPeriod(
        std::string semester,
        std::string moed,
        Date startDate,
        Date endDate,
        std::vector<ExcludedRange> excludedRanges
    );

    const std::string& semester() const;
    const std::string& moed() const;
    const Date& startDate() const;
    const Date& endDate() const;
    const std::vector<ExcludedRange>& excludedRanges() const;

    std::vector<Date> allowedDates() const;
};

struct CourseMembership {
    int groupId;
    RequirementType requirement;
};

struct RuntimeCourse {
    int index;
    const Course* course;
    std::vector<CourseMembership> memberships;
};

struct SchedulingBlock {
    std::string semester;
    std::string moed;
    std::vector<Date> allowedDates;
    std::vector<RuntimeCourse> runtimeCourses;
};

std::string toString(RequirementType type);
std::string toString(EvaluationType type);
RequirementType parseRequirementType(const std::string& value);
EvaluationType parseEvaluationType(const std::string& value);

#endif
