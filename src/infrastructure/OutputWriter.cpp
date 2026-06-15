#include "OutputWriter.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>

/*
 * Writes all combined schedules to a human-readable text file.
 *
 * Each scheduling block may have multiple solutions.
 * This method combines solutions across blocks and writes each full combined
 * schedule as one numbered schedule in the output file.
 */
void OutputWriter::writeCombinedSchedules(
    const std::string& outputPath,
    const std::vector<SchedulingBlock>& blocks,
    const std::vector<std::vector<std::vector<int>>>& blockSolutions,
    int combinedLimit
) const {
    /*
     * Open the output file.
     *
     * If the file cannot be opened, the caller receives an exception.
     */
    std::ofstream output(outputPath);
    if (!output) {
        throw std::runtime_error("Could not open output file: " + outputPath);
    }

    // Calculate total possible combinations across all blocks
    /*
     * Calculate the number of possible combined schedules.
     *
     * If each block has several solutions, the total number of combinations is
     * the product of the number of solutions in all blocks.
     */
    long long totalSchedules = 1;
    for (const auto& block : blockSolutions) {
        totalSchedules *= static_cast<long long>(block.size());
    }

    /*
     * Write a short header before the schedule details.
     */
    output << "All possible exam schedules\n";
    output << "Blocks: " << blocks.size() << "\n";
    output << "Total combined schedules: " << totalSchedules << "\n";
    if (combinedLimit >= 0) {
        output << "Written schedules limit: " << combinedLimit << "\n";
    }
    output << "\n";

    /*
     * If there are no block solutions, write a clear message and stop.
     */
    if (blockSolutions.empty()) {
        output << "No schedules found.\n";
        return;
    }

    /*
     * indices keeps the currently selected solution index for each block.
     *
     * For example, indices[0] is the selected solution for block 0.
     */
    std::vector<std::size_t> indices(blockSolutions.size(), 0);

    /*
     * Counts how many combined schedules were written.
     */
    int written = 0;

    // Recursive lambda to generate combinations from different semester blocks
    /*
     * Recursively iterates over all combinations of solutions across blocks.
     *
     * The recursion level represents the current block index.
     * When level reaches blockSolutions.size(), one full combined schedule
     * has been selected and can be printed.
     */
    std::function<bool(std::size_t)> writeRecursively = [&](std::size_t level) -> bool {
        if (level == blockSolutions.size()) {
            written++;
            output << "================ Schedule #" << written << " ================\n";

            // Temporary struct for sorting the output chronologically
            /*
             * PrintedExam is a local helper used only for formatting.
             *
             * It flattens all selected block solutions into one list that can
             * be sorted before writing to the file.
             */
            struct PrintedExam {
                std::string semester;
                std::string moed;
                Date examDate;
                const Course* course;
            };

            /*
             * Collect all exams that belong to the current combined schedule.
             */
            std::vector<PrintedExam> exams;
            for (std::size_t blockIndex = 0; blockIndex < blocks.size(); ++blockIndex) {
                const SchedulingBlock& block = blocks[blockIndex];
                const std::vector<int>& solution = blockSolutions[blockIndex][indices[blockIndex]];
                
                /*
                 * Convert each runtime course assignment from a raw date index
                 * into a printable exam record.
                 */
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
            /*
             * Sort the printable exams in a stable, readable order.
             *
             * The output is grouped first by semester and moed,
             * then ordered by date and course number.
             */
            std::sort(exams.begin(), exams.end(), [](const PrintedExam& left, const PrintedExam& right) {
                if (left.semester != right.semester) return left.semester < right.semester;
                if (left.moed != right.moed) return left.moed < right.moed;
                if (left.examDate != right.examDate) return left.examDate < right.examDate;
                return left.course->getCourseNumber() < right.course->getCourseNumber();
            });

            // Write formatted output
            /*
             * Track the last printed semester and moed so the output can add
             * a new section header whenever the group changes.
             */
            std::string lastSemester;
            std::string lastMoed;

            /*
             * Write each exam row.
             */
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

            /*
             * Return true when the requested output limit was reached.
             * This tells the recursive caller to stop writing more schedules.
             */
            return combinedLimit >= 0 && written >= combinedLimit;
        }

        /*
         * Try every solution of the current block and continue recursively
         * to the next block.
         */
        for (std::size_t i = 0; i < blockSolutions[level].size(); ++i) {
            indices[level] = i;
            if (writeRecursively(level + 1)) {
                return true;
            }
        }

        return false;
    };

    /*
     * Start recursive writing from the first block.
     */
    writeRecursively(0);
}