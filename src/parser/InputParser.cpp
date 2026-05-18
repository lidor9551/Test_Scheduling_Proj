#include "InputParser.h"
#include "../dates/Utils.h"
#include <set>
#include <sstream>
#include <algorithm>

namespace {
    // Note for V1.0: Hardcoded IDs. Consider moving to an external config file in future versions.
    const std::set<std::string> VALID_PROGRAM_IDS = {
        "83101", "83102", "83103", "83104", "83105",
        "83107", "83108", "83109", "83115", "83182"
    };

    const std::set<std::string> VALID_SEMESTERS = {"FALL", "SPRI", "SUMM"};
    const std::set<std::string> VALID_MOED = {"Aleph", "Bet", "Gimel"};

    // Enum parsers
    Evaluation parseEvaluationType(const std::string& value) {
        if (value == "Exam") return Evaluation::EXAM;
        if (value == "Project") return Evaluation::PROJECT;
        if (value == "Attendance") return Evaluation::ATTENDANCE;
        throw ParseException("Unknown evaluation type: " + value);
    }

    Requirement parseRequirementType(const std::string& value) {
        if (value == "Obligatory") return Requirement::OBLIGATORY;
        if (value == "Elective") return Requirement::ELECTIVE;
        throw ParseException("Unknown requirement type: " + value);
    }

    Semester parseSemesterType(const std::string& value) {
        if (value == "FALL") return Semester::FALL;
        if (value == "SPRI") return Semester::SPRI;
        if (value == "SUMM") return Semester::SUMM;
        throw ParseException("Unknown semester type: " + value);
    }

    // Splits a line into a CSV part and a comment part (used for date exclusions)
    std::pair<std::string, std::string> splitComment(const std::string& line) {
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) return {"", ""};

        std::string csvPart;
        int datesSeen = 0;

        for (const std::string& current : tokens) {
            std::string maybeDate = current;
            if (!maybeDate.empty() && maybeDate.back() == ',') {
                maybeDate.pop_back();
            }

            bool isDate = true;
            std::vector<std::string> pieces = Utils::split(maybeDate, '-');
            if (pieces.size() != 3) {
                isDate = false;
            } else {
                for (const std::string& piece : pieces) {
                    if (piece.empty() || !std::all_of(piece.begin(), piece.end(), ::isdigit)) {
                        isDate = false;
                        break;
                    }
                }
            }

            if (isDate) {
                if (!csvPart.empty()) csvPart += " ";
                csvPart += current;
                datesSeen++;
                if (datesSeen <= 2) continue;
            }

            std::string comment = Utils::trim(line.substr(csvPart.size()));
            return {Utils::trim(csvPart), comment};
        }

        return {Utils::trim(line), ""};
    }
}

ParseException::ParseException(const std::string& message) : std::runtime_error(message) {}

bool Validator::isValidProgramId(const std::string& programId) {
    return VALID_PROGRAM_IDS.count(programId) > 0;
}

bool Validator::isValidSemester(const std::string& semester) {
    return VALID_SEMESTERS.count(semester) > 0;
}

bool Validator::isValidMoed(const std::string& moed) {
    return VALID_MOED.count(moed) > 0;
}

// === Parsing Courses ===

std::vector<Course> InputParser::parseCourses(const std::string& filePath) const {
    const std::string text = Utils::trim(Utils::readFile(filePath));
    if (text.empty()) {
        throw ParseException("Courses file is empty.");
    }

    std::vector<std::string> records = Utils::splitRecords(text, "$$$$");
    std::vector<Course> courses;

    for (const std::string& record : records) {
        std::vector<std::string> lines = Utils::nonEmptyLines(record);
        if (lines.size() < 5) {
            throw ParseException("Course record is too short.");
        }

        const std::string& name = lines[0];
        const std::string& number = lines[1];
        const std::string& instructor = lines[2];
        const std::string& evaluationRaw = lines.back();

        if (number.size() != 5 || !std::all_of(number.begin(), number.end(), ::isdigit)) {
            throw ParseException("Invalid course number: " + number);
        }

        Evaluation evaluation;
        try {
            evaluation = parseEvaluationType(evaluationRaw);
        } catch (const std::exception& ex) {
            throw ParseException(ex.what());
        }

        // Initialize clean OOP Course
        Course course(name, number, instructor, evaluation);

        for (std::size_t i = 3; i + 1 < lines.size(); ++i) {
            std::vector<std::string> parts = Utils::split(lines[i], ',');
            if (parts.size() != 4) {
                throw ParseException("Invalid offering line: " + lines[i]);
            }

            std::string programId = Utils::trim(parts[0]);
            std::string yearText = Utils::trim(parts[1]);
            std::string semesterTxt = Utils::trim(parts[2]);
            std::string requirementText = Utils::trim(parts[3]);

            if (!Validator::isValidProgramId(programId)) {
                throw ParseException("Unknown program id: " + programId);
            }

            int year = std::stoi(yearText);
            if (year < 1 || year > 4) {
                throw ParseException("Invalid year: " + yearText);
            }

            Requirement requirement;
            Semester semester;
            try {
                requirement = parseRequirementType(requirementText);
                semester = parseSemesterType(semesterTxt);
            } catch (const std::exception& ex) {
                throw ParseException(ex.what());
            }

            // Populate the course offerings
            course.addProgram(programId, year, semester, requirement);
        }

        courses.push_back(course);
    }

    return courses;
}

// === Parsing Exam Periods ===

std::vector<ExamPeriod> InputParser::parseExamPeriods(const std::string& filePath) const {
    const std::string text = Utils::trim(Utils::readFile(filePath));
    if (text.empty()) {
        throw ParseException("Exam periods file is empty.");
    }

    std::vector<std::string> records = Utils::splitRecords(text, "$$$$");
    std::vector<ExamPeriod> periods;

    for (const std::string& record : records) {
        std::vector<std::string> lines = Utils::nonEmptyLines(record);
        if (lines.size() < 2) {
            throw ParseException("Exam period record is too short.");
        }

        std::vector<std::string> headerParts = Utils::split(lines[0], ',');
        if (headerParts.size() != 2) {
            throw ParseException("Invalid exam period header: " + lines[0]);
        }

        std::string semester = Utils::trim(headerParts[0]);
        std::string moed = Utils::trim(headerParts[1]);

        if (!Validator::isValidSemester(semester)) {
            throw ParseException("Invalid semester in exam period: " + semester);
        }
        if (!Validator::isValidMoed(moed)) {
            throw ParseException("Invalid moed: " + moed);
        }

        std::vector<std::string> dateParts = Utils::split(lines[1], ',');
        if (dateParts.size() != 2) {
            throw ParseException("Invalid date range line: " + lines[1]);
        }

        Date startDate = Date::parse(Utils::trim(dateParts[0]));
        Date endDate = Date::parse(Utils::trim(dateParts[1]));
        if (!(startDate <= endDate)) {
            throw ParseException("Exam period start date must be <= end date.");
        }

        std::vector<ExcludedRange> excludedRanges;
        for (std::size_t i = 2; i < lines.size(); ++i) {
            std::string normalized = lines[i];
            if (Utils::startsWith(normalized, "-")) {
                normalized = Utils::trim(normalized.substr(1));
            }

            std::pair<std::string, std::string> pieces = splitComment(normalized);
            std::vector<std::string> rangeParts = Utils::split(pieces.first, ',');
            std::vector<std::string> cleaned;
            for (const std::string& part : rangeParts) {
                std::string value = Utils::trim(part);
                if (!value.empty()) {
                    cleaned.push_back(value);
                }
            }

            if (cleaned.size() == 1) {
                Date single = Date::parse(cleaned[0]);
                excludedRanges.push_back(ExcludedRange{single, single, pieces.second});
            } else if (cleaned.size() == 2) {
                Date start = Date::parse(cleaned[0]);
                Date end = Date::parse(cleaned[1]);
                if (!(start <= end)) {
                    throw ParseException("Excluded range start date must be <= end date.");
                }
                excludedRanges.push_back(ExcludedRange{start, end, pieces.second});
            } else {
                throw ParseException("Invalid excluded range line: " + lines[i]);
            }
        }

        periods.emplace_back(semester, moed, startDate, endDate, excludedRanges);
    }

    return periods;
}

// === Parsing Selected Programs ===

std::vector<std::string> InputParser::parseSelectedPrograms(const std::string& filePath) const {
    const std::string text = Utils::trim(Utils::readFile(filePath));
    if (text.empty()) {
        throw ParseException("Selected programs file is empty.");
    }

    std::string normalized = text;
    std::replace(normalized.begin(), normalized.end(), '\n', ',');
    std::vector<std::string> tokens = Utils::split(normalized, ',');
    std::vector<std::string> result;
    std::set<std::string> seen;

    for (const std::string& token : tokens) {
        std::string programId = Utils::trim(token);
        if (programId.empty()) {
            continue;
        }
        if (!Validator::isValidProgramId(programId)) {
            throw ParseException("Unknown selected program id: " + programId);
        }
        if (seen.insert(programId).second) {
            result.push_back(programId);
        }
    }

    if (result.empty()) {
        throw ParseException("No selected programs found.");
    }
    if (result.size() > 5) {
        throw ParseException("At most 5 programs may be selected.");
    }

    return result;
}