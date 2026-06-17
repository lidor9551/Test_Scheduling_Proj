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

    std::vector<Course> courses = parser.parseCourses(basePath + "courses.txt");
    std::vector<ExamPeriod> periods = parser.parseExamPeriods(basePath + "periods.txt");
    std::vector<std::string> selectedPrograms = parser.parseSelectedPrograms(basePath + "selected_programs.txt");

    SchedulingPreprocessor preprocessor(courses, periods, selectedPrograms);
    std::vector<SchedulingBlock> blocks = preprocessor.buildBlocks();

    EXPECT_EQ(blocks.size(), static_cast<std::size_t>(1));

    const SchedulingBlock& block = blocks[0];

    EXPECT_TRUE(!block.allowedDates.empty());
    EXPECT_EQ(block.runtimeCourses.size(), static_cast<std::size_t>(3));
    
    ScheduleSettings emptySettings; 
    ScheduleGenerator generator(block, emptySettings, 5.0);

    std::vector<ScheduleGenerationResult> solutions = generator.runBacktracking(1);

    EXPECT_EQ(solutions.size(), static_cast<std::size_t>(1));

    const auto& assignments = solutions[0].getAssignments();
    
    EXPECT_EQ(assignments.size(), block.runtimeCourses.size());

    for (const auto& assignment : assignments) {
        EXPECT_TRUE(assignment.course != nullptr);
        
        bool found = false;
        for (const auto& allowedDate : block.allowedDates) {
            if (assignment.examDate == allowedDate) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found);
    }

    std::cout << "SchedulerIntegrationTest passed." << std::endl;
    return EXIT_SUCCESS;
}