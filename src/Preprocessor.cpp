#include "Preprocessor.h"
#include "Parser.h"
#include <map>

SchedulingPreprocessor::SchedulingPreprocessor(
    const std::vector<Course>& courses,
    const std::vector<ExamPeriod>& periods,
    const std::vector<std::string>& selectedPrograms
) : courses_(courses),
    periods_(periods),
    selectedPrograms_(selectedPrograms.begin(), selectedPrograms.end()) {}

std::vector<SchedulingBlock> SchedulingPreprocessor::buildBlocks() const {
    std::vector<SchedulingBlock> blocks;

    for (const ExamPeriod& period : periods_) {
        std::vector<RuntimeCourse> runtimeCourses = selectRuntimeCoursesForPeriod(period);
        if (runtimeCourses.empty()) {
            continue;
        }

        std::vector<Date> allowedDates = period.allowedDates();
        if (allowedDates.empty()) {
            throw ParseException(
                "No allowed dates remain for period " + period.semester() + "/" + period.moed()
            );
        }

        blocks.push_back(SchedulingBlock{
            period.semester(),
            period.moed(),
            allowedDates,
            runtimeCourses
        });
    }

    return blocks;
}

std::vector<RuntimeCourse> SchedulingPreprocessor::selectRuntimeCoursesForPeriod(const ExamPeriod& period) const {
    std::vector<RuntimeCourse> runtimeCourses;
    std::map<std::pair<std::string, int>, int> groupIndex;

    for (const Course& course : courses_) {
        if (course.evaluation() != EvaluationType::Exam) {
            continue;
        }

        std::vector<CourseMembership> memberships;
        for (const ProgramOffering& offering : course.offerings()) {
            if (selectedPrograms_.count(offering.programId) == 0) {
                continue;
            }
            if (offering.semester != period.semester()) {
                continue;
            }

            std::pair<std::string, int> key = {offering.programId, offering.year};
            auto inserted = groupIndex.emplace(key, static_cast<int>(groupIndex.size()));
            memberships.push_back(CourseMembership{inserted.first->second, offering.requirement});
        }

        if (!memberships.empty()) {
            runtimeCourses.push_back(RuntimeCourse{
                static_cast<int>(runtimeCourses.size()),
                &course,
                memberships
            });
        }
    }

    return runtimeCourses;
}
