#pragma once

#include "domain/ScheduleGenerationResult.h"
#include "scheduling/Preprocessor.h"

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

    /**
     * @brief Computes all 5 soft constraint metrics for a given schedule result.
     * @param result The completed schedule containing exam assignments.
     * @param block The original scheduling problem block containing allowed dates and courses.
     * @return A filled ScheduleMetrics struct with the evaluated scores.
     */
    static ScheduleMetrics calculate(const ScheduleGenerationResult& result, const SchedulingBlock& block);
};