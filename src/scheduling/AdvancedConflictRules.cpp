#include "AdvancedConflictRules.h"

// ============================================================================
// Rule 2.1: Minimum days between obligatory exams
// ============================================================================
MinDaysObligatoryRule::MinDaysObligatoryRule(int k, const std::vector<Date>& dates, const std::vector<RuntimeCourse>& allCourses)
    : m_k(k), m_dates(dates), m_allCourses(allCourses) {}

bool MinDaysObligatoryRule::isSatisfied(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const {
    const Course* pCourse = course.course;
    const Course& currentDomainCourse = *pCourse;
    Date candidateDate = m_dates[dateIndex];

    bool isObligatorySomewhere = false;
    for (const auto& prog : currentDomainCourse.getPrograms()) {
        if (prog.requirement == Requirement::OBLIGATORY) {
            isObligatorySomewhere = true;
            break;
        }
    }
    
    if (!isObligatorySomewhere) return true;

    for (size_t i = 0; i < state.assignedDate.size(); ++i) {
        int assignedDateIdx = state.assignedDate[i];
        if (assignedDateIdx == -1) continue;

        const Course* pAssigned = m_allCourses[i].course;
        const Course& assignedDomain = *pAssigned;

        for (const auto& currentProg : currentDomainCourse.getPrograms()) {
            if (currentProg.requirement != Requirement::OBLIGATORY) continue;

            for (const auto& assignedProg : assignedDomain.getPrograms()) {
                if (assignedProg.requirement != Requirement::OBLIGATORY) continue;

                if (currentProg.programID == assignedProg.programID && currentProg.year == assignedProg.year) {
                    int daysDiff = candidateDate.daysTo(m_dates[assignedDateIdx]);
                    
                    if (daysDiff < m_k) {
                        return false; 
                    }
                }
            }
        }
    }
    return true;
}

// ============================================================================
// Rule 2.2: Minimum days between ANY exams (Obligatory or Elective)
// ============================================================================
MinDaysAllRule::MinDaysAllRule(int k, const std::vector<Date>& dates, const std::vector<RuntimeCourse>& allCourses)
    : m_k(k), m_dates(dates), m_allCourses(allCourses) {}

bool MinDaysAllRule::isSatisfied(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const {
    const Course* pCourse = course.course;
    const Course& currentDomainCourse = *pCourse;
    Date candidateDate = m_dates[dateIndex];

    for (size_t i = 0; i < state.assignedDate.size(); ++i) {
        int assignedDateIdx = state.assignedDate[i];
        if (assignedDateIdx == -1) continue;

        const Course* pAssigned = m_allCourses[i].course;
        const Course& assignedDomain = *pAssigned;

        for (const auto& currentProg : currentDomainCourse.getPrograms()) {
            for (const auto& assignedProg : assignedDomain.getPrograms()) {
                if (currentProg.programID == assignedProg.programID && currentProg.year == assignedProg.year) {
                    int daysDiff = candidateDate.daysTo(m_dates[assignedDateIdx]);
                    if (daysDiff < m_k) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

// ============================================================================
// Rule 2.3: Maximum conflicts (same day) for elective courses
// ============================================================================
MaxElectiveConflictsRule::MaxElectiveConflictsRule(int k, const std::vector<RuntimeCourse>& allCourses)
    : m_k(k), m_allCourses(allCourses) {}

bool MaxElectiveConflictsRule::isSatisfied(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const {
    const Course* pCourse = course.course;
    const Course& currentDomainCourse = *pCourse;

    std::vector<std::string> electivePrograms;
    for (const auto& prog : currentDomainCourse.getPrograms()) {
        if (prog.requirement == Requirement::ELECTIVE) {
            electivePrograms.push_back(prog.programID);
        }
    }
    
    if (electivePrograms.empty()) return true;

    for (const std::string& targetProgram : electivePrograms) {
        int sameDayElectiveCount = 0;

        for (size_t i = 0; i < state.assignedDate.size(); ++i) {
            if (state.assignedDate[i] != dateIndex) continue; 

            const Course* pAssigned = m_allCourses[i].course;
            const Course& assignedDomain = *pAssigned;
            
            for (const auto& p : assignedDomain.getPrograms()) {
                if (p.programID == targetProgram && p.requirement == Requirement::ELECTIVE) {
                    sameDayElectiveCount++;
                    break;
                }
            }
        }

        if (sameDayElectiveCount >= m_k) {
            return false; 
        }
    }
    return true;
}

// ============================================================================
// Rule 2.4: Minimum span (days) between the FIRST and LAST obligatory exams
// ============================================================================
ObligatorySpanRule::ObligatorySpanRule(int k, const std::vector<Date>& dates, const std::vector<RuntimeCourse>& allCourses)
    : m_k(k), m_dates(dates), m_allCourses(allCourses) {}

bool ObligatorySpanRule::isSatisfied(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const {
    const Course* pCourse = course.course;
    const Course& currentDomainCourse = *pCourse;

    for (const auto& currentProg : currentDomainCourse.getPrograms()) {
        if (currentProg.requirement != Requirement::OBLIGATORY) continue;

        int totalObligatoryInProgram = 0;
        int assignedObligatoryInProgram = 1; 
        
        int earliestDateIdx = dateIndex;
        int latestDateIdx = dateIndex;

        for (size_t i = 0; i < m_allCourses.size(); ++i) {
            const Course* pCheck = m_allCourses[i].course;
            const Course& checkDomain = *pCheck;
            
            bool isRelevant = false;
            for (const auto& p : checkDomain.getPrograms()) {
                if (p.programID == currentProg.programID && p.year == currentProg.year && p.requirement == Requirement::OBLIGATORY) {
                    isRelevant = true;
                    break;
                }
            }

            if (isRelevant) {
                totalObligatoryInProgram++; 

                if (state.assignedDate[i] != -1 && &m_allCourses[i] != &course) {
                    assignedObligatoryInProgram++;
                    
                    int assignedDate = state.assignedDate[i];
                    if (assignedDate < earliestDateIdx) earliestDateIdx = assignedDate;
                    if (assignedDate > latestDateIdx) latestDateIdx = assignedDate;
                }
            }
        }

        if (assignedObligatoryInProgram == totalObligatoryInProgram) {
            int spanDays = m_dates[latestDateIdx].daysTo(m_dates[earliestDateIdx]);
            
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

bool MaxExamsPerDayRule::isSatisfied(const SchedulingState& state, const RuntimeCourse& course, int dateIndex) const {
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