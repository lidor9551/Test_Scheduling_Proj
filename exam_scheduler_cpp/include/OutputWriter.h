#ifndef OUTPUT_WRITER_H
#define OUTPUT_WRITER_H

#include "Domain.h"
#include <string>
#include <vector>

class OutputWriter {
public:
    void writeCombinedSchedules(
        const std::string& outputPath,
        const std::vector<SchedulingBlock>& blocks,
        const std::vector<std::vector<std::vector<int>>>& blockSolutions,
        int combinedLimit = -1
    ) const;
};

#endif
