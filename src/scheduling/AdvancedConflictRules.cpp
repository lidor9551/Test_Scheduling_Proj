#pragma once

#include "scheduling/IConflictRule.h"
#include "scheduling/SchedulingState.h"
#include "domain/Course.h"
#include "domain/Date.h"
#include <vector>
#include <string>

/*
 * AdvancedConflictRules.h
 * * This file contains the implementations of the hard constraint rules (Version 3).
 * Each rule inherits from IConflictRule and is evaluated during the Backtracking 
 * algorithm's execution to prune invalid scheduling branches.
 */

// Rule 2.1: Minimum days between obligatory exams
class MinDaysObligatoryRule : public IConflictRule {
private:
    int m_k;
    const std::vector<Date>& m_dates;
    const std::vector<RuntimeCourse>& m_allCourses;

public:
    MinDaysObligatoryRule(int k, const std::vector<Date>& dates, const std::vector<RuntimeCourse>& allCourses)
        : m_k(k), m_dates(dates), m_allCourses(allCourses) {}

    bool isSatisfied(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const override {
        const Course& currentDomainCourse = course.getCourse();
        Date candidateDate = m_dates[dateIndex];

        // 1. Check if the current course has any OBLIGATORY requirement.
        // If it is only an elective course everywhere, this rule does not apply.
        bool isObligatorySomewhere = false;
        for (const auto& prog : currentDomainCourse.getPrograms()) {
            if (prog.requirement == Requirement::OBLIGATORY) {
                isObligatorySomewhere = true;
                break;
            }
        }
        
        if (!isObligatorySomewhere) return true;

        // 2. Iterate through all currently assigned courses in the state
        for (size_t i = 0; i < state.assignedDate.size(); ++i) {
            int assignedDateIdx = state.assignedDate[i];
            if (assignedDateIdx == -1) continue; // Course not assigned yet

            const Course& assignedDomain = m_allCourses[i].getCourse();

            // 3. Find intersections: Same program, same year, and BOTH are obligatory
            for (const auto& currentProg : currentDomainCourse.getPrograms()) {
                if (currentProg.requirement != Requirement::OBLIGATORY) continue;

                for (const auto& assignedProg : assignedDomain.getPrograms()) {
                    if (assignedProg.requirement != Requirement::OBLIGATORY) continue;

                    // If they belong to the same program and year
                    if (currentProg.programID == assignedProg.programID && currentProg.year == assignedProg.year) {
                        
                        // Calculate absolute day difference
                        int daysDiff = candidateDate.daysTo(m_dates[assignedDateIdx]);
                        
                        // If the interval is strictly less than k, the constraint is violated
                        if (daysDiff < m_k) {
                            return false; 
                        }
                    }
                }
            }
        }
        return true;
    }
};

// Rule 2.2: Minimum days between ANY exams (Obligatory or Elective)
class MinDaysAllRule : public IConflictRule {
private:
    int m_k;
    const std::vector<Date>& m_dates;
    const std::vector<RuntimeCourse>& m_allCourses;

public:
    MinDaysAllRule(int k, const std::vector<Date>& dates, const std::vector<RuntimeCourse>& allCourses)
        : m_k(k), m_dates(dates), m_allCourses(allCourses) {}

    bool isSatisfied(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const override {
        const Course& currentDomainCourse = course.getCourse();
        Date candidateDate = m_dates[dateIndex];

        // Iterate through all currently assigned courses
        for (size_t i = 0; i < state.assignedDate.size(); ++i) {
            int assignedDateIdx = state.assignedDate[i];
            if (assignedDateIdx == -1) continue;

            const Course& assignedDomain = m_allCourses[i].getCourse();

            // Check for any matching program and year, ignoring the requirement type
            for (const auto& currentProg : currentDomainCourse.getPrograms()) {
                for (const auto& assignedProg : assignedDomain.getPrograms()) {
                    
                    if (currentProg.programID == assignedProg.programID && currentProg.year == assignedProg.year) {
                        
                        int daysDiff = candidateDate.daysTo(m_dates[assignedDateIdx]);
                        
                        if (daysDiff < m_k) {
                            return false; // Minimum days interval violated
                        }
                    }
                }
            }
        }
        return true;
    }
};

// Rule 2.3: Maximum conflicts (same day) for elective courses
class MaxElectiveConflictsRule : public IConflictRule {
private:
    int m_k;
    const std::vector<RuntimeCourse>& m_allCourses;

public:
    MaxElectiveConflictsRule(int k, const std::vector<RuntimeCourse>& allCourses)
        : m_k(k), m_allCourses(allCourses) {}

    bool isSatisfied(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const override {
        const Course& currentDomainCourse = course.getCourse();

        // 1. Extract all program IDs where this course is an elective
        std::vector<std::string> electivePrograms;
        for (const auto& prog : currentDomainCourse.getPrograms()) {
            if (prog.requirement == Requirement::ELECTIVE) {
                electivePrograms.push_back(prog.programID);
            }
        }
        
        // If it's not an elective in any program, the rule is automatically satisfied
        if (electivePrograms.empty()) return true;

        // 2. For each relevant program, count how many elective exams are already scheduled on this exact date
        for (const std::string& targetProgram : electivePrograms) {
            int sameDayElectiveCount = 0;

            for (size_t i = 0; i < state.assignedDate.size(); ++i) {
                // Only evaluate courses assigned to the candidate date
                if (state.assignedDate[i] != dateIndex) continue; 

                const Course& assignedDomain = m_allCourses[i].getCourse();
                
                // Check if the assigned course is also an elective for the target program
                for (const auto& p : assignedDomain.getPrograms()) {
                    if (p.programID == targetProgram && p.requirement == Requirement::ELECTIVE) {
                        sameDayElectiveCount++;
                        break;
                    }
                }
            }

            // If adding this course exceeds the allowed elective conflicts threshold, prune the branch
            if (sameDayElectiveCount >= m_k) {
                return false; 
            }
        }
        return true;
    }
};

// Rule 2.4: Minimum span (days) between the FIRST and LAST obligatory exams
ObligatorySpanRule::ObligatorySpanRule(int k, const std::vector<Date>& dates, const std::vector<RuntimeCourse>& allCourses)
    : m_k(k), m_dates(dates), m_allCourses(allCourses) {}

bool ObligatorySpanRule::isSatisfied(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const {
    const Course& currentDomainCourse = course.getCourse();

    // Iterate through all programs where the current course is obligatory.
    for (const auto& currentProg : currentDomainCourse.getPrograms()) {
        if (currentProg.requirement != Requirement::OBLIGATORY) continue;

        int totalObligatoryInProgram = 0;
        int assignedObligatoryInProgram = 1; // Start at 1 to include the current candidate course.
        
        int earliestDateIdx = dateIndex;
        int latestDateIdx = dateIndex;

        // Scan all courses in the system to calculate totals and find boundary dates.
        for (size_t i = 0; i < m_allCourses.size(); ++i) {
            const Course& checkDomain = m_allCourses[i].getCourse();
            
            // Check if this course belongs to the same program and year as an obligatory course.
            bool isRelevant = false;
            for (const auto& p : checkDomain.getPrograms()) {
                if (p.programID == currentProg.programID && p.year == currentProg.year && p.requirement == Requirement::OBLIGATORY) {
                    isRelevant = true;
                    break;
                }
            }

            if (isRelevant) {
                totalObligatoryInProgram++; 

                // If it is already assigned in the state (and is not the current candidate course).
                if (state.assignedDate[i] != -1 && &m_allCourses[i] != &course) {
                    assignedObligatoryInProgram++;
                    
                    int assignedDate = state.assignedDate[i];
                    if (assignedDate < earliestDateIdx) earliestDateIdx = assignedDate;
                    if (assignedDate > latestDateIdx) latestDateIdx = assignedDate;
                }
            }
        }

        // If all obligatory exams for this program/year are now scheduled (this is the last one).
        if (assignedObligatoryInProgram == totalObligatoryInProgram) {
            // Calculate the total span of days from the first to the last exam.
            int spanDays = m_dates[latestDateIdx].daysTo(m_dates[earliestDateIdx]);
            
            if (spanDays < m_k) {
                return false; // The span is too small, constraint violated.
            }
        }
    }

    // If not all obligatory exams are scheduled yet, or the condition is met, return true.
    return true; 
}

// Rule 2.5: Maximum total exams per day
class MaxExamsPerDayRule : public IConflictRule {
private:
    int m_k;

public:
    MaxExamsPerDayRule(int k) : m_k(k) {}

    bool isSatisfied(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const override {
        int examsOnThisDate = 0;
        
        // Count how many courses in the current state are assigned to this specific date index
        for (int assignedIdx : state.assignedDate) {
            if (assignedIdx == dateIndex) {
                examsOnThisDate++;
            }
        }
        
        // If the date is already at maximum capacity, the new course cannot be added
        if (examsOnThisDate >= m_k) {
            return false; 
        }
        
        return true;
    }
};