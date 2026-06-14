#include "ScheduleGenerationResult.h"

#include <utility>

// Constructor: Initializes the result with a specific set of exam assignments.
// Using std::move to avoid unnecessary copying of the vector.
ScheduleGenerationResult::ScheduleGenerationResult(std::vector<ExamAssignment> assignments)
    : m_assignments(std::move(assignments)), 
      m_electiveConflicts(0), 
      m_minDaysBetweenObligatory(0) {
    // Note for V3.0: In the future, we might calculate the metrics right here 
    // in the constructor by iterating over m_assignments, or receive them as parameters.
}

// Retrieves the read-only list of all exam assignments in this schedule
const std::vector<ExamAssignment>& ScheduleGenerationResult::getAssignments() const {
    return m_assignments;
}

// Retrieves the number of overlapping elective courses (V3.0 metric)
int ScheduleGenerationResult::getElectiveConflicts() const {
    return m_electiveConflicts;
}

// Retrieves the tightest gap between any two obligatory exams (V3.0 metric)
int ScheduleGenerationResult::getMinDaysBetweenObligatory() const {
    return m_minDaysBetweenObligatory;
}