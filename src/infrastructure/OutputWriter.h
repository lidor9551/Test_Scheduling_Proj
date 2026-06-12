#pragma once

#include "scheduling/Preprocessor.h"
#include <string>
#include <vector>

class OutputWriter {
public:
    // Writes all valid combined schedules to a text file
    // Takes the original scheduling blocks and the raw array solutions from the generator
    void writeCombinedSchedules(
        const std::string& outputPath,
        const std::vector<SchedulingBlock>& blocks,
        const std::vector<std::vector<std::vector<int>>>& blockSolutions,
        int combinedLimit = -1
    ) const;
};