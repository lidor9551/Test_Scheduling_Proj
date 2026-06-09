#include "parser/InputParser.h"
#include "preprocessing/Preprocessor.h"

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

    const std::string coursesPath = basePath + "courses.txt";
    const std::string periodsPath = basePath + "periods.txt";
    const std::string selectedProgramsPath = basePath + "selected_programs.txt";

    InputParser parser;

    std::vector<Course> courses = parser.parseCourses(coursesPath);
    std::vector<ExamPeriod> periods = parser.parseExamPeriods(periodsPath);
    std::vector<std::string> selectedPrograms =
        parser.parseSelectedPrograms(selectedProgramsPath);

    EXPECT_EQ(courses.size(), static_cast<std::size_t>(3));
    EXPECT_EQ(periods.size(), static_cast<std::size_t>(1));
    EXPECT_EQ(selectedPrograms.size(), static_cast<std::size_t>(1));
    EXPECT_EQ(selectedPrograms[0], std::string("83101"));

    SchedulingPreprocessor preprocessor(courses, periods, selectedPrograms);
    std::vector<SchedulingBlock> blocks = preprocessor.buildBlocks();

    EXPECT_EQ(blocks.size(), static_cast<std::size_t>(1));

    const SchedulingBlock& block = blocks[0];

    EXPECT_EQ(block.semester, std::string("FALL"));
    EXPECT_EQ(block.moed, std::string("Aleph"));

    EXPECT_TRUE(!block.allowedDates.empty());
    EXPECT_EQ(block.runtimeCourses.size(), static_cast<std::size_t>(3));

    for (const RuntimeCourse& runtimeCourse : block.runtimeCourses) {
        EXPECT_TRUE(runtimeCourse.course != nullptr);
        EXPECT_TRUE(!runtimeCourse.memberships.empty());
    }

    std::cout << "ParserPreprocessorIntegrationTest passed." << std::endl;
    return EXIT_SUCCESS;
}