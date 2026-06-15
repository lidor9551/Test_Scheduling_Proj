#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

/*
 * Utils contains small helper functions used across the project.
 *
 * These functions are defined as inline because this is a header-only utility file.
 * The helpers mainly support parsing and text processing for input files.
 */
namespace Utils {

    // removes leading and trailing whitespace from a string
    /*
     * Removes leading and trailing whitespace characters from a string.
     *
     * This is used heavily by the parser to clean values read from text files.
     */
    inline std::string trim(const std::string& s) {
        /*
         * Find the first character that is not whitespace.
         */
        size_t start = s.find_first_not_of(" \n\r\t\f\v");
        if (start == std::string::npos) {
            return ""; // String is empty or contains only whitespaces
        }

        /*
         * Find the last character that is not whitespace.
         */
        size_t end = s.find_last_not_of(" \n\r\t\f\v");

        /*
         * Return only the meaningful part of the string.
         */
        return s.substr(start, end - start + 1);
    }

    // splits a string into a list by a delimiter (e.g., hyphen in dates)
    /*
     * Splits a string into tokens using a single character delimiter.
     *
     * Examples:
     * - "83101,1,FALL,Obligatory" split by ','.
     * - "29-01-2026" split by '-'.
     */
    inline std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);

        /*
         * Read parts from the stream until no more delimiter-separated tokens exist.
         */
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }

    // checks if a string starts with a specific substring
    /*
     * Checks whether str begins with prefix.
     *
     * This is used by the parser for simple format handling,
     * for example lines that start with a dash.
     */
    inline bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }

    // reads a file completely into a single large string
    /*
     * Reads an entire file into a single string.
     *
     * If the file cannot be opened, an empty string is returned.
     * The parser then decides whether that should be treated as an error.
     */
    inline std::string readFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) return "";

        std::stringstream buffer;

        /*
         * Copy the whole file content into the stringstream buffer.
         */
        buffer << file.rdbuf();

        return buffer.str();
    }

    // filters out empty lines
    /*
     * Splits text into lines, trims every line, and keeps only non-empty lines.
     *
     * This makes record parsing easier because blank lines in input files
     * do not affect the parser logic.
     */
    inline std::vector<std::string> nonEmptyLines(const std::string& text) {
        std::vector<std::string> lines = split(text, '\n');
        std::vector<std::string> result;

        /*
         * Trim each line and keep it only if it still contains content.
         */
        for (const auto& line : lines) {
            std::string trimmed = trim(line);
            if (!trimmed.empty()) {
                result.push_back(trimmed);
            }
        }

        return result;
    }

    // splits a file into records by a delimiter (e.g., $$$$)
    /*
     * Splits a full input file into logical records.
     *
     * In the project input format, "$$$$" separates records such as:
     * - course records
     * - exam period records
     */
    inline std::vector<std::string> splitRecords(const std::string& text, const std::string& delimiter) {
        std::vector<std::string> records;
        size_t start = 0;
        size_t end = text.find(delimiter);

        /*
         * Extract every record that appears before a delimiter.
         */
        while (end != std::string::npos) {
            records.push_back(trim(text.substr(start, end - start)));
            start = end + delimiter.length();
            end = text.find(delimiter, start);
        }

        /*
         * Add the final record after the last delimiter.
         */
        records.push_back(trim(text.substr(start)));

        return records;
    }

}