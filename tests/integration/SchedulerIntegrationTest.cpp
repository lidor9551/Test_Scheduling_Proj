#include "infrastructure/InputParser.h"
#include "scheduling/Preprocessor.h"
#include "scheduling/ScheduleGenerator.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#define EXPECT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAILED: " << #condition \
                      << " at line " << __LINE__ << std::endl; \
            return EXIT_FAILURE; \
        } \
    } while (false)

#define EXPECT_EQ(actual, expected) \
    do { \
        if (!((actual) == (expected))) { \
            std::cerr << "FAILED: " << #actual << " == " << #expected \
                      << " at line " << __LINE__ << std::endl; \
            return EXIT_FAILURE; \
        } \
    } while (false)

int main() {
    const std::string basePath = "../tests/data/valid/simple/";

    InputParser parser;

    std::vector<Course> courses =
        parser.parseCourses(basePath + "courses.txt");

    std::vector<ExamPeriod> periods =
        parser.parseExamPeriods(basePath + "periods.txt");

    std::vector<std::string> selectedPrograms =
        parser.parseSelectedPrograms(basePath + "selected_programs.txt");

    SchedulingPreprocessor preprocessor(courses, periods, selectedPrograms);
    std::vector<SchedulingBlock> blocks = preprocessor.buildBlocks();

    EXPECT_EQ(blocks.size(), static_cast<std::size_t>(1));

    const SchedulingBlock& block = blocks[0];

    EXPECT_TRUE(!block.allowedDates.empty());
    EXPECT_EQ(block.runtimeCourses.size(), static_cast<std::size_t>(3));

    ScheduleGenerator generator(block, 5.0);

    std::vector<std::vector<int>> solutions = generator.runBacktracking(1);

    EXPECT_EQ(solutions.size(), static_cast<std::size_t>(1));

    const std::vector<int>& firstSolution = solutions[0];

    EXPECT_EQ(firstSolution.size(), block.runtimeCourses.size());

    for (int dateIndex : firstSolution) {
        EXPECT_TRUE(dateIndex >= 0);
        EXPECT_TRUE(dateIndex < static_cast<int>(block.allowedDates.size()));
    }

    std::cout << "SchedulerIntegrationTest passed." << std::endl;
    return EXIT_SUCCESS;
}