#include "MetricsCalculator.h"
#include <algorithm>
#include <cmath>


ScheduleMetrics MetricsCalculator::calculate(const ScheduleGenerationResult& result, const SchedulingBlock& block) {
    ScheduleMetrics metrics;

    // Data structures to hold mapped schedule data for efficient processing.
    // We group dates by 'groupId' to evaluate constraints per student track.
    std::map<int, std::vector<Date>> groupMandatoryDates;
    std::map<int, std::vector<Date>> groupAllDates;
    
    // We use a string representation of the Date as a key to avoid requiring 
    // a custom 'operator<' for the Date class in std::map.
    std::map<std::string, int> totalExamsPerDay;
    std::map<int, std::map<std::string, int>> electiveExamsPerGroupPerDay;

    // Step 1: Data Preparation (Single Pass)
    // We iterate through the raw assignments only once to populate our maps.
    // This ensures high performance even with thousands of generated schedules.
    for (const auto& assignment : result.getAssignments()) { 
        std::string dateKey = assignment.examDate.toString(); 
        
        // Track the absolute number of exams taking place on any given day
        totalExamsPerDay[dateKey]++;

        // Find the matching RuntimeCourse in the original scheduling block
        // to retrieve its specific group memberships and requirements.
        const RuntimeCourse* runtimeCourse = nullptr;
        for (const auto& rc : block.runtimeCourses) {
            if (rc.course == assignment.course) {
                runtimeCourse = &rc;
                break;
            }
        }
        
        // Safety check, should not happen in a valid result
        if (!runtimeCourse) continue;

        // Distribute the exam date into the relevant group timelines
        for (const auto& membership : runtimeCourse->memberships) {
            // Every exam is added to the general timeline for this group
            groupAllDates[membership.groupId].push_back(assignment.examDate);
            
            if (membership.requirement == Requirement::OBLIGATORY) {
                // Track separately for mandatory-only metrics (e.g., minimum days between mandatory)
                groupMandatoryDates[membership.groupId].push_back(assignment.examDate);
            } else if (membership.requirement == Requirement::ELECTIVE) {
                // Track elective conflicts per day for this specific group
                electiveExamsPerGroupPerDay[membership.groupId][dateKey]++;
            }
        }
    }

    // Step 2: Delegation
    // Pass the prepared data structures to dedicated helper functions 
    // to calculate each specific metric autonomously.
    metrics.obligatorySpan           = calcObligatorySpan(groupMandatoryDates);
    metrics.avgDaysBetweenObligatory = calcAvgDaysObligatory(groupMandatoryDates);
    metrics.avgDaysBetweenAll        = calcAvgDaysAll(groupAllDates);
    metrics.totalElectiveConflicts   = calcTotalElectiveConflicts(electiveExamsPerGroupPerDay);
    metrics.maxExamsInSingleDay      = calcMaxExamsInSingleDay(totalExamsPerDay);

    return metrics;
}

// HELPER FUNCTIONS IMPLEMENTATION

/*
 * Calculates the maximum total time span (in days) from the first mandatory exam
 * to the last mandatory exam across all student groups.
 * A smaller span means the mandatory exams are more tightly packed.
 */
int MetricsCalculator::calcObligatorySpan(std::map<int, std::vector<Date>> groupMandatoryDates) {
    int maxSpan = 0;
    for (auto& [groupId, dates] : groupMandatoryDates) {
        if (dates.empty()) continue;
        
        // Sort chronologically to easily find the first and last dates
        std::sort(dates.begin(), dates.end());
        int currentSpan = dates.front().daysTo(dates.back());
        maxSpan = std::max(maxSpan, currentSpan);
    }
    return maxSpan;
}

/*
 * Calculates the average number of days between consecutive mandatory exams
 * across all student groups. Higher is usually better for student workload.
 */
double MetricsCalculator::calcAvgDaysObligatory(std::map<int, std::vector<Date>> groupMandatoryDates) {
    double totalGapsSum = 0.0;
    int gapsCount = 0;
    
    for (auto& [groupId, dates] : groupMandatoryDates) {
        if (dates.size() < 2) continue; // Need at least 2 exams to have a gap
        
        std::sort(dates.begin(), dates.end());
        for (size_t i = 1; i < dates.size(); ++i) {
            totalGapsSum += dates[i - 1].daysTo(dates[i]);
            gapsCount++;
        }
    }
    return (gapsCount > 0) ? (totalGapsSum / gapsCount) : 0.0;
}

/*
 * Calculates the average number of days between ANY consecutive exams
 * (mandatory or elective) across all student groups.
 */
double MetricsCalculator::calcAvgDaysAll(std::map<int, std::vector<Date>> groupAllDates) {
    double totalGapsSum = 0.0;
    int gapsCount = 0;
    
    for (auto& [groupId, dates] : groupAllDates) {
        if (dates.size() < 2) continue;
        
        std::sort(dates.begin(), dates.end());
        for (size_t i = 1; i < dates.size(); ++i) {
            totalGapsSum += dates[i - 1].daysTo(dates[i]);
            gapsCount++;
        }
    }
    return (gapsCount > 0) ? (totalGapsSum / gapsCount) : 0.0;
}

/*
 * Calculates the total number of elective exam conflicts.
 * A conflict occurs when a student group has more than 1 elective exam scheduled
 * on the exact same date. (e.g., 2 exams = 1 conflict, 3 exams = 2 conflicts).
 */
int MetricsCalculator::calcTotalElectiveConflicts(const std::map<int, std::map<std::string, int>>& electiveExams) {
    int conflicts = 0;
    for (const auto& [groupId, dateMap] : electiveExams) {
        for (const auto& [dateKey, count] : dateMap) {
            if (count > 1) {
                conflicts += (count - 1);
            }
        }
    }
    return conflicts;
}

/*
 * Calculates the maximum number of exams (from any group/track) that fall 
 * on a single date in the entire calendar. Helps identify institutional bottlenecks.
 */
int MetricsCalculator::calcMaxExamsInSingleDay(const std::map<std::string, int>& totalExamsPerDay) {
    int maxExams = 0;
    for (const auto& [dateKey, count] : totalExamsPerDay) {
        maxExams = std::max(maxExams, count);
    }
    return maxExams;
}