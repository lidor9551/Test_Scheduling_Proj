#include "AdvancedConflictRules.h"

#include <algorithm>

namespace {

bool hasMembership(const RuntimeCourse& course, int groupId, Requirement requirement) {
    return std::any_of(
        course.memberships.begin(),
        course.memberships.end(),
        [groupId, requirement](const CourseMembership& membership) {
            return membership.groupId == groupId &&
                   membership.requirement == requirement;
        }
    );
}

bool hasAnyMembershipInGroup(const RuntimeCourse& course, int groupId) {
    return std::any_of(
        course.memberships.begin(),
        course.memberships.end(),
        [groupId](const CourseMembership& membership) {
            return membership.groupId == groupId;
        }
    );
}

} // namespace

// ============================================================================
// Rule 2.1: Minimum days between obligatory exams
// ============================================================================
MinDaysObligatoryRule::MinDaysObligatoryRule(
    int k,
    const std::vector<Date>& dates,
    const std::vector<RuntimeCourse>& allCourses)
    : m_k(k), m_dates(dates), m_allCourses(allCourses) {}

bool MinDaysObligatoryRule::isSatisfied(
    const SchedulingState& state,
    const RuntimeCourse& course,
    int dateIndex) const {

    const Date candidateDate = m_dates[dateIndex];

    for (const CourseMembership& currentMembership : course.memberships) {
        if (currentMembership.requirement != Requirement::OBLIGATORY) {
            continue;
        }

        for (std::size_t i = 0; i < state.assignedDate.size(); ++i) {
            if (i >= m_allCourses.size()) {
                break;
            }

            if (static_cast<int>(i) == course.id) {
                continue;
            }

            const int assignedDateIdx = state.assignedDate[i];
            if (assignedDateIdx == -1) {
                continue;
            }

            const RuntimeCourse& assignedCourse = m_allCourses[i];
            if (hasMembership(assignedCourse,
                              currentMembership.groupId,
                              Requirement::OBLIGATORY)) {

                const int daysDiff = candidateDate.daysTo(m_dates[assignedDateIdx]);

                if (daysDiff < m_k) {
                    return false;
                }
            }
        }
    }

    return true;
}

// ============================================================================
// Rule 2.2: Minimum days between ANY exams (Obligatory or Elective)
// ============================================================================
MinDaysAllRule::MinDaysAllRule(
    int k,
    const std::vector<Date>& dates,
    const std::vector<RuntimeCourse>& allCourses)
    : m_k(k), m_dates(dates), m_allCourses(allCourses) {}

bool MinDaysAllRule::isSatisfied(
    const SchedulingState& state,
    const RuntimeCourse& course,
    int dateIndex) const {

    const Date candidateDate = m_dates[dateIndex];

    for (const CourseMembership& currentMembership : course.memberships) {
        for (std::size_t i = 0; i < state.assignedDate.size(); ++i) {
            if (i >= m_allCourses.size()) {
                break;
            }

            if (static_cast<int>(i) == course.id) {
                continue;
            }

            const int assignedDateIdx = state.assignedDate[i];
            if (assignedDateIdx == -1) {
                continue;
            }

            const RuntimeCourse& assignedCourse = m_allCourses[i];
            if (hasAnyMembershipInGroup(assignedCourse, currentMembership.groupId)) {
                const int daysDiff = candidateDate.daysTo(m_dates[assignedDateIdx]);

                if (daysDiff < m_k) {
                    return false;
                }
            }
        }
    }

    return true;
}

// ============================================================================
// Rule 2.3: Maximum conflicts (same day) for elective courses
// ============================================================================
MaxElectiveConflictsRule::MaxElectiveConflictsRule(
    int k,
    const std::vector<RuntimeCourse>& allCourses)
    : m_k(k), m_allCourses(allCourses) {}

bool MaxElectiveConflictsRule::isSatisfied(
    const SchedulingState& state,
    const RuntimeCourse& course,
    int dateIndex) const {

    for (const CourseMembership& currentMembership : course.memberships) {
        if (currentMembership.requirement != Requirement::ELECTIVE) {
            continue;
        }

        int sameDayElectiveCount = 0;

        for (std::size_t i = 0; i < state.assignedDate.size(); ++i) {
            if (i >= m_allCourses.size()) {
                break;
            }

            if (static_cast<int>(i) == course.id) {
                continue;
            }

            if (state.assignedDate[i] != dateIndex) {
                continue;
            }

            const RuntimeCourse& assignedCourse = m_allCourses[i];
            if (hasMembership(assignedCourse,
                              currentMembership.groupId,
                              Requirement::ELECTIVE)) {
                sameDayElectiveCount++;
            }
        }

        /*
         * Existing same-day elective exams are the number of new elective
         * conflicts that this candidate would introduce.
         *
         * Example:
         * - k = 0 allows the first elective exam on a day.
         * - k = 0 rejects a second elective exam on the same day.
         * - k = 1 allows a second elective exam, but rejects a third.
         */
        if (sameDayElectiveCount > m_k) {
            return false;
        }
    }

    return true;
}

// ============================================================================
// Rule 2.4: Minimum span (days) between the FIRST and LAST obligatory exams
// ============================================================================
ObligatorySpanRule::ObligatorySpanRule(
    int k,
    const std::vector<Date>& dates,
    const std::vector<RuntimeCourse>& allCourses)
    : m_k(k), m_dates(dates), m_allCourses(allCourses) {}

bool ObligatorySpanRule::isSatisfied(
    const SchedulingState& state,
    const RuntimeCourse& course,
    int dateIndex) const {

    for (const CourseMembership& currentMembership : course.memberships) {
        if (currentMembership.requirement != Requirement::OBLIGATORY) {
            continue;
        }

        int totalObligatoryInProgram = 0;
        int assignedObligatoryInProgram = 1; // includes the candidate assignment

        int earliestDateIdx = dateIndex;
        int latestDateIdx = dateIndex;

        for (std::size_t i = 0; i < m_allCourses.size(); ++i) {
            const RuntimeCourse& checkedCourse = m_allCourses[i];

            if (!hasMembership(checkedCourse,
                               currentMembership.groupId,
                               Requirement::OBLIGATORY)) {
                continue;
            }

            totalObligatoryInProgram++;

            if (checkedCourse.id == course.id ||
                i >= state.assignedDate.size() ||
                state.assignedDate[i] == -1) {
                continue;
            }

            assignedObligatoryInProgram++;

            const int assignedDate = state.assignedDate[i];

            if (assignedDate < earliestDateIdx) {
                earliestDateIdx = assignedDate;
            }

            if (assignedDate > latestDateIdx) {
                latestDateIdx = assignedDate;
            }
        }

        if (assignedObligatoryInProgram == totalObligatoryInProgram) {
            const int spanDays = m_dates[latestDateIdx].daysTo(m_dates[earliestDateIdx]);

            if (spanDays < m_k) {
                return false;
            }
        }
    }

    return true;
}

// ============================================================================
// Rule 2.5: Maximum total exams per day
// ============================================================================
MaxExamsPerDayRule::MaxExamsPerDayRule(int k) : m_k(k) {}

bool MaxExamsPerDayRule::isSatisfied(
    const SchedulingState& state,
    const RuntimeCourse& course,
    int dateIndex) const {

    int examsOnThisDate = 0;

    for (int assignedIdx : state.assignedDate) {
        if (assignedIdx == dateIndex) {
            examsOnThisDate++;
        }
    }

    if (examsOnThisDate >= m_k) {
        return false;
    }

    return true;
}