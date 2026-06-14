#pragma once

#include "domain/Course.h"
#include "domain/ExamPeriod.h"

#include <cstddef>
#include <string>
#include <vector>

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
    struct AppendResult {
        int addedCourses = 0;
        int skippedDuplicateCourses = 0;
        int addedExamPeriods = 0;
        int skippedDuplicateExamPeriods = 0;
    };

    struct ValidationResult {
        std::vector<std::string> errors;

        bool isValid() const;
        std::string message() const;
    };

    static constexpr std::size_t MaxSelectedPrograms = 5;

    SchedulingSession() = default;

    void replaceData(std::vector<Course> courses,
                     std::vector<ExamPeriod> examPeriods);

    void replaceExamPeriods(std::vector<ExamPeriod> examPeriods);
    
    AppendResult appendData(const std::vector<Course>& courses,
                            const std::vector<ExamPeriod>& examPeriods);

    void clear();

    bool selectProgram(const std::string& programId);
    bool deselectProgram(const std::string& programId);
    bool isProgramSelected(const std::string& programId) const;

    const std::vector<Course>& courses() const;
    const std::vector<ExamPeriod>& examPeriods() const;
    const std::vector<std::string>& selectedPrograms() const;

    int courseCount() const;
    int examPeriodCount() const;
    int selectedProgramCount() const;

    bool hasData() const;
    
    ValidationResult validateBeforeGeneration() const;

private:
    std::vector<Course> courses_;
    std::vector<ExamPeriod> examPeriods_;
    std::vector<std::string> selectedPrograms_;

    bool hasCourse(const std::string& courseNumber) const;
    bool hasExamPeriod(Semester semester, Moed moed) const;
};