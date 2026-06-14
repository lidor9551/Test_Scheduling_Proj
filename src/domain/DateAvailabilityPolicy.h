#pragma once

#include "domain/Date.h"
#include "domain/ExamPeriod.h"

#include <vector>

/*
 * DateAvailabilityPolicy centralizes the rules that decide whether a date
 * can be used for an exam.
 *
 * This policy is shared by multiple parts of the system:
 * - ExamPeriod::allowedDates()
 * - CalendarManager
 * - ScheduleOutputManager
 * - Scheduling preprocessing
 *
 * The purpose is to make sure that the UI and the scheduling algorithm agree
 * on the same set of available and blocked dates.
 */
class DateAvailabilityPolicy {
public:
    /*
     * Returns true if the given date is a Saturday.
     *
     * Saturdays are blocked by system policy and should not be used
     * for exam scheduling.
     */
    static bool isSaturday(const Date& date);

    /*
     * Returns true if the date is inside one of the excluded ranges.
     *
     * Excluded ranges are manually or automatically blocked date intervals
     * inside an exam period.
     */
    static bool isInExcludedRanges(const Date& date,
                                   const std::vector<ExcludedRange>& excludedRanges);

    /*
     * Returns true if the date can be used for an exam.
     *
     * A date is allowed only when:
     * - it is not Saturday
     * - it is not inside an excluded range
     */
    static bool isAllowedExamDate(const Date& date,
                                  const std::vector<ExcludedRange>& excludedRanges);

    /*
     * Builds the full list of allowed dates between startDate and endDate.
     *
     * The range is inclusive:
     * startDate <= date <= endDate.
     */
    static std::vector<Date> allowedDates(const Date& startDate,
                                          const Date& endDate,
                                          const std::vector<ExcludedRange>& excludedRanges);
};