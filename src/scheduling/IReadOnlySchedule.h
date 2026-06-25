#pragma once

/**
 * @brief Abstract interface for reading schedule state.
 * * This interface abstracts the underlying data structure, allowing conflict rules
 * to validate both the algorithm's fast internal state (SchedulingState) 
 * and the UI's output objects (ScheduleGenerationResult).
 */
class IReadOnlySchedule {
public:
    virtual ~IReadOnlySchedule() = default;

    /*
     * Returns the assigned date index for a given course index.
     * Must return -1 if the course has not been assigned yet.
     */
    virtual int getAssignedDate(int courseIndex) const = 0;

    /*
     * Returns the number of obligatory exams for a specific academic group on a specific date.
     */
    virtual int getObligatoryCount(int groupIndex, int dateIndex) const = 0;

    /*
     * Returns the number of elective exams for a specific academic group on a specific date.
     */
    virtual int getElectiveCount(int groupIndex, int dateIndex) const = 0;

    /*
     * Returns the total number of exams assigned to a specific date.
     */
    virtual int getTotalExamsOnDate(int dateIndex) const = 0;
};
