#pragma once

#include <vector>
#include "Course.h"
#include "Date.h"

/**
 * @class ScheduleGenerationResult
 * @brief Domain model representing a single, complete, and valid exam schedule.
 * * This class replaces the raw std::vector<int> primitive obsession from Version 1.0. 
 * Its primary role is to bridge the gap between the purely mathematical scheduling 
 * engine (ScheduleGenerator) and the presentation/infrastructure layers (UI/Export).
 * By holding concrete Course and Date objects, it eliminates the need for the UI 
 * or the OutputWriter to perform data translation and index matching. 
 * It also serves as the foundation for Version 3.0 by storing schedule metrics 
 * (e.g., elective conflicts, minimum days between exams) used for sorting and filtering.
 */

// Helper structure: Represents a single scheduled exam slot
struct ExamAssignment {
    const Course* course;  // Pointer to the original course object with all its details
    Date examDate;         // The exact assigned date for the exam
    bool isObligatory;     // Indicates if the course is obligatory 
};

class ScheduleGenerationResult {
private:
    std::vector<ExamAssignment> m_assignments; // The list of all exams in this specific schedule

    // V3.0 Preparation: Pre-calculated metrics for sorting optimal schedules
    int m_electiveConflicts = 0;
    int m_minDaysBetweenObligatory = 0;

public:
    // Constructors
    ScheduleGenerationResult() = default;
    explicit ScheduleGenerationResult(std::vector<ExamAssignment> assignments);

    // Getters
    const std::vector<ExamAssignment>& getAssignments() const;
    
    // Metrics accessors (to be calculated and utilized per V3.0 requirements)
    int getElectiveConflicts() const;
    int getMinDaysBetweenObligatory() const;
};