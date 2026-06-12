#pragma once

#include <string>
#include <vector>
#include <QMetaType>

#include "Course.h"
#include "../dates/Date.h"

struct ExcludedRange {
    Date start;
    Date end;
    std::string reason;

    bool contains(const Date& date) const;
};

class ExamPeriod {
private:
    Semester semester;
    Moed moed;
    Date startDate;
    Date endDate;
    std::vector<ExcludedRange> excludedRanges;

public:
    ExamPeriod() {}

    ExamPeriod(Semester sem,
               Moed m,
               const Date& start,
               const Date& end,
               std::vector<ExcludedRange> excluded);

    Semester getSemester() const;
    Moed getMoed() const;
    const Date& getStartDate() const;
    const Date& getEndDate() const;
    const std::vector<ExcludedRange>& getExcludedRanges() const;

    std::vector<Date> allowedDates() const;
};

Q_DECLARE_METATYPE(ExamPeriod)