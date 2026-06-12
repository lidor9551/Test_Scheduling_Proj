#include "OutputWriter.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>

void OutputWriter::writeCombinedSchedules(
    const std::string& outputPath,
    const std::vector<SchedulingBlock>& blocks,
    const std::vector<std::vector<std::vector<int>>>& blockSolutions,
    int combinedLimit
) const {
    std::ofstream output(outputPath);
    if (!output) {
        throw std::runtime_error("Could not open output file: " + outputPath);
    }

    // Calculate total possible combinations across all blocks
    long long totalSchedules = 1;
    for (const auto& block : blockSolutions) {
        totalSchedules *= static_cast<long long>(block.size());
    }

    output << "All possible exam schedules\n";
    output << "Blocks: " << blocks.size() << "\n";
    output << "Total combined schedules: " << totalSchedules << "\n";
    if (combinedLimit >= 0) {
        output << "Written schedules limit: " << combinedLimit << "\n";
    }
    output << "\n";

    if (blockSolutions.empty()) {
        output << "No schedules found.\n";
        return;
    }

    std::vector<std::size_t> indices(blockSolutions.size(), 0);
    int written = 0;

    // Recursive lambda to generate combinations from different semester blocks
    std::function<bool(std::size_t)> writeRecursively = [&](std::size_t level) -> bool {
        if (level == blockSolutions.size()) {
            written++;
            output << "================ Schedule #" << written << " ================\n";

            // Temporary struct for sorting the output chronologically
            struct PrintedExam {
                std::string semester;
                std::string moed;
                Date examDate;
                const Course* course;
            };

            std::vector<PrintedExam> exams;
            for (std::size_t blockIndex = 0; blockIndex < blocks.size(); ++blockIndex) {
                const SchedulingBlock& block = blocks[blockIndex];
                const std::vector<int>& solution = blockSolutions[blockIndex][indices[blockIndex]];
                
                for (const RuntimeCourse& runtimeCourse : block.runtimeCourses) {
                    int dateIndex = solution[runtimeCourse.id];
                    exams.push_back(PrintedExam{
                        block.semester,
                        block.moed,
                        block.allowedDates[dateIndex],
                        runtimeCourse.course
                    });
                }
            }

            // Sort by Semester -> Moed -> Date -> Course Number
            std::sort(exams.begin(), exams.end(), [](const PrintedExam& left, const PrintedExam& right) {
                if (left.semester != right.semester) return left.semester < right.semester;
                if (left.moed != right.moed) return left.moed < right.moed;
                if (left.examDate != right.examDate) return left.examDate < right.examDate;
                return left.course->getCourseNumber() < right.course->getCourseNumber();
            });

            // Write formatted output
            std::string lastSemester;
            std::string lastMoed;
            for (const PrintedExam& exam : exams) {
                if (exam.semester != lastSemester || exam.moed != lastMoed) {
                    output << "\n--- " << exam.semester << " | " << exam.moed << " ---\n";
                    lastSemester = exam.semester;
                    lastMoed = exam.moed;
                }

                output << exam.examDate.toString()
                       << " | " << exam.course->getCourseNumber()
                       << " | " << exam.course->getCourseName()
                       << " | " << exam.course->getInstructorName()
                       << "\n";
            }

            output << "\n";
            return combinedLimit >= 0 && written >= combinedLimit;
        }

        for (std::size_t i = 0; i < blockSolutions[level].size(); ++i) {
            indices[level] = i;
            if (writeRecursively(level + 1)) {
                return true;
            }
        }

        return false;
    };

    writeRecursively(0);
}