#include "domain/DateAvailabilityPolicy.h"

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