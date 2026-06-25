#pragma once

#include "scheduling/IConflictRule.h"
#include "scheduling/SchedulingState.h"
#include "scheduling/IReadOnlySchedule.h"
#include "domain/Course.h"
#include "domain/Date.h"
#include <vector>

/*
 * AdvancedConflictRules.h
 * Declarations for the Version 3 hard constraint rules.
 */

// Rule 2.1: Minimum days between obligatory exams
class MinDaysObligatoryRule : public IConflictRule {
private:
    int m_k;
    const std::vector<Date>& m_dates;
    const std::vector<RuntimeCourse>& m_allCourses;

public:
    MinDaysObligatoryRule(int k, const std::vector<Date>& dates, const std::vector<RuntimeCourse>& allCourses);
    bool isSatisfied(const IReadOnlySchedule& schedule, const RuntimeCourse& course, int dateIndex) const override;
};

// Rule 2.2: Minimum days between ANY exams (Obligatory or Elective)
class MinDaysAllRule : public IConflictRule {
private:
    int m_k;
    const std::vector<Date>& m_dates;
    const std::vector<RuntimeCourse>& m_allCourses;

public:
    MinDaysAllRule(int k, const std::vector<Date>& dates, const std::vector<RuntimeCourse>& allCourses);
    bool isSatisfied(const IReadOnlySchedule& schedule, const RuntimeCourse& course, int dateIndex) const override;
};

// Rule 2.3: Maximum conflicts (same day) for elective courses
class MaxElectiveConflictsRule : public IConflictRule {
private:
    int m_k;
    const std::vector<RuntimeCourse>& m_allCourses;

public:
    MaxElectiveConflictsRule(int k, const std::vector<RuntimeCourse>& allCourses);
    bool isSatisfied(const IReadOnlySchedule& schedule, const RuntimeCourse& course, int dateIndex) const override;
};

// Rule 2.4: Minimum span (days) between the FIRST and LAST obligatory exams
class ObligatorySpanRule : public IConflictRule {
private:
    int m_k;
    const std::vector<Date>& m_dates;
    const std::vector<RuntimeCourse>& m_allCourses;

public:
    ObligatorySpanRule(int k, const std::vector<Date>& dates, const std::vector<RuntimeCourse>& allCourses);
    bool isSatisfied(const IReadOnlySchedule& schedule, const RuntimeCourse& course, int dateIndex) const override;
};

// Rule 2.5: Maximum total exams per day
class MaxExamsPerDayRule : public IConflictRule {
private:
    int m_k;

public:
    MaxExamsPerDayRule(int k);
    bool isSatisfied(const IReadOnlySchedule& schedule, const RuntimeCourse& course, int dateIndex) const override;
};