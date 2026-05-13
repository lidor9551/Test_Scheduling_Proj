#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "Domain.h"
#include <set>
#include <string>
#include <vector>

class SchedulingPreprocessor {
private:
    const std::vector<Course>& courses_;
    const std::vector<ExamPeriod>& periods_;
    std::set<std::string> selectedPrograms_;

    std::vector<RuntimeCourse> selectRuntimeCoursesForPeriod(const ExamPeriod& period) const;

public:
    SchedulingPreprocessor(
        const std::vector<Course>& courses,
        const std::vector<ExamPeriod>& periods,
        const std::vector<std::string>& selectedPrograms
    );

    std::vector<SchedulingBlock> buildBlocks() const;
};

#endif
