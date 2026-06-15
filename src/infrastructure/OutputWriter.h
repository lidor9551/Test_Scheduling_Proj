#pragma once

#include "scheduling/Preprocessor.h"
#include <string>
#include <vector>

/*
 * OutputWriter belongs to the infrastructure layer.
 *
 * Its responsibility is to write generated scheduling results to an external
 * text file in a readable format.
 *
 * This class receives scheduling data from the scheduling layer and converts it
 * into plain text output for the user.
 */
class OutputWriter {
public:
    // Writes all valid combined schedules to a text file
    // Takes the original scheduling blocks and the raw array solutions from the generator
    /*
     * Writes all valid combined schedules into a text file.
     *
     * Parameters:
     * - outputPath: destination file path.
     * - blocks: the scheduling blocks that were solved.
     * - blockSolutions: raw solution vectors for each block.
     * - combinedLimit: optional maximum number of combined schedules to write.
     *
     * A negative combinedLimit means no explicit output limit.
     */
    void writeCombinedSchedules(
        const std::string& outputPath,
        const std::vector<SchedulingBlock>& blocks,
        const std::vector<std::vector<std::vector<int>>>& blockSolutions,
        int combinedLimit = -1
    ) const;
};