#pragma once

#include <vector>
#include <string>
#include "scheduling/IReadOnlySchedule.h"
#include "scheduling/Preprocessor.h"
#include "domain/ScheduleGenerationResult.h"
#include "domain/Date.h"

class DragAndDropAdapter : public IReadOnlySchedule {
public:
    DragAndDropAdapter(const ScheduleGenerationResult& result, 
                       const std::vector<Date>& allowedDates,
                       const std::vector<SchedulingBlock>& blocks,
                       const std::string& ignoreCourseId = "");

    int getObligatoryCount(int groupId, int dateIndex) const override;
    int getElectiveCount(int groupId, int dateIndex) const override;
    int getAssignedDate(int courseId) const;


private:
    const ScheduleGenerationResult& m_result;
    const std::vector<Date>& m_allowedDates;
    const std::vector<SchedulingBlock>& m_blocks;
    std::string m_ignoreCourseId;
};