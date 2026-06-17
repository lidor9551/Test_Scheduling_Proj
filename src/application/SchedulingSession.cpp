#include "application/SchedulingSession.h"
#include "scheduling/IConflictRule.h"

#include <algorithm>
#include <utility>
#include <sstream>

/*
 * Replaces all loaded courses and exam periods.
 *
 * std::move is used because the caller gives ownership of the new vectors
 * to the session. This avoids unnecessary copying.
 */
void SchedulingSession::replaceData(std::vector<Course> courses,
                                    std::vector<ExamPeriod> examPeriods) {
    courses_ = std::move(courses);
    examPeriods_ = std::move(examPeriods);
}

/*
 * Replaces only the exam periods in the session.
 *
 * This supports flows where the user edits the calendar dates
 * without reloading the course file.
 */
void SchedulingSession::replaceExamPeriods(std::vector<ExamPeriod> examPeriods) {
    examPeriods_ = std::move(examPeriods);
}

// basic struct for rule info.
Struct RuleConfig {
    bool isActive = false;
    int k = 0;
};

// struct for rule config types.
Struct ScheduleSettings {
    RuleConfig minDaysObligatory;
    RuleConfig minDaysAll;
    RuleConfig maxElectiveConflicts;
    RuleConfig obligatorySpan;
    RuleConfig maxExamsPerDay;
};

/*
 * Adds new courses and exam periods without deleting existing data.
 *
 * This method also counts skipped duplicates so the UI can give useful feedback
 * about what was actually added.
 */
SchedulingSession::AppendResult SchedulingSession::appendData(
    const std::vector<Course>& courses,
    const std::vector<ExamPeriod>& examPeriods) {
    AppendResult result;

    /*
     * Add each course only if another course with the same course number
     * is not already stored in the session.
     */
    for (const Course& course : courses) {
        if (hasCourse(course.getCourseNumber())) {
            result.skippedDuplicateCourses++;
            continue;
        }

        courses_.push_back(course);
        result.addedCourses++;
    }

    /*
     * Add each exam period only if the same semester and moed combination
     * is not already stored in the session.
     */
    for (const ExamPeriod& period : examPeriods) {
        if (hasExamPeriod(period.getSemester(), period.getMoed())) {
            result.skippedDuplicateExamPeriods++;
            continue;
        }

        examPeriods_.push_back(period);
        result.addedExamPeriods++;
    }

    return result;
}

/*
 * A validation result is valid only when no errors were collected.
 */
bool SchedulingSession::ValidationResult::isValid() const {
    return errors.empty();
}

/*
 * Converts multiple validation errors into one displayable message.
 *
 * The method keeps the original error order and separates messages with spaces.
 */
std::string SchedulingSession::ValidationResult::message() const {
    if (errors.empty()) {
        return "";
    }

    std::ostringstream stream;

    for (std::size_t i = 0; i < errors.size(); ++i) {
        if (i > 0) {
            stream << " ";
        }

        stream << errors[i];
    }

    return stream.str();
}

/*
 * Clears the entire session.
 *
 * After this call, the application behaves as if no data was loaded
 * and no programs were selected.
 */
void SchedulingSession::clear() {
    courses_.clear();
    examPeriods_.clear();
    selectedPrograms_.clear();
}

/*
 * Selects an academic program for the next scheduling run.
 *
 * The method rejects:
 * - empty IDs
 * - duplicate selections
 * - selections beyond MaxSelectedPrograms
 */
bool SchedulingSession::selectProgram(const std::string& programId) {
    if (programId.empty() || isProgramSelected(programId)) {
        return false;
    }

    if (selectedPrograms_.size() >= MaxSelectedPrograms) {
        return false;
    }

    selectedPrograms_.push_back(programId);
    return true;
}

/*
 * Removes an academic program from the selected list.
 *
 * If the program is not found, nothing is changed and the method returns false.
 */
bool SchedulingSession::deselectProgram(const std::string& programId) {
    auto it = std::find(selectedPrograms_.begin(),
                        selectedPrograms_.end(),
                        programId);

    if (it == selectedPrograms_.end()) {
        return false;
    }

    selectedPrograms_.erase(it);
    return true;
}

/*
 * Checks whether a program is already selected.
 *
 * This is used both for UI state and to prevent duplicate selections.
 */
bool SchedulingSession::isProgramSelected(const std::string& programId) const {
    return std::find(selectedPrograms_.begin(),
                     selectedPrograms_.end(),
                     programId) != selectedPrograms_.end();
}

/*
 * Returns the loaded courses as a read-only reference.
 *
 * Returning a reference avoids copying the full course list.
 */
const std::vector<Course>& SchedulingSession::courses() const {
    return courses_;
}

/*
 * Returns the loaded exam periods as a read-only reference.
 */
const std::vector<ExamPeriod>& SchedulingSession::examPeriods() const {
    return examPeriods_;
}

const std::vector<IConflictRule>& SchedulingSession::conflictRules() const {
    return conflictRules_;
}

/*
 * Returns the selected program IDs as a read-only reference.
 */
const std::vector<std::string>& SchedulingSession::selectedPrograms() const {
    return selectedPrograms_;
}

/*
 * Returns the number of loaded courses.
 *
 * The value is converted to int because Qt/QML-facing counters usually use int.
 */
int SchedulingSession::courseCount() const {
    return static_cast<int>(courses_.size());
}

/*
 * Returns the number of loaded exam periods.
 */
int SchedulingSession::examPeriodCount() const {
    return static_cast<int>(examPeriods_.size());
}

/*
 * Returns the number of selected programs.
 */
int SchedulingSession::selectedProgramCount() const {
    return static_cast<int>(selectedPrograms_.size());
}

/*
 * Returns whether the session contains any loaded data.
 *
 * This is useful for deciding whether the application is in an empty state.
 */
bool SchedulingSession::hasData() const {
    return !courses_.empty() || !examPeriods_.empty();
}

/*
 * Validates the required conditions before running the scheduler.
 *
 * Schedule generation requires:
 * - at least one course
 * - at least one exam period
 * - at least one selected academic program
 */
SchedulingSession::ValidationResult SchedulingSession::validateBeforeGeneration() const {
    ValidationResult result;

    if (courses_.empty()) {
        result.errors.push_back("No courses were loaded.");
    }

    if (examPeriods_.empty()) {
        result.errors.push_back("No exam periods were loaded.");
    }

    if (selectedPrograms_.empty()) {
        result.errors.push_back("At least one academic program must be selected.");
    }

    return result;
}

/*
 * Checks whether a course already exists in the session.
 *
 * Course number is treated as the unique course identity.
 */
bool SchedulingSession::hasCourse(const std::string& courseNumber) const {
    return std::any_of(courses_.begin(),
                       courses_.end(),
                       [&courseNumber](const Course& course) {
                           return course.getCourseNumber() == courseNumber;
                       });
}

/*
 * Checks whether an exam period already exists in the session.
 *
 * The identity of an exam period is currently defined by semester and moed.
 */
bool SchedulingSession::hasExamPeriod(Semester semester, Moed moed) const {
    return std::any_of(examPeriods_.begin(),
                       examPeriods_.end(),
                       [semester, moed](const ExamPeriod& period) {
                           return period.getSemester() == semester &&
                                  period.getMoed() == moed;
                       });
}