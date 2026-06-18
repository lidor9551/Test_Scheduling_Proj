#include "MetricsCalculator.h"
#include <map>
#include <algorithm>
#include <cmath>

ScheduleMetrics MetricsCalculator::calculate(const ScheduleGenerationResult& result, const SchedulingBlock& block) {
    ScheduleMetrics metrics;

    std::map<int, std::vector<Date>> groupMandatoryDates;
    std::map<int, std::vector<Date>> groupAllDates;
    
    // We use std::string as the map key to avoid 'operator<' issues with the Date object
    std::map<std::string, int> totalExamsPerDay;
    std::map<int, std::map<std::string, int>> electiveExamsPerGroupPerDay;

    // Map assignments back to their student groups
    for (const auto& assignment : result.getAssignments()) { 
        
        // Use a string representation of the date for the map key. 
        // If your Date class uses a different method to get a string, change 'toString()' accordingly.
        std::string dateKey = assignment.examDate.toString(); 
        
        totalExamsPerDay[dateKey]++;

        const RuntimeCourse* runtimeCourse = nullptr;
        for (const auto& rc : block.runtimeCourses) {
            if (rc.course == assignment.course) {
                runtimeCourse = &rc;
                break;
            }
        }
        if (!runtimeCourse) continue;

        for (const auto& membership : runtimeCourse->memberships) {
            groupAllDates[membership.groupId].push_back(assignment.examDate);
            
            if (membership.requirement == Requirement::OBLIGATORY) {
                groupMandatoryDates[membership.groupId].push_back(assignment.examDate);
            } else if (membership.requirement == Requirement::ELECTIVE) {
                electiveExamsPerGroupPerDay[membership.groupId][dateKey]++;
            }
        }
    }

    // Compute Metrics 2.1 & 2.4
    double totalMandatoryGapsSum = 0.0;
    int mandatoryGapsCount = 0;
    int maxObligatorySpan = 0;

    for (auto& [groupId, dates] : groupMandatoryDates) {
        if (dates.empty()) continue;
        
        // Assuming Date has operator< defined for sorting. If not, this might need a custom comparator.
        std::sort(dates.begin(), dates.end());

        int currentSpan = dates.front().daysTo(dates.back());
        maxObligatorySpan = std::max(maxObligatorySpan, currentSpan);

        for (size_t i = 1; i < dates.size(); ++i) {
            totalMandatoryGapsSum += dates[i - 1].daysTo(dates[i]);
            mandatoryGapsCount++;
        }
    }
    metrics.obligatorySpan = maxObligatorySpan;
    metrics.avgDaysBetweenObligatory = (mandatoryGapsCount > 0) ? (totalMandatoryGapsSum / mandatoryGapsCount) : 0.0;

    // Compute Metric 2.2
    double totalAllGapsSum = 0.0;
    int allGapsCount = 0;

    for (auto& [groupId, dates] : groupAllDates) {
        if (dates.size() < 2) continue;

        std::sort(dates.begin(), dates.end());
        for (size_t i = 1; i < dates.size(); ++i) {
            totalAllGapsSum += dates[i - 1].daysTo(dates[i]);
            allGapsCount++;
        }
    }
    metrics.avgDaysBetweenAll = (allGapsCount > 0) ? (totalAllGapsSum / allGapsCount) : 0.0;

    // Compute Metric 2.3
    int electiveConflicts = 0;
    for (const auto& [groupId, dateMap] : electiveExamsPerGroupPerDay) {
        for (const auto& [dateKey, count] : dateMap) {
            if (count > 1) {
                electiveConflicts += (count - 1);
            }
        }
    }
    metrics.totalElectiveConflicts = electiveConflicts;

    // Compute Metric 2.5
    int maxExamsDay = 0;
    for (const auto& [dateKey, count] : totalExamsPerDay) {
        maxExamsDay = std::max(maxExamsDay, count);
    }
    metrics.maxExamsInSingleDay = maxExamsDay;

    return metrics;
}