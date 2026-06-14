#include "ExamPeriod.h"
#include "domain/DateAvailabilityPolicy.h"

/*
 * Checks whether a date is inside this excluded range.
 *
 * The range is inclusive:
 * start <= date <= end.
 */
bool ExcludedRange::contains(const Date& date) const {
    return start <= date && date <= end;
}

/*
 * Creates an exam period.
 *
 * The excluded ranges are moved into the object to avoid unnecessary copying.
 */
ExamPeriod::ExamPeriod(Semester sem,
                       Moed m,
                       const Date& start,
                       const Date& end,
                       std::vector<ExcludedRange> excluded)
    : semester(sem),
      moed(m),
      startDate(start),
      endDate(end),
      excludedRanges(std::move(excluded)) {
}

// === Getters ===

// Returns the semester connected to this period.
Semester ExamPeriod::getSemester() const {
    return semester;
}

// Returns the moed connected to this period.
Moed ExamPeriod::getMoed() const {
    return moed;
}

// Returns the first date of this exam period.
const Date& ExamPeriod::getStartDate() const {
    return startDate;
}

// Returns the last date of this exam period.
const Date& ExamPeriod::getEndDate() const {
    return endDate;
}

// Returns the blocked date ranges for this period.
const std::vector<ExcludedRange>& ExamPeriod::getExcludedRanges() const {
    return excludedRanges;
}

// === Logic ===

/*
 * Builds the list of dates that can be used by the scheduler.
 *
 * The policy object centralizes all date availability rules.
 * This keeps the scheduler, calendar editor, and output view consistent.
 */
std::vector<Date> ExamPeriod::allowedDates() const {
    return DateAvailabilityPolicy::allowedDates(
        startDate,
        endDate,
        excludedRanges
    );
}