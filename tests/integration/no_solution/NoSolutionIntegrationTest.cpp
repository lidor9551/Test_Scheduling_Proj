#include "infrastructure/InputParser.h"
#include "scheduling/Preprocessor.h"
#include "scheduling/ScheduleGenerator.h"
#include "domain/ScheduleGenerationResult.h" 

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
    const std::string basePath = "../tests/data/no_solution/impossible-schedule/";

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

    std::vector<ScheduleGenerationResult> solutions = generator.runBacktracking(1);

    EXPECT_TRUE(solutions.empty());

    std::cout << "NoSolutionIntegrationTest passed." << std::endl;
    return EXIT_SUCCESS;
}