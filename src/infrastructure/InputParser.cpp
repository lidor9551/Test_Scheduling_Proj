#include "InputParser.h"
#include "utils/Utils.h"
#include <set>
#include <sstream>
#include <algorithm>

/*
 * This anonymous namespace contains helper data and helper functions
 * used only inside this implementation file.
 *
 * Keeping them inside an anonymous namespace gives them internal linkage,
 * so they cannot conflict with symbols from other files.
 */
namespace {
    // TODO: Hardcoded IDs. Consider moving them to an external config file.
    /*
     * Valid academic program IDs supported by the system.
     *
     * These IDs are currently hardcoded according to the project requirements.
     */
    const std::set<std::string> VALID_PROGRAM_IDS = {
        "83101", "83102", "83103", "83104", "83105",
        "83107", "83108", "83109", "83115", "83182"
    };

    /*
     * Valid semester string values as they appear in the input files.
     */
    const std::set<std::string> VALID_SEMESTERS = {"FALL", "SPRI", "SUMM"};

    /*
     * Valid moed string values as they appear in the input files.
     */
    const std::set<std::string> VALID_MOED = {"Aleph", "Bet", "Gimel"};

    // Enum parsers
    /*
     * Converts an evaluation string from the input file into the Evaluation enum.
     *
     * Throws ParseException if the value is unknown.
     */
    Evaluation parseEvaluationType(const std::string& value) {
        if (value == "Exam") return Evaluation::EXAM;
        if (value == "Project") return Evaluation::PROJECT;
        if (value == "Attendance") return Evaluation::ATTENDANCE;
        throw ParseException("Unknown evaluation type: " + value);
    }

    /*
     * Converts a requirement string into the Requirement enum.
     */
    Requirement parseRequirementType(const std::string& value) {
        if (value == "Obligatory") return Requirement::OBLIGATORY;
        if (value == "Elective") return Requirement::ELECTIVE;
        throw ParseException("Unknown requirement type: " + value);
    }

    /*
     * Converts a semester string into the Semester enum.
     */
    Semester parseSemesterType(const std::string& value) {
        if (value == "FALL") return Semester::FALL;
        if (value == "SPRI") return Semester::SPRI;
        if (value == "SUMM") return Semester::SUMM;
        throw ParseException("Unknown semester type: " + value);
    }

    /*
     * Converts a year string into the Year enum.
     */
        Year parseYearType(const std::string& value) {
        if (value == "1") return Year::FIRST;
        if (value == "2") return Year::SECOND;
        if (value == "3") return Year::THIRD;
        if (value == "4") return Year::FOURTH;
        throw ParseException("Invalid year: " + value);
    }

    /*
     * Converts a moed string into the Moed enum.
     */
    Moed parseMoedType(const std::string& value) {
        if (value == "Aleph") return Moed::ALEPH;
        if (value == "Bet") return Moed::BET;
        if (value == "Gimel") return Moed::GIMEL;
        throw ParseException("Unknown moed: " + value);
    }

    // Splits a line into a CSV part and a comment part (used for date exclusions)
    /*
     * Splits an excluded-date line into:
     * - the date/range CSV part
     * - the optional free-text comment
     *
     * This is needed because excluded ranges may contain a reason after the dates.
     */
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

        /*
         * Walk over the tokens and identify date-like values.
         *
         * Once the date part ends, everything after it is treated as a comment.
         */
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

/*
 * Creates a parsing exception with the provided message.
 */
ParseException::ParseException(const std::string& message) : std::runtime_error(message) {}

/*
 * Checks whether a program ID is known to the system.
 */
bool Validator::isValidProgramId(const std::string& programId) {
    return VALID_PROGRAM_IDS.count(programId) > 0;
}

/*
 * Checks whether a semester string is valid.
 */
bool Validator::isValidSemester(const std::string& semester) {
    return VALID_SEMESTERS.count(semester) > 0;
}

/*
 * Checks whether a moed string is valid.
 */
bool Validator::isValidMoed(const std::string& moed) {
    return VALID_MOED.count(moed) > 0;
}

// === Parsing Courses ===

/*
 * Parses the courses file into Course domain objects.
 *
 * The expected format is record-based, where records are separated by "$$$$".
 * Each course record contains:
 * - course name
 * - course number
 * - instructor name
 * - one or more program offering lines
 * - evaluation method
 */
std::vector<Course> InputParser::parseCourses(const std::string& filePath) const {
    const std::string text = Utils::trim(Utils::readFile(filePath));
    if (text.empty()) {
        throw ParseException("Courses file is empty.");
    }

    /*
     * Split the full file into course records.
     */
    std::vector<std::string> records = Utils::splitRecords(text, "$$$$");
    std::vector<Course> courses;

    /*
     * Parse each course record independently.
     */
    for (const std::string& record : records) {
        std::vector<std::string> lines = Utils::nonEmptyLines(record);
        if (lines.empty()) {
            continue;
        }
        if (!lines[0].empty() && lines[0].front() == '#') {
            continue;
        }
        if (lines.size() < 5) {
            throw ParseException("Course record is too short.");
        }

        const std::string& name = lines[0];
        const std::string& number = lines[1];
        const std::string& instructor = lines[2];
        const std::string& evaluationRaw = lines.back();

        /*
         * Course numbers must be exactly five digits.
         */
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
        /*
         * Create the Course object after the basic fields were validated.
         */
        Course course(name, number, instructor, evaluation);

        /*
         * Program offering lines are located between the instructor line
         * and the final evaluation line.
         */
        for (std::size_t i = 3; i + 1 < lines.size(); ++i) {
            std::vector<std::string> parts = Utils::split(lines[i], ',');
            if (parts.size() != 4) {
                throw ParseException("Invalid offering line: " + lines[i]);
            }

            std::string programId = Utils::trim(parts[0]);
            std::string yearText = Utils::trim(parts[1]);
            std::string semesterTxt = Utils::trim(parts[2]);
            std::string requirementText = Utils::trim(parts[3]);

            /*
             * Validate the program ID before adding the offering to the course.
             */
            if (!Validator::isValidProgramId(programId)) {
                throw ParseException("Unknown program id: " + programId);
            }

            Year year;
            Requirement requirement;
            Semester semester;

            try {
                year = parseYearType(yearText);
                requirement = parseRequirementType(requirementText);
                semester = parseSemesterType(semesterTxt);
            } catch (const std::exception& ex) {
                throw ParseException(ex.what());
            }

            // Populate the course offerings using strongly typed domain enums.
            /*
             * Add the validated program offering to the course.
             */
            course.addProgram(programId, year, semester, requirement);
        }

        /*
         * Store the completed Course object.
         */
        courses.push_back(course);
    }

    return courses;
}

// === Parsing Exam Periods ===

/*
 * Parses the exam periods file into ExamPeriod domain objects.
 *
 * The expected format is record-based, where records are separated by "$$$$".
 * Each exam period record contains:
 * - semester and moed
 * - start and end dates
 * - optional excluded dates or excluded date ranges
 */
std::vector<ExamPeriod> InputParser::parseExamPeriods(const std::string& filePath) const {
    const std::string text = Utils::trim(Utils::readFile(filePath));
    if (text.empty()) {
        throw ParseException("Exam periods file is empty.");
    }

    /*
     * Split the full file into separate exam period records.
     */
    std::vector<std::string> records = Utils::splitRecords(text, "$$$$");
    std::vector<ExamPeriod> periods;

    /*
     * Parse each exam period record independently.
     */
    for (const std::string& record : records) {
        std::vector<std::string> lines = Utils::nonEmptyLines(record);
        if (lines.empty()) {
            continue;
        }
        if (!lines[0].empty() && lines[0].front() == '#') {
            continue;
        }
        if (lines.size() < 2) {
            throw ParseException("Exam period record is too short.");
        }

        /*
         * The first line contains the exam period identity:
         * semester and moed.
         */
        std::vector<std::string> headerParts = Utils::split(lines[0], ',');
        if (headerParts.size() != 2) {
            throw ParseException("Invalid exam period header: " + lines[0]);
        }

        std::string semesterText = Utils::trim(headerParts[0]);
        std::string moedText = Utils::trim(headerParts[1]);

        /*
         * Validate the raw semester and moed strings before converting them
         * into strongly typed enum values.
         */
        if (!Validator::isValidSemester(semesterText)) {
            throw ParseException("Invalid semester in exam period: " + semesterText);
        }
        if (!Validator::isValidMoed(moedText)) {
            throw ParseException("Invalid moed: " + moedText);
        }

        Semester semester;
        Moed moed;

        try {
            semester = parseSemesterType(semesterText);
            moed = parseMoedType(moedText);
        } catch (const std::exception& ex) {
            throw ParseException(ex.what());
        }

        /*
         * The second line contains the inclusive date range of the period.
         */
        std::vector<std::string> dateParts = Utils::split(lines[1], ',');
        if (dateParts.size() != 2) {
            throw ParseException("Invalid date range line: " + lines[1]);
        }

        /*
         * Parse the start and end dates of the exam period.
         */
        Date startDate = Date::parse(Utils::trim(dateParts[0]));
        Date endDate = Date::parse(Utils::trim(dateParts[1]));
        if (!(startDate <= endDate)) {
            throw ParseException("Exam period start date must be <= end date.");
        }

        /*
         * Parse all optional excluded dates or excluded ranges.
         *
         * Each excluded line may describe:
         * - one blocked date
         * - a blocked date range
         * - an optional text reason
         */
        std::vector<ExcludedRange> excludedRanges;
        for (std::size_t i = 2; i < lines.size(); ++i) {
            std::string normalized = lines[i];

            /*
             * Some input lines may start with '-'.
             * Remove it before parsing the date content.
             */
            if (Utils::startsWith(normalized, "-")) {
                normalized = Utils::trim(normalized.substr(1));
            }

            /*
             * Split the line into the actual date/range part and the optional reason.
             */
            std::pair<std::string, std::string> pieces = splitComment(normalized);
            std::vector<std::string> rangeParts = Utils::split(pieces.first, ',');
            std::vector<std::string> cleaned;

            /*
             * Trim each part and ignore empty values.
             */
            for (const std::string& part : rangeParts) {
                std::string value = Utils::trim(part);
                if (!value.empty()) {
                    cleaned.push_back(value);
                }
            }

            /*
             * A single date is represented internally as a range whose start
             * and end are the same date.
             */
            if (cleaned.size() == 1) {
                Date single = Date::parse(cleaned[0]);
                excludedRanges.push_back(ExcludedRange{single, single, pieces.second});
            } else if (cleaned.size() == 2) {
                /*
                 * A two-date line represents an inclusive excluded range.
                 */
                Date start = Date::parse(cleaned[0]);
                Date end = Date::parse(cleaned[1]);
                if (!(start <= end)) {
                    throw ParseException("Excluded range start date must be <= end date.");
                }
                excludedRanges.push_back(ExcludedRange{start, end, pieces.second});
            } else {
                /*
                 * Any other structure is invalid because the parser expects
                 * either one date or two dates.
                 */
                throw ParseException("Invalid excluded range line: " + lines[i]);
            }
        }

        /*
         * Create and store the completed ExamPeriod object.
         */
        periods.emplace_back(semester, moed, startDate, endDate, excludedRanges);
    }

    return periods;
}

// === Parsing Selected Programs ===

/*
 * Parses the selected programs file.
 *
 * The file may contain program IDs separated by commas or new lines.
 * The result is validated, deduplicated, and limited to five programs.
 */
std::vector<std::string> InputParser::parseSelectedPrograms(const std::string& filePath) const {
    const std::string text = Utils::trim(Utils::readFile(filePath));
    if (text.empty()) {
        throw ParseException("Selected programs file is empty.");
    }

    /*
     * Normalize new lines into commas so the parser can handle both formats:
     * - one line with comma-separated IDs
     * - multiple lines with one or more IDs
     */
    std::string normalized = text;
    std::replace(normalized.begin(), normalized.end(), '\n', ',');
    std::vector<std::string> tokens = Utils::split(normalized, ',');
    std::vector<std::string> result;
    std::set<std::string> seen;

    /*
     * Validate each selected program ID and keep only the first occurrence
     * of every unique program.
     */
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

    /*
     * At least one program must be selected.
     */
    if (result.empty()) {
        throw ParseException("No selected programs found.");
    }

    /*
     * The requirements allow selecting at most five academic programs.
     */
    if (result.size() > 5) {
        throw ParseException("At most 5 programs may be selected.");
    }

    return result;
}

/*
 * Returns all supported academic program IDs.
 *
 * The IDs are copied from the internal set into a vector for easier use
 * by callers that need a list-like structure.
 */
std::vector<std::string> Validator::getAllProgramIds() {
    return std::vector<std::string>(VALID_PROGRAM_IDS.begin(), VALID_PROGRAM_IDS.end());
}