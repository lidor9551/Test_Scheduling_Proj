#include "DragAndDropAdapter.h"
#include "scheduling/Preprocessor.h"

// Constructor initializes the adapter with the schedule, dates, and the preprocessor blocks
DragAndDropAdapter::DragAndDropAdapter(const ScheduleGenerationResult& result, 
                                       const std::vector<Date>& allowedDates,
                                       const std::vector<SchedulingBlock>& blocks,
                                       const std::string& ignoreCourseId)
    : m_result(result), m_allowedDates(allowedDates), m_blocks(blocks), m_ignoreCourseId(ignoreCourseId) {}

int DragAndDropAdapter::getObligatoryCount(int groupId, int dateIndex) const {
    int count = 0;
    
    // Safety check to prevent array out-of-bounds
    if (dateIndex < 0 || dateIndex >= static_cast<int>(m_allowedDates.size())) {
        return 0;
    }
    
    std::string targetDateStr = m_allowedDates[dateIndex].toString();

    // Iterate through all assigned exams in the UI's temporary schedule
    for (const auto& assignment : m_result.getAssignments()) {
        
        // Check if the assignment is placed on the date we are inspecting
        if (assignment.examDate.toString() == targetDateStr && assignment.course) {
            std::string assignedCourseNum = assignment.course->getCourseNumber();

            if (!m_ignoreCourseId.empty() && assignedCourseNum == m_ignoreCourseId) {
                continue; 
            }

            // We must find this specific course inside the preprocessor blocks 
            // to discover which integer 'groupId's are actually taking it.
            for (const auto& block : m_blocks) {
                for (const auto& rc : block.runtimeCourses) {
                    
                    // Match the runtime course to the domain course by course number
                    if (rc.course && rc.course->getCourseNumber() == assignedCourseNum) {
                        
                        // Check if the requested groupId is taking this course as OBLIGATORY
                        for (const auto& membership : rc.memberships) {
                            if (membership.groupId == groupId && membership.requirement == Requirement::OBLIGATORY) {
                                count++;
                                // Group found for this course, break early to avoid double-counting
                                break; 
                            }
                        }
                    }
                }
            }
        }
    }
    return count;
}

int DragAndDropAdapter::getElectiveCount(int groupId, int dateIndex) const {
    int count = 0;
    
    // Safety check to prevent array out-of-bounds
    if (dateIndex < 0 || dateIndex >= static_cast<int>(m_allowedDates.size())) {
        return 0;
    }
    
    std::string targetDateStr = m_allowedDates[dateIndex].toString();

    // Iterate through all assigned exams in the UI's temporary schedule
    for (const auto& assignment : m_result.getAssignments()) {
        
        // Check if the assignment is placed on the date we are inspecting
        if (assignment.examDate.toString() == targetDateStr && assignment.course) {
            std::string assignedCourseNum = assignment.course->getCourseNumber();

            if (!m_ignoreCourseId.empty() && assignedCourseNum == m_ignoreCourseId) {
                continue; 
            }

            // Search through the blocks to resolve the group IDs
            for (const auto& block : m_blocks) {
                for (const auto& rc : block.runtimeCourses) {
                    
                    // Match the runtime course
                    if (rc.course && rc.course->getCourseNumber() == assignedCourseNum) {
                        
                        // Check if the requested groupId is taking this course as ELECTIVE
                        for (const auto& membership : rc.memberships) {
                            if (membership.groupId == groupId && membership.requirement == Requirement::ELECTIVE) {
                                count++;
                                // Group found for this course, break early to avoid double-counting
                                break; 
                            }
                        }
                    }
                }
            }
        }
    }
    return count;
}

int DragAndDropAdapter::getAssignedDate(int courseId) const {
    std::string targetCourseNum = "";
    
    // 1. Translate internal engine ID to real Course Number using blocks
    for (const auto& block : m_blocks) {
        for (const auto& rc : block.runtimeCourses) {
            // Assuming the internal ID is stored in rc.id. 
            // (If it's called something else in your code, change 'rc.id' to match)
            if (rc.id == courseId && rc.course) {
                targetCourseNum = rc.course->getCourseNumber();
                break;
            }
        }
        if (!targetCourseNum.empty()) break;
    }

    // Fallback: If no mapping found, maybe courseId WAS the real course number
    if (targetCourseNum.empty()) {
        targetCourseNum = std::to_string(courseId);
    }

    // 2. Find it in the schedule
    for (const auto& assignment : m_result.getAssignments()) {
        if (assignment.course && assignment.course->getCourseNumber() == targetCourseNum) {
            
            // If the rule is asking about the course we are dragging, 
            // we must return -1 so the rule engine treats it as "unassigned" (floating).
            if (!m_ignoreCourseId.empty() && targetCourseNum == m_ignoreCourseId) {
                return -1;
            }
            
            // Found it! Return its date index
            for (std::size_t i = 0; i < m_allowedDates.size(); ++i) {
                if (m_allowedDates[i].toString() == assignment.examDate.toString()) {
                    return static_cast<int>(i);
                }
            }
        }
    }
    return -1; 
}