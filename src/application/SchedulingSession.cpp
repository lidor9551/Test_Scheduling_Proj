#include "application/SchedulingSession.h"

#include <algorithm>
#include <utility>

void SchedulingSession::replaceData(std::vector<Course> courses,
                                    std::vector<ExamPeriod> examPeriods) {
    courses_ = std::move(courses);
    examPeriods_ = std::move(examPeriods);
}

void SchedulingSession::replaceExamPeriods(std::vector<ExamPeriod> examPeriods) {
    examPeriods_ = std::move(examPeriods);
}

SchedulingSession::AppendResult SchedulingSession::appendData(
    const std::vector<Course>& courses,
    const std::vector<ExamPeriod>& examPeriods) {
    AppendResult result;

    for (const Course& course : courses) {
        if (hasCourse(course.getCourseNumber())) {
            result.skippedDuplicateCourses++;
            continue;
        }

        courses_.push_back(course);
        result.addedCourses++;
    }

    for (const ExamPeriod& period : examPeriods) {
        if (hasExamPeriod(period.getSemester(), period.getMoed())) {
            result.skippedDuplicateExamPeriods++;
            continue;
        }

        examPeriods_.push_back(period);
        result.addedExamPeriods++;
    }

    return result;
}

void SchedulingSession::clear() {
    courses_.clear();
    examPeriods_.clear();
    selectedPrograms_.clear();
}

bool SchedulingSession::selectProgram(const std::string& programId) {
    if (programId.empty() || isProgramSelected(programId)) {
        return false;
    }

    if (selectedPrograms_.size() >= MaxSelectedPrograms) {
        return false;
    }

    selectedPrograms_.push_back(programId);
    return true;
}

bool SchedulingSession::deselectProgram(const std::string& programId) {
    auto it = std::find(selectedPrograms_.begin(),
                        selectedPrograms_.end(),
                        programId);

    if (it == selectedPrograms_.end()) {
        return false;
    }

    selectedPrograms_.erase(it);
    return true;
}

bool SchedulingSession::isProgramSelected(const std::string& programId) const {
    return std::find(selectedPrograms_.begin(),
                     selectedPrograms_.end(),
                     programId) != selectedPrograms_.end();
}

const std::vector<Course>& SchedulingSession::courses() const {
    return courses_;
}

const std::vector<ExamPeriod>& SchedulingSession::examPeriods() const {
    return examPeriods_;
}

const std::vector<std::string>& SchedulingSession::selectedPrograms() const {
    return selectedPrograms_;
}

int SchedulingSession::courseCount() const {
    return static_cast<int>(courses_.size());
}

int SchedulingSession::examPeriodCount() const {
    return static_cast<int>(examPeriods_.size());
}

int SchedulingSession::selectedProgramCount() const {
    return static_cast<int>(selectedPrograms_.size());
}

bool SchedulingSession::hasData() const {
    return !courses_.empty() || !examPeriods_.empty();
}

bool SchedulingSession::hasCourse(const std::string& courseNumber) const {
    return std::any_of(courses_.begin(),
                       courses_.end(),
                       [&courseNumber](const Course& course) {
                           return course.getCourseNumber() == courseNumber;
                       });
}

bool SchedulingSession::hasExamPeriod(Semester semester, Moed moed) const {
    return std::any_of(examPeriods_.begin(),
                       examPeriods_.end(),
                       [semester, moed](const ExamPeriod& period) {
                           return period.getSemester() == semester &&
                                  period.getMoed() == moed;
                       });
}