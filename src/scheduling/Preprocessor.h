#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include "domain/Course.h"
#include "domain/ExamPeriod.h"

/*
 * The preprocessor prepares clean scheduling input for the backtracking engine.
 *
 * It receives the full course list, all exam periods, and the selected programs.
 * Then it filters and converts that data into compact runtime structures
 * that are easier and faster for the scheduling algorithm to use.
 */

// Represents a specific group of students (Program + Year) taking the course
/*
 * CourseMembership connects a runtime course to one academic student group.
 *
 * A student group is represented by a unique numeric groupId.
 * This numeric ID allows the scheduler to check conflicts using fast vectors
 * instead of repeatedly comparing program strings and year values.
 */
struct CourseMembership {
    int groupId; // Unique ID to check for collisions (e.g., Program 83104, Year 2)
    Requirement requirement; // OBLIGATORY or ELECTIVE
};

// A stripped-down wrapper for a course during runtime scheduling
/*
 * RuntimeCourse is the scheduler-friendly representation of a Course.
 *
 * It keeps a pointer to the original Course object, but also adds:
 * - a compact internal ID for the algorithm
 * - all memberships relevant to the selected programs and exam period
 *
 * This avoids copying full Course objects while still preserving course identity.
 */
struct RuntimeCourse {
    int id; // Internal ID for the backtracking algorithm
    const Course* course; // Pointer to the original OOP Course (no copying!)
    std::vector<CourseMembership> memberships; // Which groups are taking it
};

// The final package sent to the Scheduler: One exam period and all its relevant courses & free days
/*
 * SchedulingBlock is one complete scheduling problem.
 *
 * Each block contains:
 * - one semester
 * - one moed
 * - all allowed dates for that exam period
 * - all runtime courses that should be scheduled in that period
 *
 * The ScheduleGenerator receives one SchedulingBlock and tries to create
 * all valid schedules for it.
 */
struct SchedulingBlock {
    std::string semester;
    std::string moed;
    std::vector<Date> allowedDates;
    std::vector<RuntimeCourse> runtimeCourses;
};

/*
 * SchedulingPreprocessor converts domain data into scheduling data.
 *
 * The domain layer stores rich objects such as Course and ExamPeriod.
 * The scheduling layer needs a more compact representation that is optimized
 * for backtracking and conflict checks.
 */
class SchedulingPreprocessor {
private:
    /*
     * Reference to all loaded courses.
     *
     * This class does not own the vector.
     * It only reads from it while building scheduling blocks.
     */
    const std::vector<Course>& courses_;

    /*
     * Reference to all loaded exam periods.
     *
     * Each period may become one SchedulingBlock if it has relevant courses.
     */
    const std::vector<ExamPeriod>& periods_;

    /*
     * Set of program IDs selected by the user.
     *
     * A set is used because membership checks are simple and fast.
     */
    std::set<std::string> selectedPrograms_;

    // Filters and prepares the courses specific to a given exam period
    /*
     * Selects only the courses that are relevant for one exam period.
     *
     * A course is included only if:
     * - it requires an exam
     * - it belongs to one of the selected programs
     * - it is taught in the same semester as the exam period
     */
    std::vector<RuntimeCourse> selectRuntimeCoursesForPeriod(const ExamPeriod& period) const;

public:
    /*
     * Creates a preprocessor from loaded courses, exam periods, and selected programs.
     *
     * The course and period vectors are stored by reference.
     * The selected programs are copied into a set for efficient filtering.
     */
    SchedulingPreprocessor(
        const std::vector<Course>& courses,
        const std::vector<ExamPeriod>& periods,
        const std::vector<std::string>& selectedPrograms
    );

    // Builds the scheduling blocks for all periods
    /*
     * Builds all scheduling blocks that should be passed to the scheduler.
     *
     * Periods without relevant runtime courses are skipped.
     */
    std::vector<SchedulingBlock> buildBlocks() const;
};