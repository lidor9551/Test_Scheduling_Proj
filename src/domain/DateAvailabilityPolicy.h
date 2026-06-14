#pragma once

#include "domain/Date.h"
#include "domain/ExamPeriod.h"

#include <vector>

/**
 * @brief Shared policy for deciding which dates are available for exams.
 *
 * The same policy is used by:
 * - calendar management
 * - scheduling preprocessing
 * - future holiday / special rule support
 */
class DateAvailabilityPolicy {
public:
    static bool isSaturday(const Date& date);

    static bool isInExcludedRanges(const Date& date,
                                   const std::vector<ExcludedRange>& excludedRanges);

    static bool isAllowedExamDate(const Date& date,
                                  const std::vector<ExcludedRange>& excludedRanges);

    static std::vector<Date> allowedDates(const Date& startDate,
                                          const Date& endDate,
                                          const std::vector<ExcludedRange>& excludedRanges);
};