#include "Preprocessor.h"
#include <stdexcept>
#include <QDebug>

SchedulingPreprocessor::SchedulingPreprocessor(
    const std::vector<Course>& courses,
    const std::vector<ExamPeriod>& periods,
    const std::vector<std::string>& selectedPrograms
) : courses_(courses),
    periods_(periods),
    selectedPrograms_(selectedPrograms.begin(), selectedPrograms.end()) {}

std::string SchedulingPreprocessor::semesterToString(Semester sem) const {
    switch (sem) {
        case Semester::FALL: return "FALL";
        case Semester::SPRI: return "SPRI";
        case Semester::SUMM: return "SUMM";
        default: return "";
    }
}

std::vector<SchedulingBlock> SchedulingPreprocessor::buildBlocks() const {
    std::vector<SchedulingBlock> blocks;

    for (const ExamPeriod& period : periods_) {
        // Fetch only relevant courses for this period
        std::vector<RuntimeCourse> runtimeCourses = selectRuntimeCoursesForPeriod(period);
        if (runtimeCourses.empty()) {
            continue;
        }

        std::vector<Date> allowedDates = period.allowedDates();
        if (allowedDates.empty()) {
            throw std::runtime_error(
                "No allowed dates remain for period " + period.getSemester() + "/" + period.getMoed()
            );
        }

        blocks.push_back(SchedulingBlock{
            period.getSemester(),
            period.getMoed(),
            allowedDates,
            runtimeCourses
        });
    }
    qDebug() << ">>> Number of blocks built:" << blocks.size(); 

    if (blocks.empty()) {
        qDebug() << ">>> ERROR: Blocks are empty! Stopping generation."; 
        return{};
    }

    return blocks;
}

std::vector<RuntimeCourse> SchedulingPreprocessor::selectRuntimeCoursesForPeriod(const ExamPeriod& period) const {
    std::vector<RuntimeCourse> runtimeCourses;
    std::map<std::pair<std::string, int>, int> groupIndex;

    for (const Course& course : courses_) {
        // This is where your OOP Model shines: instantly filter non-exam courses
        if (!course.requiresExam()) {
            continue;
        }

        std::vector<CourseMembership> memberships;
        for (const auto& offering : course.getPrograms()) {
            // Filter by explicitly selected programs
            if (selectedPrograms_.count(offering.programID) == 0) {
                continue;
            }
            // Filter by semester
            if (semesterToString(offering.semester) != period.getSemester()) {
                continue;
            }

            // Map (ProgramID, Year) to a unique integer ID to optimize collision checks later
            std::pair<std::string, int> key = {offering.programID, offering.year};
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