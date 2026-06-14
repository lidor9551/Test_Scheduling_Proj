#include "ScheduleGenerator.h"
#include "scheduling/SchedulingWorker.h"
#include <algorithm>
#include <chrono>
#include <functional>
#include <limits>

SolverTimeoutException::SolverTimeoutException(const std::string& message)
    : std::runtime_error(message) {}

ScheduleGenerator::ScheduleGenerator(SchedulingBlock block, double maxRuntimeSeconds)
    : block_(std::move(block)),
      groupCount_(computeGroupCount()),
      maxRuntimeSeconds_(maxRuntimeSeconds) {
    qRegisterMetaType<std::vector<ScheduleGenerationResult>>("std::vector<ScheduleGenerationResult>");
}

std::vector<ScheduleGenerationResult> ScheduleGenerator::runBacktracking(int limitPerBlock) const {
    return generateAll(limitPerBlock);
}

int ScheduleGenerator::computeGroupCount() const {
    int maxGroup = -1;
    for (const RuntimeCourse& course : block_.runtimeCourses) {
        for (const CourseMembership& membership : course.memberships) {
            maxGroup = std::max(maxGroup, membership.groupId);
        }
    }
    return maxGroup + 1;
}

bool ScheduleGenerator::canAssign(const SchedulerState& state, const RuntimeCourse& course, int dateIndex) const {
    for (const CourseMembership& membership : course.memberships) {
        int groupId = membership.groupId;
        
        // Using the strict Requirement enum from the OOP Model
        if (membership.requirement == Requirement::OBLIGATORY) {
            if (state.obligatoryCount[groupId][dateIndex] > 0) {
                return false;
            }
            if (state.electiveCount[groupId][dateIndex] > 0) {
                return false;
            }
        } else {
            if (state.obligatoryCount[groupId][dateIndex] > 0) {
                return false;
            }
        }
    }
    return true;
}

void ScheduleGenerator::assign(SchedulerState& state, const RuntimeCourse& course, int dateIndex) const {
    state.assignedDate[course.id] = dateIndex;
    for (const CourseMembership& membership : course.memberships) {
        if (membership.requirement == Requirement::OBLIGATORY) {
            state.obligatoryCount[membership.groupId][dateIndex]++;
        } else {
            state.electiveCount[membership.groupId][dateIndex]++;
        }
    }
}

void ScheduleGenerator::unassign(SchedulerState& state, const RuntimeCourse& course, int dateIndex) const {
    state.assignedDate[course.id] = -1;
    for (const CourseMembership& membership : course.memberships) {
        if (membership.requirement == Requirement::OBLIGATORY) {
            state.obligatoryCount[membership.groupId][dateIndex]--;
        } else {
            state.electiveCount[membership.groupId][dateIndex]--;
        }
    }
}

int ScheduleGenerator::datePressure(const SchedulerState& state, const RuntimeCourse& course, int dateIndex) const {
    int pressure = 0;
    for (const CourseMembership& membership : course.memberships) {
        pressure += state.obligatoryCount[membership.groupId][dateIndex];
        pressure += state.electiveCount[membership.groupId][dateIndex];
    }
    return pressure;
}

std::vector<int> ScheduleGenerator::orderedCandidateDates(const SchedulerState& state, const RuntimeCourse& course) const {
    std::vector<int> candidates;
    for (int dateIndex = 0; dateIndex < static_cast<int>(block_.allowedDates.size()); ++dateIndex) {
        if (canAssign(state, course, dateIndex)) {
            candidates.push_back(dateIndex);
        }
    }

    std::sort(candidates.begin(), candidates.end(), [&](int left, int right) {
        int leftPressure = datePressure(state, course, left);
        int rightPressure = datePressure(state, course, right);
        if (leftPressure != rightPressure) {
            return leftPressure < rightPressure;
        }
        return block_.allowedDates[left] < block_.allowedDates[right];
    });

    return candidates;
}

int ScheduleGenerator::selectNextCourse(const SchedulerState& state, const std::vector<bool>& remaining) const {
    int bestIndex = -1;
    int bestDomainSize = std::numeric_limits<int>::max();
    int bestHasOnlyElective = std::numeric_limits<int>::max();
    int bestMembershipScore = std::numeric_limits<int>::min();
    std::string bestCourseNumber;

    for (int i = 0; i < static_cast<int>(remaining.size()); ++i) {
        if (!remaining[i]) {
            continue;
        }

        const RuntimeCourse& course = block_.runtimeCourses[i];
        int domainSize = 0;
        for (int dateIndex = 0; dateIndex < static_cast<int>(block_.allowedDates.size()); ++dateIndex) {
            if (canAssign(state, course, dateIndex)) {
                domainSize++;
            }
        }

        bool hasObligatory = false;
        for (const CourseMembership& membership : course.memberships) {
            if (membership.requirement == Requirement::OBLIGATORY) {
                hasObligatory = true;
                break;
            }
        }

        int onlyElectiveFlag = hasObligatory ? 0 : 1;
        int membershipScore = -static_cast<int>(course.memberships.size());
        
        // Using the new getter from the OOP Model
        const std::string& number = course.course->getCourseNumber();

        bool better = false;
        if (domainSize < bestDomainSize) {
            better = true;
        } else if (domainSize == bestDomainSize && onlyElectiveFlag < bestHasOnlyElective) {
            better = true;
        } else if (domainSize == bestDomainSize &&
                   onlyElectiveFlag == bestHasOnlyElective &&
                   membershipScore < bestMembershipScore) {
            better = true;
        } else if (domainSize == bestDomainSize &&
                   onlyElectiveFlag == bestHasOnlyElective &&
                   membershipScore == bestMembershipScore &&
                   (bestIndex == -1 || number < bestCourseNumber)) {
            better = true;
        }

        if (better) {
            bestIndex = i;
            bestDomainSize = domainSize;
            bestHasOnlyElective = onlyElectiveFlag;
            bestMembershipScore = membershipScore;
            bestCourseNumber = number;
        }
    }

    return bestIndex;
}

std::vector<ScheduleGenerationResult> ScheduleGenerator::generateAll(int limitPerBlock) const {
    SchedulerState state{
        std::vector<int>(block_.runtimeCourses.size(), -1),
        std::vector<std::vector<int>>(groupCount_, std::vector<int>(block_.allowedDates.size(), 0)),
        std::vector<std::vector<int>>(groupCount_, std::vector<int>(block_.allowedDates.size(), 0))
    };

    std::vector<bool> remaining(block_.runtimeCourses.size(), true);
    std::vector<ScheduleGenerationResult> solutions;

    const auto startTime = std::chrono::steady_clock::now();

    // Define the internal recursive backtracking function
    std::function<void()> backtrack = [&]() {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - startTime).count();
        
        // Stop if we exceeded the time limit (e.g., 30 seconds)
        if (elapsed > maxRuntimeSeconds_) {
            throw SolverTimeoutException(
                "Solver exceeded " + std::to_string(maxRuntimeSeconds_) +
                " seconds for block " + block_.semester + "/" + block_.moed
            );
        }

        if (limitPerBlock >= 0 && static_cast<int>(solutions.size()) >= limitPerBlock) {
            return;
        }

        bool done = true;
        for (bool flag : remaining) {
            if (flag) {
                done = false;
                break;
            }
        }

        if (done) {
            // We've found a complete solution. Let's convert it to the new ScheduleGenerationResult format.
            std::vector<ExamAssignment> assignments;
            
            for (size_t i = 0; i < state.assignedDate.size(); ++i) {
                int dateIndex = state.assignedDate[i];
                if (dateIndex == -1) continue; 

                const RuntimeCourse& runtimeCourse = block_.runtimeCourses[i];
                
                // Determine if the course is obligatory or elective based on its memberships
                bool isObligatory = false;
                for (const CourseMembership& membership : runtimeCourse.memberships) {
                    if (membership.requirement == Requirement::OBLIGATORY) {
                        isObligatory = true;
                        break;
                    }
                }

                // Create an ExamAssignment for this course and add it to the list of assignments for this solution
                assignments.push_back({
                    runtimeCourse.course,                // Pointer to the original Course object
                    block_.allowedDates[dateIndex],      // The exact date
                    isObligatory                         // Obligatory or elective
                });
            }

            // Save the final cleaned result
            solutions.emplace_back(ScheduleGenerationResult(assignments));
            return;
        }

        int nextCourseIndex = selectNextCourse(state, remaining);
        remaining[nextCourseIndex] = false;

        std::vector<int> candidates = orderedCandidateDates(state, block_.runtimeCourses[nextCourseIndex]);
        for (int dateIndex : candidates) {
            assign(state, block_.runtimeCourses[nextCourseIndex], dateIndex);
            backtrack();
            unassign(state, block_.runtimeCourses[nextCourseIndex], dateIndex);

            if (limitPerBlock >= 0 && static_cast<int>(solutions.size()) >= limitPerBlock) {
                break;
            }
        }

        remaining[nextCourseIndex] = true;
    };

    backtrack();
    return solutions;
}
