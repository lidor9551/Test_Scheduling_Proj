#pragma once

#include "domain/ScheduleGenerationResult.h"
#include "scheduling/Preprocessor.h"
#include <unordered_map>

/**
 * @class MetricsCalculator
 * @brief A static utility class responsible for evaluating a completed schedule.
 * * Since all methods are static, instantiation is explicitly prevented by deleting 
 * the default constructor. This avoids the overhead of an abstract class.
 */
class MetricsCalculator {
public:
    // Disallow instantiation by deleting the default constructor
    MetricsCalculator() = delete;

    /*
     * Indexed, reusable metadata for one scheduling block.
     *
     * The generator can prepare this once per block and reuse it when computing
     * metrics for many generated solutions from the same block.
     */
    struct PreparedBlockData {
        std::unordered_map<const Course*, std::vector<CourseMembership>> membershipsByCourse;
        int groupCount = 0;
    };

    /**
     * @brief Computes all 5 soft constraint metrics for a given schedule result.
     * @param result The completed schedule containing exam assignments.
     * @param block The original scheduling problem block containing allowed dates and courses.
     * @return A filled ScheduleMetrics struct with the evaluated scores.
     */
    static ScheduleMetrics calculate(const ScheduleGenerationResult& result, const SchedulingBlock& block);

    /*
     * Prepares block metadata used by the optimized metrics calculation path.
     */
    static PreparedBlockData prepareBlockData(const SchedulingBlock& block);

    /*
     * Computes metrics using pre-indexed block metadata.
     */
    static ScheduleMetrics calculate(const ScheduleGenerationResult& result, const PreparedBlockData& blockData);
    
private:
    /*
     * Aggregated schedule data used to calculate all metrics in one pass.
     */
    struct PreparedScheduleData {
        std::vector<std::vector<Date>> groupMandatoryDates;
        std::vector<std::vector<Date>> groupAllDates;
        std::vector<std::unordered_map<int, int>> electiveExamsPerGroupPerDay;
        std::unordered_map<int, int> totalExamsPerDay;
    };

    static PreparedScheduleData prepareScheduleData(
        const ScheduleGenerationResult& result,
        const PreparedBlockData& blockData
    );

    static void sortGroupDates(std::vector<std::vector<Date>>& groupDates);
    static int calcObligatorySpan(const std::vector<std::vector<Date>>& sortedGroupMandatoryDates);
    static double calcAverageGap(const std::vector<std::vector<Date>>& sortedGroupDates);
    static int calcTotalElectiveConflicts(const std::vector<std::unordered_map<int, int>>& electiveExams);
    static int calcMaxExamsInSingleDay(const std::unordered_map<int, int>& totalExamsPerDay);
};
