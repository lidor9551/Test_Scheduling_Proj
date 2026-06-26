#include "application/MoveValidationService.h"

#include "presentation/DragAndDropAdapter.h"

#include "scheduling/SameGroupConflictRule.h"
#include "scheduling/AdvancedConflictRules.h"

MoveValidationResult MoveValidationService::validateMove(
    const ScheduleGenerationResult& tempScheduleCopy,
    const std::string& courseId,
    const Date& targetDate,
    const std::vector<Date>& allowedDates,
    const std::vector<SchedulingBlock>& blocks,
    const ScheduleSettings& settings
) const {
    int dateIndex = -1;

    for (std::size_t i = 0; i < allowedDates.size(); ++i) {
        if (allowedDates[i].toString() == targetDate.toString()) {
            dateIndex = static_cast<int>(i);
            break;
        }
    }

    if (dateIndex == -1) {
        return {
            false,
            "Target date is not part of the active scheduling dates.",
            "DateBoundary"
        };
    }

    std::vector<RuntimeCourse> allCourses;
    for (const auto& block : blocks) {
        allCourses.insert(
            allCourses.end(),
            block.runtimeCourses.begin(),
            block.runtimeCourses.end()
        );
    }

    ScheduleGenerationResult simulatedSchedule = tempScheduleCopy;

    // Remove the moved course from its original date before testing the new date.
    simulatedSchedule.removeAssignment(courseId);

    DragAndDropAdapter uiAdapter(
        simulatedSchedule,
        allowedDates,
        blocks,
        courseId
    );

    RuntimeCourse courseToMove;
    bool foundCourse = false;

    for (const auto& block : blocks) {
        for (const auto& rc : block.runtimeCourses) {
            if (rc.course != nullptr &&
                rc.course->getCourseNumber() == courseId) {
                courseToMove = rc;
                foundCourse = true;
                break;
            }
        }

        if (foundCourse) {
            break;
        }
    }

    if (!foundCourse) {
        return {
            false,
            "Course was not found in the active scheduling block.",
            "CourseNotFound"
        };
    }

    SameGroupConflictRule hardRule;
    if (!hardRule.isSatisfied(uiAdapter, courseToMove, dateIndex)) {
        return {
            false,
            "Move violates same-group conflict rule.",
            "SameGroupConflictRule"
        };
    }

    if (settings.minDaysObligatory.isActive) {
        MinDaysObligatoryRule rule(
            settings.minDaysObligatory.k,
            allowedDates,
            allCourses
        );

        if (!rule.isSatisfied(uiAdapter, courseToMove, dateIndex)) {
            return {
                false,
                "Move violates minimum days between obligatory exams.",
                "MinDaysObligatoryRule"
            };
        }
    }

    if (settings.minDaysAll.isActive) {
        MinDaysAllRule rule(
            settings.minDaysAll.k,
            allowedDates,
            allCourses
        );

        if (!rule.isSatisfied(uiAdapter, courseToMove, dateIndex)) {
            return {
                false,
                "Move violates minimum days between exams.",
                "MinDaysAllRule"
            };
        }
    }

    if (settings.maxElectiveConflicts.isActive) {
        MaxElectiveConflictsRule rule(
            settings.maxElectiveConflicts.k,
            allCourses
        );

        if (!rule.isSatisfied(uiAdapter, courseToMove, dateIndex)) {
            return {
                false,
                "Move violates maximum elective conflicts rule.",
                "MaxElectiveConflictsRule"
            };
        }
    }

    if (settings.obligatorySpan.isActive) {
        ObligatorySpanRule rule(
            settings.obligatorySpan.k,
            allowedDates,
            allCourses
        );

        if (!rule.isSatisfied(uiAdapter, courseToMove, dateIndex)) {
            return {
                false,
                "Move violates obligatory exams span rule.",
                "ObligatorySpanRule"
            };
        }
    }

    if (settings.maxExamsPerDay.isActive) {
        MaxExamsPerDayRule rule(settings.maxExamsPerDay.k);

        if (!rule.isSatisfied(uiAdapter, courseToMove, dateIndex)) {
            return {
                false,
                "Move violates maximum exams per day rule.",
                "MaxExamsPerDayRule"
            };
        }
    }

    return {
        true,
        "",
        ""
    };
}