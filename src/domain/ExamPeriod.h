#pragma once

#include <string>
#include <vector>
#include <QMetaType>

#include "Course.h"
#include "Date.h"

/*
 * ExcludedRange represents a blocked date range inside an exam period.
 *
 * These ranges are used when specific dates should not be available for exams.
 * For example:
 * - manually blocked dates
 * - future holidays
 * - special institutional restrictions
 */
struct ExcludedRange {
    // First blocked date in the range.
    Date start;

    // Last blocked date in the range.
    Date end;

    // Optional explanation for why this range is blocked.
    std::string reason;

    // Returns true if the given date is inside this blocked range.
    bool contains(const Date& date) const;
};

/*
 * ExamPeriod represents one scheduling window for exams.
 *
 * Each period belongs to:
 * - a semester
 * - a moed
 * - a start date
 * - an end date
 * - optional excluded ranges
 *
 * The scheduling algorithm uses ExamPeriod to know which dates are available
 * for placing exams.
 */
class ExamPeriod {
private:
    // Semester connected to this exam period.
    Semester semester;

    // Moed connected to this exam period.
    Moed moed;

    // First date of the exam period.
    Date startDate;

    // Last date of the exam period.
    Date endDate;

    // Date ranges that are blocked inside this period.
    std::vector<ExcludedRange> excludedRanges;

public:
    // Default constructor is required for Qt QVariant and QMetaType support.
    ExamPeriod() {}

    // Creates an exam period with its semester, moed, date range, and exclusions.
    ExamPeriod(Semester sem,
               Moed m,
               const Date& start,
               const Date& end,
               std::vector<ExcludedRange> excluded);

    // Returns the semester of this exam period.
    Semester getSemester() const;

    // Returns the moed of this exam period.
    Moed getMoed() const;

    // Returns the first date of the period.
    const Date& getStartDate() const;

    // Returns the last date of the period.
    const Date& getEndDate() const;

    // Returns all blocked ranges defined for this period.
    const std::vector<ExcludedRange>& getExcludedRanges() const;

    /*
     * Returns all dates that are allowed for exam scheduling.
     *
     * The actual availability rules are delegated to DateAvailabilityPolicy.
     */
    std::vector<Date> allowedDates() const;
};

// Allows ExamPeriod objects to be stored in QVariant and passed to QML-facing code.
Q_DECLARE_METATYPE(ExamPeriod)