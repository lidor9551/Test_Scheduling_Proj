#ifndef PARSER_H
#define PARSER_H

#include "Domain.h"
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

class ParseException : public std::runtime_error {
public:
    explicit ParseException(const std::string& message);
};

class Validator {
public:
    static bool isValidProgramId(const std::string& programId);
    static bool isValidSemester(const std::string& semester);
    static bool isValidMoed(const std::string& moed);
};

class CourseFileParser {
public:
    std::vector<Course> parse(const std::string& filePath) const;
};

class ExamPeriodFileParser {
public:
    std::vector<ExamPeriod> parse(const std::string& filePath) const;
};

class SelectedProgramsFileParser {
public:
    std::vector<std::string> parse(const std::string& filePath) const;
};

#endif
