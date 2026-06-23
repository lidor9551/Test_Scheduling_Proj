#pragma once

#include "domain/Date.h"
#include "domain/ExamPeriod.h"

#include <vector>

namespace ScheduleAssertions {

inline bool dateInsidePeriod(const Date& date, const ExamPeriod& period) {
    return date >= period.getStartDate() && date <= period.getEndDate();
}

inline bool allDatesInsidePeriod(
    const std::vector<Date>& dates,
    const ExamPeriod& period
) {
    for (const Date& date : dates) {
        if (!dateInsidePeriod(date, period)) {
            return false;
        }
    }

    return true;
}

inline bool noDateInsideExcludedRange(
    const std::vector<Date>& dates,
    const ExcludedRange& range
) {
    for (const Date& date : dates) {
        if (range.contains(date)) {
            return false;
        }
    }

    return true;
}

} // namespace ScheduleAssertions