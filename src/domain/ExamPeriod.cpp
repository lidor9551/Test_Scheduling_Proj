#include "ExamPeriod.h"
#include "domain/DateAvailabilityPolicy.h"

bool ExcludedRange::contains(const Date& date) const {
    return start <= date && date <= end;
}

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

Semester ExamPeriod::getSemester() const {
    return semester;
}

Moed ExamPeriod::getMoed() const {
    return moed;
}

const Date& ExamPeriod::getStartDate() const {
    return startDate;
}

const Date& ExamPeriod::getEndDate() const {
    return endDate;
}

const std::vector<ExcludedRange>& ExamPeriod::getExcludedRanges() const {
    return excludedRanges;
}

// === Logic ===

std::vector<Date> ExamPeriod::allowedDates() const {
    return DateAvailabilityPolicy::allowedDates(
        startDate,
        endDate,
        excludedRanges
    );
}