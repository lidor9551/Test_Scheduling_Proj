#include "MetricsCalculator.h"

#include <algorithm>

namespace {

int dateKey(const Date& date) {
    return date.getYear() * 10000 + date.getMonth() * 100 + date.getDay();
}

} // namespace

MetricsCalculator::PreparedBlockData MetricsCalculator::prepareBlockData(const SchedulingBlock& block) {
    PreparedBlockData data;
    data.membershipsByCourse.reserve(block.runtimeCourses.size());

    int maxGroupId = -1;
    for (const RuntimeCourse& runtimeCourse : block.runtimeCourses) {
        data.membershipsByCourse[runtimeCourse.course] = runtimeCourse.memberships;

        for (const CourseMembership& membership : runtimeCourse.memberships) {
            maxGroupId = std::max(maxGroupId, membership.groupId);
        }
    }

    data.groupCount = maxGroupId + 1;
    return data;
}

ScheduleMetrics MetricsCalculator::calculate(const ScheduleGenerationResult& result, const SchedulingBlock& block) {
    return calculate(result, prepareBlockData(block));
}

ScheduleMetrics MetricsCalculator::calculate(
    const ScheduleGenerationResult& result,
    const PreparedBlockData& blockData
) {
    ScheduleMetrics metrics;
    PreparedScheduleData scheduleData = prepareScheduleData(result, blockData);

    sortGroupDates(scheduleData.groupMandatoryDates);
    sortGroupDates(scheduleData.groupAllDates);

    metrics.obligatorySpan = calcObligatorySpan(scheduleData.groupMandatoryDates);
    metrics.avgDaysBetweenObligatory = calcAverageGap(scheduleData.groupMandatoryDates);
    metrics.avgDaysBetweenAll = calcAverageGap(scheduleData.groupAllDates);
    metrics.totalElectiveConflicts = calcTotalElectiveConflicts(scheduleData.electiveExamsPerGroupPerDay);
    metrics.maxExamsInSingleDay = calcMaxExamsInSingleDay(scheduleData.totalExamsPerDay);

    return metrics;
}

MetricsCalculator::PreparedScheduleData MetricsCalculator::prepareScheduleData(
    const ScheduleGenerationResult& result,
    const PreparedBlockData& blockData
) {
    PreparedScheduleData data;
    data.groupMandatoryDates.resize(blockData.groupCount);
    data.groupAllDates.resize(blockData.groupCount);
    data.electiveExamsPerGroupPerDay.resize(blockData.groupCount);

    for (const ExamAssignment& assignment : result.getAssignments()) {
        const int assignmentDateKey = dateKey(assignment.examDate);
        ++data.totalExamsPerDay[assignmentDateKey];

        const auto membershipIt = blockData.membershipsByCourse.find(assignment.course);
        if (membershipIt == blockData.membershipsByCourse.end()) {
            continue;
        }

        const std::vector<CourseMembership>& memberships = membershipIt->second;
        for (const CourseMembership& membership : memberships) {
            if (membership.groupId < 0 || membership.groupId >= blockData.groupCount) {
                continue;
            }

            data.groupAllDates[static_cast<std::size_t>(membership.groupId)].push_back(assignment.examDate);

            if (membership.requirement == Requirement::OBLIGATORY) {
                data.groupMandatoryDates[static_cast<std::size_t>(membership.groupId)].push_back(assignment.examDate);
            } else if (membership.requirement == Requirement::ELECTIVE) {
                ++data.electiveExamsPerGroupPerDay[static_cast<std::size_t>(membership.groupId)][assignmentDateKey];
            }
        }
    }

    return data;
}

void MetricsCalculator::sortGroupDates(std::vector<std::vector<Date>>& groupDates) {
    for (std::vector<Date>& dates : groupDates) {
        std::sort(dates.begin(), dates.end());
    }
}

int MetricsCalculator::calcObligatorySpan(const std::vector<std::vector<Date>>& sortedGroupMandatoryDates) {
    int maxSpan = 0;

    for (const std::vector<Date>& dates : sortedGroupMandatoryDates) {
        if (dates.empty()) {
            continue;
        }

        maxSpan = std::max(maxSpan, dates.front().daysTo(dates.back()));
    }

    return maxSpan;
}

double MetricsCalculator::calcAverageGap(const std::vector<std::vector<Date>>& sortedGroupDates) {
    double totalGapsSum = 0.0;
    int gapsCount = 0;

    for (const std::vector<Date>& dates : sortedGroupDates) {
        if (dates.size() < 2) {
            continue;
        }

        for (std::size_t i = 1; i < dates.size(); ++i) {
            totalGapsSum += dates[i - 1].daysTo(dates[i]);
            ++gapsCount;
        }
    }

    return gapsCount > 0 ? totalGapsSum / gapsCount : 0.0;
}

int MetricsCalculator::calcTotalElectiveConflicts(
    const std::vector<std::unordered_map<int, int>>& electiveExams
) {
    int conflicts = 0;

    for (const std::unordered_map<int, int>& dateCounts : electiveExams) {
        for (const auto& dateCount : dateCounts) {
            if (dateCount.second > 1) {
                conflicts += dateCount.second - 1;
            }
        }
    }

    return conflicts;
}

int MetricsCalculator::calcMaxExamsInSingleDay(const std::unordered_map<int, int>& totalExamsPerDay) {
    int maxExams = 0;

    for (const auto& dateCount : totalExamsPerDay) {
        maxExams = std::max(maxExams, dateCount.second);
    }

    return maxExams;
}
