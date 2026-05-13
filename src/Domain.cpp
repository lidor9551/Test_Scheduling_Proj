#include "Domain.h"
#include <stdexcept>

Course::Course(
    std::string name,
    std::string number,
    std::string instructor,
    std::vector<ProgramOffering> offerings,
    EvaluationType evaluation
) : name_(std::move(name)),
    number_(std::move(number)),
    instructor_(std::move(instructor)),
    offerings_(std::move(offerings)),
    evaluation_(evaluation) {}

const std::string& Course::name() const { return name_; }
const std::string& Course::number() const { return number_; }
const std::string& Course::instructor() const { return instructor_; }
const std::vector<ProgramOffering>& Course::offerings() const { return offerings_; }
EvaluationType Course::evaluation() const { return evaluation_; }

bool ExcludedRange::contains(const Date& date) const {
    return start <= date && date <= end;
}

ExamPeriod::ExamPeriod(
    std::string semester,
    std::string moed,
    Date startDate,
    Date endDate,
    std::vector<ExcludedRange> excludedRanges
) : semester_(std::move(semester)),
    moed_(std::move(moed)),
    startDate_(startDate),
    endDate_(endDate),
    excludedRanges_(std::move(excludedRanges)) {}

const std::string& ExamPeriod::semester() const { return semester_; }
const std::string& ExamPeriod::moed() const { return moed_; }
const Date& ExamPeriod::startDate() const { return startDate_; }
const Date& ExamPeriod::endDate() const { return endDate_; }
const std::vector<ExcludedRange>& ExamPeriod::excludedRanges() const { return excludedRanges_; }

std::vector<Date> ExamPeriod::allowedDates() const {
    std::vector<Date> result;
    for (Date current = startDate_; current <= endDate_; current = current.nextDay()) {
        bool excluded = false;
        for (const ExcludedRange& range : excludedRanges_) {
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

std::string toString(RequirementType type) {
    return type == RequirementType::Obligatory ? "Obligatory" : "Elective";
}

std::string toString(EvaluationType type) {
    switch (type) {
        case EvaluationType::Exam: return "Exam";
        case EvaluationType::Project: return "Project";
        case EvaluationType::Attendance: return "Attendance";
    }
    throw std::invalid_argument("Unknown evaluation type");
}

RequirementType parseRequirementType(const std::string& value) {
    if (value == "Obligatory") return RequirementType::Obligatory;
    if (value == "Elective") return RequirementType::Elective;
    throw std::invalid_argument("Unknown requirement type: " + value);
}

EvaluationType parseEvaluationType(const std::string& value) {
    if (value == "Exam") return EvaluationType::Exam;
    if (value == "Project") return EvaluationType::Project;
    if (value == "Attendance") return EvaluationType::Attendance;
    throw std::invalid_argument("Unknown evaluation type: " + value);
}
