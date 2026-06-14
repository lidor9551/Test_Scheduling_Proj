#include "domain/DateAvailabilityPolicy.h"

/*
 * Checks whether a Date falls on Saturday.
 *
 * This uses Sakamoto's algorithm to calculate the day of week
 * without depending on Qt types such as QDate.
 *
 * The returned day index is:
 * 0 = Sunday, 1 = Monday, ..., 6 = Saturday.
 */
bool DateAvailabilityPolicy::isSaturday(const Date& date) {
    int year = date.getYear();
    const int month = date.getMonth();
    const int day = date.getDay();

    static const int monthOffsets[] = {
        0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4
    };

    if (month < 3) {
        year -= 1;
    }

    // Sakamoto algorithm:
    // 0 = Sunday, 1 = Monday, ..., 6 = Saturday.
    const int dayOfWeek =
        (year + year / 4 - year / 100 + year / 400 +
         monthOffsets[month - 1] + day) % 7;

    return dayOfWeek == 6;
}

/*
 * Checks whether the given date appears inside any excluded range.
 *
 * The range check itself is delegated to ExcludedRange::contains().
 */
bool DateAvailabilityPolicy::isInExcludedRanges(
    const Date& date,
    const std::vector<ExcludedRange>& excludedRanges) {
    for (const ExcludedRange& range : excludedRanges) {
        if (range.contains(date)) {
            return true;
        }
    }

    return false;
}

/*
 * Applies the complete date availability rule for one date.
 *
 * This is the main method that other layers should use when they need to know
 * whether an exam can be placed on a specific date.
 */
bool DateAvailabilityPolicy::isAllowedExamDate(
    const Date& date,
    const std::vector<ExcludedRange>& excludedRanges) {
    if (isSaturday(date)) {
        return false;
    }

    if (isInExcludedRanges(date, excludedRanges)) {
        return false;
    }

    return true;
}

/*
 * Builds all allowed exam dates inside a date range.
 *
 * The loop checks every date from startDate to endDate, including both ends.
 * Each date is accepted only if it passes isAllowedExamDate().
 */
std::vector<Date> DateAvailabilityPolicy::allowedDates(
    const Date& startDate,
    const Date& endDate,
    const std::vector<ExcludedRange>& excludedRanges) {
    std::vector<Date> result;

    for (Date current = startDate; current <= endDate; current = current.nextDay()) {
        if (isAllowedExamDate(current, excludedRanges)) {
            result.push_back(current);
        }
    }

    return result;
}