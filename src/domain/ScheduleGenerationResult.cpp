#include "ScheduleGenerationResult.h"

#include <algorithm>
#include <utility>

/*
 * This implementation file contains only simple storage logic.
 *
 * ScheduleGenerationResult does not calculate schedules by itself.
 * It only stores the final assignments and exposes them safely to other layers.
 */

// Constructor: Initializes the result with a specific set of exam assignments.
// Using std::move to avoid unnecessary copying of the vector.
/*
 * Stores the generated assignments inside the result object.
 *
 * std::move transfers the vector contents into m_assignments efficiently,
 * which avoids copying all exam assignments.
 */
ScheduleGenerationResult::ScheduleGenerationResult(std::vector<ExamAssignment> assignments)
    : m_assignments(std::move(assignments)), 
      m_electiveConflicts(0), 
      m_minDaysBetweenObligatory(0) {
}

// Retrieves the read-only list of all exam assignments in this schedule
/*
 * Provides read-only access to the schedule assignments.
 *
 * This is used by:
 * - the output screen
 * - export logic
 * - tests
 * - any future sorting or filtering logic
 */
const std::vector<ExamAssignment>& ScheduleGenerationResult::getAssignments() const {
    return m_assignments;
}

// Retrieves the number of overlapping elective courses.
/*
 * Returns the elective conflict metric.
 *
 * In the current version, the value is kept as zero.
 * It is part of the result model so future versions can rank schedules.
 */
int ScheduleGenerationResult::getElectiveConflicts() const {
    return m_electiveConflicts;
}

// Retrieves the tightest gap between any two obligatory exams.
/*
 * Returns the minimum distance metric between obligatory exams.
 *
 * In the current version, the value is kept as zero.
 * It is prepared for future optimization and sorting requirements.
 */
int ScheduleGenerationResult::getMinDaysBetweenObligatory() const {
    return m_minDaysBetweenObligatory;
}

// function to remove assignment from specific solution
bool ScheduleGenerationResult::removeAssignment(const std::string& courseId) {
    auto it = std::remove_if(m_assignments.begin(), m_assignments.end(),
        [&courseId](const ExamAssignment& a) {
            return a.course->getCourseNumber() == courseId;
        });
        
    if (it != m_assignments.end()) {
        m_assignments.erase(it, m_assignments.end());
        return true;
    }
    return false;
}
// function to change assignment's date in specific solution
bool ScheduleGenerationResult::updateAssignmentDate(const std::string& courseId, const Date& newDate) {
    for (auto& assignment : m_assignments) {
        if (assignment.course->getCourseNumber() == courseId) {
            assignment.examDate = newDate;
            return true;
        }
    }
    return false;
}