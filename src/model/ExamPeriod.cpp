#include "ExamPeriod.h"

bool ExcludedRange::contains(const Date& date) const {
    return start <= date && date <= end;
}


ExamPeriod::ExamPeriod(std::string sem, std::string m, const Date& start, 
                       const Date& end, std::vector<ExcludedRange> excluded)
    : semester(std::move(sem)), moed(std::move(m)), 
      startDate(start), endDate(end), excludedRanges(std::move(excluded)) {
}

// === Getters ===

const std::string& ExamPeriod::getSemester() const { 
    return semester; 
}

const std::string& ExamPeriod::getMoed() const { 
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
    std::vector<Date> result;
    
    // all dates from startDate to endDate
    for (Date current = startDate; current <= endDate; current = current.nextDay()) {
        bool excluded = false;
        
        // check if current date is in any of the excluded ranges
        for (const ExcludedRange& range : excludedRanges) {
            if (range.contains(current)) {
                excluded = true;
                break;
            }
        }
        
        if (!excluded) {
            result.push_back(current);
        }
    }
    
    return result;
}