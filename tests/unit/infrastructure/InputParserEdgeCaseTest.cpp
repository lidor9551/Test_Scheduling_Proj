#include "infrastructure/InputParser.h"
#include "TestMacros.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

void writeFile(const std::string& path, const std::string& content) {
    std::ofstream output(path);

    if (!output) {
        TEST_FAIL("FAILED: could not create temporary test file: " << path);
    }

    output << content;
}

void removeFile(const std::string& path) {
    std::remove(path.c_str());
}

template <typename Action>
void expectParseException(Action action, const std::string& scenarioName) {
    try {
        action();

        TEST_FAIL("FAILED: expected ParseException for scenario: " << scenarioName);
    } catch (const ParseException&) {
        // Expected path.
    } catch (const std::exception& ex) {
        TEST_FAIL("FAILED: expected ParseException for scenario: "
                  << scenarioName << ", got std::exception: " << ex.what());
    } catch (...) {
        TEST_FAIL("FAILED: expected ParseException for scenario: "
                  << scenarioName << ", got unknown exception");
    }
}

bool containsProgram(const std::vector<std::string>& programs, const std::string& expected) {
    for (const std::string& program : programs) {
        if (program == expected) {
            return true;
        }
    }

    return false;
}

void testValidatorKnownValues() {
    TEST_EXPECT_TRUE(Validator::isValidProgramId("83101"));
    TEST_EXPECT_TRUE(Validator::isValidProgramId("83108"));
    TEST_EXPECT_FALSE(Validator::isValidProgramId("99999"));

    TEST_EXPECT_TRUE(Validator::isValidSemester("FALL"));
    TEST_EXPECT_TRUE(Validator::isValidSemester("SPRI"));
    TEST_EXPECT_TRUE(Validator::isValidSemester("SUMM"));
    TEST_EXPECT_FALSE(Validator::isValidSemester("SPRING"));

    TEST_EXPECT_TRUE(Validator::isValidMoed("Aleph"));
    TEST_EXPECT_TRUE(Validator::isValidMoed("Bet"));
    TEST_EXPECT_TRUE(Validator::isValidMoed("Gimel"));
    TEST_EXPECT_FALSE(Validator::isValidMoed("MoedA"));
}

void testParseSelectedProgramsTrimsAndDeduplicates() {
    const std::string path = "input_parser_selected_programs_valid.txt";

    writeFile(
        path,
        " 83101, 83102 \n"
        "83101\n"
        "83108\n"
    );

    InputParser parser;
    std::vector<std::string> programs = parser.parseSelectedPrograms(path);

    TEST_EXPECT_EQ(programs.size(), static_cast<std::size_t>(3));
    TEST_EXPECT_TRUE(containsProgram(programs, "83101"));
    TEST_EXPECT_TRUE(containsProgram(programs, "83102"));
    TEST_EXPECT_TRUE(containsProgram(programs, "83108"));

    removeFile(path);
}

void testParseSelectedProgramsRejectsUnknownProgram() {
    const std::string path = "input_parser_selected_programs_invalid.txt";

    writeFile(
        path,
        "83101\n"
        "99999\n"
    );

    InputParser parser;

    expectParseException(
        [&]() {
            parser.parseSelectedPrograms(path);
        },
        "unknown selected program id"
    );

    removeFile(path);
}

void testParseCoursesValidCourseWithMultipleOfferings() {
    const std::string path = "input_parser_courses_valid.txt";

    writeFile(
        path,
        "Algorithms\n"
        "89101\n"
        "Dr. A\n"
        "83101, 1, FALL, Obligatory\n"
        "83102, 2, SPRI, Elective\n"
        "Exam\n"
        "$$$$\n"
    );

    InputParser parser;
    std::vector<Course> courses = parser.parseCourses(path);

    TEST_EXPECT_EQ(courses.size(), static_cast<std::size_t>(1));

    const Course& course = courses[0];

    TEST_EXPECT_EQ(course.getCourseName(), std::string("Algorithms"));
    TEST_EXPECT_EQ(course.getCourseNumber(), std::string("89101"));
    TEST_EXPECT_EQ(course.getInstructorName(), std::string("Dr. A"));
    TEST_EXPECT_EQ(course.getEvaluationMethod(), Evaluation::EXAM);
    TEST_EXPECT_TRUE(course.requiresExam());

    const auto& programs = course.getPrograms();

    TEST_EXPECT_EQ(programs.size(), static_cast<std::size_t>(2));

    TEST_EXPECT_EQ(programs[0].programID, std::string("83101"));
    TEST_EXPECT_EQ(programs[0].year, Year::FIRST);
    TEST_EXPECT_EQ(programs[0].semester, Semester::FALL);
    TEST_EXPECT_EQ(programs[0].requirement, Requirement::OBLIGATORY);

    TEST_EXPECT_EQ(programs[1].programID, std::string("83102"));
    TEST_EXPECT_EQ(programs[1].year, Year::SECOND);
    TEST_EXPECT_EQ(programs[1].semester, Semester::SPRI);
    TEST_EXPECT_EQ(programs[1].requirement, Requirement::ELECTIVE);

    removeFile(path);
}

void testParseCoursesRejectsInvalidCourseNumber() {
    const std::string path = "input_parser_courses_invalid_number.txt";

    writeFile(
        path,
        "Algorithms\n"
        "8910\n"
        "Dr. A\n"
        "83101, 1, FALL, Obligatory\n"
        "Exam\n"
        "$$$$\n"
    );

    InputParser parser;

    expectParseException(
        [&]() {
            parser.parseCourses(path);
        },
        "invalid course number"
    );

    removeFile(path);
}

void testParseCoursesRejectsInvalidEvaluationType() {
    const std::string path = "input_parser_courses_invalid_evaluation.txt";

    writeFile(
        path,
        "Algorithms\n"
        "89101\n"
        "Dr. A\n"
        "83101, 1, FALL, Obligatory\n"
        "FinalExam\n"
        "$$$$\n"
    );

    InputParser parser;

    expectParseException(
        [&]() {
            parser.parseCourses(path);
        },
        "invalid evaluation type"
    );

    removeFile(path);
}

void testParseCoursesRejectsInvalidOfferingLine() {
    const std::string path = "input_parser_courses_invalid_offering.txt";

    writeFile(
        path,
        "Algorithms\n"
        "89101\n"
        "Dr. A\n"
        "83101, 1, FALL\n"
        "Exam\n"
        "$$$$\n"
    );

    InputParser parser;

    expectParseException(
        [&]() {
            parser.parseCourses(path);
        },
        "invalid offering line"
    );

    removeFile(path);
}

void testParseExamPeriodsWithSingleAndRangeExclusions() {
    const std::string path = "input_parser_periods_valid.txt";

    writeFile(
        path,
        "FALL,Aleph\n"
        "01-01-2026,10-01-2026\n"
        "- 05-01-2026 single day reason\n"
        "- 06-01-2026, 07-01-2026 range reason\n"
        "$$$$\n"
    );

    InputParser parser;
    std::vector<ExamPeriod> periods = parser.parseExamPeriods(path);

    TEST_EXPECT_EQ(periods.size(), static_cast<std::size_t>(1));

    const ExamPeriod& period = periods[0];

    TEST_EXPECT_EQ(period.getSemester(), Semester::FALL);
    TEST_EXPECT_EQ(period.getMoed(), Moed::ALEPH);
    TEST_EXPECT_EQ(period.getStartDate(), Date(1, 1, 2026));
    TEST_EXPECT_EQ(period.getEndDate(), Date(10, 1, 2026));

    const std::vector<ExcludedRange>& exclusions = period.getExcludedRanges();

    TEST_EXPECT_EQ(exclusions.size(), static_cast<std::size_t>(2));

    TEST_EXPECT_EQ(exclusions[0].start, Date(5, 1, 2026));
    TEST_EXPECT_EQ(exclusions[0].end, Date(5, 1, 2026));
    TEST_EXPECT_TRUE(exclusions[0].contains(Date(5, 1, 2026)));
    TEST_EXPECT_FALSE(exclusions[0].contains(Date(4, 1, 2026)));

    TEST_EXPECT_EQ(exclusions[1].start, Date(6, 1, 2026));
    TEST_EXPECT_EQ(exclusions[1].end, Date(7, 1, 2026));
    TEST_EXPECT_TRUE(exclusions[1].contains(Date(6, 1, 2026)));
    TEST_EXPECT_TRUE(exclusions[1].contains(Date(7, 1, 2026)));
    TEST_EXPECT_FALSE(exclusions[1].contains(Date(8, 1, 2026)));

    removeFile(path);
}

void testParseExamPeriodsRejectsInvalidPeriodRange() {
    const std::string path = "input_parser_periods_invalid_range.txt";

    writeFile(
        path,
        "FALL,Aleph\n"
        "10-01-2026,01-01-2026\n"
        "$$$$\n"
    );

    InputParser parser;

    expectParseException(
        [&]() {
            parser.parseExamPeriods(path);
        },
        "exam period start date after end date"
    );

    removeFile(path);
}

void testParseExamPeriodsRejectsInvalidExcludedRange() {
    const std::string path = "input_parser_periods_invalid_excluded_range.txt";

    writeFile(
        path,
        "FALL,Aleph\n"
        "01-01-2026,10-01-2026\n"
        "- 08-01-2026, 06-01-2026 invalid range\n"
        "$$$$\n"
    );

    InputParser parser;

    expectParseException(
        [&]() {
            parser.parseExamPeriods(path);
        },
        "excluded range start date after end date"
    );

    removeFile(path);
}

} // namespace

int main() {
    testValidatorKnownValues();

    testParseSelectedProgramsTrimsAndDeduplicates();
    testParseSelectedProgramsRejectsUnknownProgram();

    testParseCoursesValidCourseWithMultipleOfferings();
    testParseCoursesRejectsInvalidCourseNumber();
    testParseCoursesRejectsInvalidEvaluationType();
    testParseCoursesRejectsInvalidOfferingLine();

    testParseExamPeriodsWithSingleAndRangeExclusions();
    testParseExamPeriodsRejectsInvalidPeriodRange();
    testParseExamPeriodsRejectsInvalidExcludedRange();

    std::cout << "InputParserEdgeCaseTest passed." << std::endl;
    return EXIT_SUCCESS;
}