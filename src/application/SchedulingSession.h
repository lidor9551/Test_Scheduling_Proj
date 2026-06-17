#pragma once

#include "domain/Course.h"
#include "domain/ExamPeriod.h"
#include "scheduling/IConflictRule.h"

#include <cstddef>
#include <string>
#include <vector>

// basic struct for rule info.
struct RuleConfig {
    bool isActive = false;
    int k = 0;
};

// struct for rule config types.
struct ScheduleSettings {
    RuleConfig minDaysObligatory;
    RuleConfig minDaysAll;
    RuleConfig maxElectiveConflicts;
    RuleConfig obligatorySpan;
    RuleConfig maxExamsPerDay;
};

/*
 * SchedulingSession belongs to the application layer.
 *
 * Its purpose is to keep the current state of the scheduling workflow in one place.
 * Instead of letting different managers keep separate copies of courses, periods,
 * and selected programs, this class acts as the central state holder.
 */

/**
 * @brief Central application state for the scheduling flow.
 *
 * This class is intended to become the single source of truth for:
 * - loaded courses
 * - loaded exam periods
 * - selected academic programs
 *
 * For TES-110, this class is only introduced and connected to the build.
 * AppController behavior is not changed yet.
 */
class SchedulingSession {
public:
    /*
     * AppendResult describes what happened when new data was appended.
     *
     * It helps the controller tell the user how many records were added
     * and how many were skipped because they already existed.
     */
    struct AppendResult {
        // Number of new courses successfully added to the session.
        int addedCourses = 0;

        // Number of courses skipped because a course with the same course number already exists.
        int skippedDuplicateCourses = 0;

        // Number of new exam periods successfully added to the session.
        int addedExamPeriods = 0;

        // Number of exam periods skipped because the same semester and moed already exist.
        int skippedDuplicateExamPeriods = 0;
    };

    /*
     * ValidationResult stores validation errors found before schedule generation.
     *
     * The scheduler should run only when the session has all required data:
     * courses, exam periods, and at least one selected program.
     */
    struct ValidationResult {
        // A list of human-readable validation errors.
        std::vector<std::string> errors;

        // Returns true only when no validation errors were found.
        bool isValid() const;

        // Joins all validation errors into one readable message.
        std::string message() const;
    };

    // Maximum number of academic programs that can be selected for one scheduling run.
    static constexpr std::size_t MaxSelectedPrograms = 5;

    // Creates an empty scheduling session.
    SchedulingSession() = default;

    /*
     * Replaces all currently loaded data.
     *
     * This is used when the user chooses to load a new dataset
     * and discard the previous courses and exam periods.
     */
    void replaceData(std::vector<Course> courses,
                     std::vector<ExamPeriod> examPeriods);

    /*
     * Replaces only the exam periods.
     *
     * This is useful after calendar editing, where the course list stays the same
     * but the available exam periods may have changed.
     */
    void replaceExamPeriods(std::vector<ExamPeriod> examPeriods);
    
    /*
     * Appends courses and exam periods to the existing session data.
     *
     * Duplicate courses and duplicate exam periods are skipped instead of inserted.
     */
    AppendResult appendData(const std::vector<Course>& courses,
                            const std::vector<ExamPeriod>& examPeriods);

    // Clears all session data, including selected programs.
    void clear();

    /*
     * Adds a program to the selected program list.
     *
     * Returns false if the program is empty, already selected,
     * or if the maximum number of selected programs was reached.
     */
    bool selectProgram(const std::string& programId);

    /*
     * Removes a program from the selected program list.
     *
     * Returns false if the program was not selected.
     */
    bool deselectProgram(const std::string& programId);

    // Returns true if the given program is currently selected.
    bool isProgramSelected(const std::string& programId) const;

    // Returns all loaded courses.
    const std::vector<Course>& courses() const;

    // Returns all loaded exam periods.
    const std::vector<ExamPeriod>& examPeriods() const;

    // Returns all conflicts rules.
    const std::vector<IConflictRule>& conflictRules() const;

    // Returns all currently selected academic programs.
    const std::vector<std::string>& selectedPrograms() const;

    // Returns the number of loaded courses.
    int courseCount() const;

    // Returns the number of loaded exam periods.
    int examPeriodCount() const;

    // Returns the number of selected academic programs.
    int selectedProgramCount() const;

    // Returns true if the session contains any loaded courses or exam periods.
    bool hasData() const;
    
    /*
     * Validates whether the session is ready for schedule generation.
     *
     * This method protects the scheduling flow from running with missing data.
     */
    ValidationResult validateBeforeGeneration() const;

    // Getters and setters for scheduling settings
    void setSettings(const ScheduleSettings& settings) { m_settings = settings; }
    ScheduleSettings getSettings() const { return m_settings; }

private:
    // All courses currently loaded into the application.
    std::vector<Course> courses_;

    // All exam periods currently loaded or edited in the application.
    std::vector<ExamPeriod> examPeriods_;

    // Settings for the scheduling rules
    ScheduleSettings m_settings;

    // Program IDs selected by the user for scheduling.
    std::vector<std::string> selectedPrograms_;

    // Returns true if a course with the given course number already exists.
    bool hasCourse(const std::string& courseNumber) const;

    // Returns true if an exam period with the same semester and moed already exists.
    bool hasExamPeriod(Semester semester, Moed moed) const;
};