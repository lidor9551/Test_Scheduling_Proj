#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include "../model/Course.h"
#include "../model/ExamPeriod.h"

// Exception class for any parsing errors
class ParseException : public std::runtime_error {
public:
    explicit ParseException(const std::string& message);
};

// Validates IDs, Semesters, and Moeds against allowed sets
class Validator {
public:
    static bool isValidProgramId(const std::string& programId);
    static bool isValidSemester(const std::string& semester);
    static bool isValidMoed(const std::string& moed);
};

// Main Parser class handling all file inputs
class InputParser {
public:
    // Parses courses.txt and returns a list of Course objects
    std::vector<Course> parseCourses(const std::string& filePath) const;

    // Parses periods.txt and returns a list of ExamPeriod objects
    std::vector<ExamPeriod> parseExamPeriods(const std::string& filePath) const;

    // Parses selected_programs.txt and returns a list of selected program IDs
    std::vector<std::string> parseSelectedPrograms(const std::string& filePath) const;
};