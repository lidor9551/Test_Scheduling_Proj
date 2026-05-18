#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include "../model/Course.h"
#include "../model/ExamPeriod.h"

// Represents a specific group of students (Program + Year) taking the course
struct CourseMembership {
    int groupId; // Unique ID to check for collisions (e.g., Program 83104, Year 2)
    Requirement requirement; // OBLIGATORY or ELECTIVE
};

// A stripped-down wrapper for a course during runtime scheduling
struct RuntimeCourse {
    int id; // Internal ID for the backtracking algorithm
    const Course* course; // Pointer to the original OOP Course (no copying!)
    std::vector<CourseMembership> memberships; // Which groups are taking it
};

// The final package sent to the Scheduler: One exam period and all its relevant courses & free days
struct SchedulingBlock {
    std::string semester;
    std::string moed;
    std::vector<Date> allowedDates;
    std::vector<RuntimeCourse> runtimeCourses;
};

class SchedulingPreprocessor {
private:
    const std::vector<Course>& courses_;
    const std::vector<ExamPeriod>& periods_;
    std::set<std::string> selectedPrograms_;

    // Helper to map Semester Enum to String for comparison
    std::string semesterToString(Semester sem) const;

    // Filters and prepares the courses specific to a given exam period
    std::vector<RuntimeCourse> selectRuntimeCoursesForPeriod(const ExamPeriod& period) const;

public:
    SchedulingPreprocessor(
        const std::vector<Course>& courses,
        const std::vector<ExamPeriod>& periods,
        const std::vector<std::string>& selectedPrograms
    );

    // Builds the scheduling blocks for all periods
    std::vector<SchedulingBlock> buildBlocks() const;
};