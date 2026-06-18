#pragma once

#include <vector>
#include "Course.h"
#include "Date.h"

/*
 * This file defines the result model produced by the scheduling algorithm.
 *
 * Instead of returning raw date indexes, the scheduler now returns meaningful
 * domain objects: each assignment connects a real Course to a real Date.
 *
 * This makes the output easier to display, export, test, and extend.
 */

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
/*
 * ExamAssignment represents one exam placed on one date.
 *
 * It does not own the Course object.
 * It only keeps a pointer to the original course so the UI/export layer can still
 * access the full course details without copying the entire course.
 */
struct ExamAssignment {
    const Course* course;  // Pointer to the original course object with all its details
    Date examDate;         // The exact assigned date for the exam
    bool isObligatory;     // Indicates if the course is obligatory 
};

// Struct to hold the evaluation scores (metrics) for a completed schedule
struct ScheduleMetrics {
    double avgDaysBetweenObligatory = 0.0; // Metric 2.1: Average days between mandatory exams
    double avgDaysBetweenAll = 0.0;        // Metric 2.2: Average days between any exams
    int totalElectiveConflicts = 0;        // Metric 2.3: Total number of elective exam overlaps
    int obligatorySpan = 0;                // Metric 2.4: Total day span from the first to last mandatory exam
    int maxExamsInSingleDay = 0;           // Metric 2.5: Maximum number of exams scheduled on the exact same day
};

/*
 * ScheduleGenerationResult represents one complete schedule solution.
 *
 * A single result contains all exam assignments for one valid schedule.
 * The application may generate many ScheduleGenerationResult objects,
 * and the output screen lets the user browse between them.
 */
class ScheduleGenerationResult {
private:
    /*
     * Stores all course-date assignments that belong to this schedule.
     *
     * Each item says:
     * - which course was scheduled
     * - on which date
     * - whether it is obligatory
     */
    std::vector<ExamAssignment> m_assignments; // The list of all exams in this specific schedule

    // V3.0 Preparation: Pre-calculated metrics for sorting optimal schedules
    /*
     * Stores the number of elective conflicts calculated for this schedule.
     *
     * This value is prepared for future sorting/filtering requirements.
     */
    int m_electiveConflicts = 0;

    /*
     * Stores the minimum number of days between obligatory exams.
     *
     * This value is also prepared for future optimization requirements.
     */
    int m_minDaysBetweenObligatory = 0;

public:

    // The newly added metrics used for sorting and UI display
    ScheduleMetrics metrics;
    
    // Constructors
    /*
     * Creates an empty schedule result.
     *
     * This is useful for containers, tests, and default initialization.
     */
    ScheduleGenerationResult() = default;

    /*
     * Creates a schedule result from a ready list of exam assignments.
     *
     * The implementation moves the assignment vector into the object.
     */
    explicit ScheduleGenerationResult(std::vector<ExamAssignment> assignments);

    // Getters
    /*
     * Returns the assignments that make up this schedule.
     *
     * The returned reference is const so callers can read the schedule
     * without modifying its internal state.
     */
    const std::vector<ExamAssignment>& getAssignments() const;
    
    // Metrics accessors (to be calculated and utilized per V3.0 requirements)
    /*
     * Returns the elective conflict metric for this schedule.
     *
     * Currently this is initialized to zero and reserved for future ranking logic.
     */
    int getElectiveConflicts() const;

    /*
     * Returns the minimum gap between obligatory exams.
     *
     * Currently this is initialized to zero and reserved for future ranking logic.
     */
    int getMinDaysBetweenObligatory() const;
};