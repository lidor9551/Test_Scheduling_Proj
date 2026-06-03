#pragma once
#include <string>
#include <vector>
#include <QMetaType>
#include "../dates/Date.h" 

struct ExcludedRange {
    Date start;
    Date end;
    std::string reason; 

    bool contains(const Date& date) const;
};

class ExamPeriod {
private:
    std::string semester;
    std::string moed;
    Date startDate;
    Date endDate;
    std::vector<ExcludedRange> excludedRanges;

public:
    ExamPeriod() {}

    ExamPeriod(std::string sem, std::string m, const Date& start, 
               const Date& end, std::vector<ExcludedRange> excluded);

    const std::string& getSemester() const;
    const std::string& getMoed() const;
    const Date& getStartDate() const;
    const Date& getEndDate() const;
    const std::vector<ExcludedRange>& getExcludedRanges() const;

    std::vector<Date> allowedDates() const;
};

Q_DECLARE_METATYPE(ExamPeriod)