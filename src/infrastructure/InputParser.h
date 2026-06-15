#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include "domain/Course.h"
#include "domain/ExamPeriod.h"

/*
 * InputParser belongs to the infrastructure layer.
 *
 * This layer is responsible for reading external files and converting them
 * into domain objects used by the rest of the application.
 *
 * The parser validates the file format and throws ParseException whenever
 * the input does not match the expected structure.
 */

// Exception class for any parsing errors
/*
 * ParseException represents an input-format or validation error.
 *
 * It is used when the parser cannot safely convert a file into domain objects.
 */
class ParseException : public std::runtime_error {
public:
    // Creates a parsing exception with a readable error message.
    explicit ParseException(const std::string& message);
};

// Validates IDs, Semesters, and Moeds against allowed sets
/*
 * Validator provides basic validation helpers for known domain values.
 *
 * It checks whether program IDs, semester names, and moed values are part
 * of the allowed input vocabulary.
 */
class Validator {
public:
    // Returns true if the program ID exists in the supported program list.
    static bool isValidProgramId(const std::string& programId);

    // Returns true if the semester string is one of the supported semester values.
    static bool isValidSemester(const std::string& semester);

    // Returns true if the moed string is one of the supported moed values.
    static bool isValidMoed(const std::string& moed);

    // Returns all supported program IDs.
    std::vector<std::string> getAllProgramIds();
};

// Main Parser class handling all file inputs
/*
 * InputParser reads the project input files and creates typed domain objects.
 *
 * It supports:
 * - course data files
 * - exam period files
 * - selected programs files
 */
class InputParser {
public:
    // Parses courses.txt and returns a list of Course objects
    /*
     * Parses the courses input file.
     *
     * Each valid course record becomes one Course object.
     */
    std::vector<Course> parseCourses(const std::string& filePath) const;

    // Parses periods.txt and returns a list of ExamPeriod objects
    /*
     * Parses the exam periods input file.
     *
     * Each valid exam period record becomes one ExamPeriod object.
     */
    std::vector<ExamPeriod> parseExamPeriods(const std::string& filePath) const;

    // Parses selected_programs.txt and returns a list of selected program IDs
    /*
     * Parses the selected programs input file.
     *
     * The result is validated, deduplicated, and limited to at most five programs.
     */
    std::vector<std::string> parseSelectedPrograms(const std::string& filePath) const;
};