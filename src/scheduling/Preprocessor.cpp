#include "Preprocessor.h"
#include <stdexcept>
#include <QDebug>

/*
 * Constructs the preprocessor.
 *
 * courses_ and periods_ are stored as references because this class only reads them.
 * selectedPrograms_ is converted into a set to make filtering by program ID efficient.
 */
SchedulingPreprocessor::SchedulingPreprocessor(
    const std::vector<Course>& courses,
    const std::vector<ExamPeriod>& periods,
    const std::vector<std::string>& selectedPrograms
) : courses_(courses),
    periods_(periods),
    selectedPrograms_(selectedPrograms.begin(), selectedPrograms.end()) {}

/*
 * Builds scheduling blocks for all loaded exam periods.
 *
 * Each block represents one independent scheduling problem:
 * one semester/moed period, its allowed dates, and the courses that need exams.
 */
std::vector<SchedulingBlock> SchedulingPreprocessor::buildBlocks() const {
    std::vector<SchedulingBlock> blocks;

    /*
     * Go over each exam period and try to build a scheduling block for it.
     */
    for (const ExamPeriod& period : periods_) {
        // Fetch only relevant courses for this period
        /*
         * Filter the global course list into courses that match this specific period.
         */
        std::vector<RuntimeCourse> runtimeCourses = selectRuntimeCoursesForPeriod(period);
        if (runtimeCourses.empty()) {
            continue;
        }

        /*
         * Ask the ExamPeriod for the dates that can actually be used for exams.
         * This includes date availability rules such as excluded ranges and Saturdays.
         */
        std::vector<Date> allowedDates = period.allowedDates();
        if (allowedDates.empty()) {
            /*
             * If a period has courses but no valid dates, scheduling cannot continue
             * for that period because there is nowhere to place the exams.
             */
            throw std::runtime_error(
                "No allowed dates remain for period "
                + semesterToString(period.getSemester())
                + "/"
                + moedToString(period.getMoed())
            );
        }

        /*
         * Package the period name, allowed dates, and runtime courses
         * into one SchedulingBlock for the generator.
         */
        blocks.push_back(SchedulingBlock{
            semesterToString(period.getSemester()),
            moedToString(period.getMoed()),
            allowedDates,
            runtimeCourses
        });
    } 

    /*
     * If no blocks were created, there is nothing meaningful to send to the solver.
     */
    if (blocks.empty()) {
        qWarning() << ">>> ERROR: Blocks are empty! Stopping generation.";
        return{};
    }

    return blocks;
}

/*
 * Selects and converts courses for a single exam period.
 *
 * The output is a list of RuntimeCourse objects, where each object is ready
 * for the backtracking scheduler.
 */
std::vector<RuntimeCourse> SchedulingPreprocessor::selectRuntimeCoursesForPeriod(const ExamPeriod& period) const {
    std::vector<RuntimeCourse> runtimeCourses;

    /*
     * Maps each academic group to a numeric ID.
     *
     * The key is:
     * - program ID
     * - academic year
     *
     * The value is the integer group ID used by SchedulingState.
     */
    std::map<std::pair<std::string, Year>, int> groupIndex;

    /*
     * Scan every loaded course and keep only courses relevant to this period.
     */
    for (const Course& course : courses_) {
        // This is where your OOP Model shines: instantly filter non-exam courses
        /*
         * Courses evaluated by project or attendance are ignored,
         * because only courses with exams need exam dates.
         */
        if (!course.requiresExam()) {
            continue;
        }

        /*
         * Collect the relevant memberships for this course.
         *
         * A course may appear in several programs, years, and semesters,
         * but only some of them may be relevant to this scheduling run.
         */
        std::vector<CourseMembership> memberships;
        for (const auto& offering : course.getPrograms()) {
            // Filter by explicitly selected programs
            /*
             * Ignore programs that the user did not select.
             */
            if (selectedPrograms_.count(offering.programID) == 0) {
                continue;
            }
            // Filter by semester using the strongly typed enum value.
            /*
             * Ignore course offerings that belong to a different semester
             * than the current exam period.
             */
            if (offering.semester != period.getSemester()) {
                continue;
            }

            // Map (ProgramID, Year) to a unique integer ID to optimize collision checks later.
            /*
             * Create or reuse a numeric group ID for this program/year pair.
             *
             * The group ID is later used as an index in conflict-count matrices.
             */
            std::pair<std::string, Year> key = {offering.programID, offering.year};
            auto inserted = groupIndex.emplace(key, static_cast<int>(groupIndex.size()));
            
            /*
             * Store the membership that connects this runtime course
             * to the academic group that takes it.
             */
            memberships.push_back(CourseMembership{inserted.first->second, offering.requirement});
        }

        /*
         * Add the course to the runtime list only if it has at least one
         * relevant membership after filtering.
         */
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