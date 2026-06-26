#pragma once

#include <string>
#include <vector>

#include "domain/Date.h"
#include "domain/ScheduleGenerationResult.h"
#include "scheduling/Preprocessor.h"
#include "application/SchedulingSession.h"

struct MoveValidationResult {
    bool allowed = false;
    std::string reason;
    std::string violatedRuleName;
};

class MoveValidationService {
public:
    MoveValidationResult validateMove(
        const ScheduleGenerationResult& tempScheduleCopy,
        const std::string& courseId,
        const Date& targetDate,
        const std::vector<Date>& allowedDates,
        const std::vector<SchedulingBlock>& blocks,
        const ScheduleSettings& settings
    ) const;
};