#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace Utils {

    // removes leading and trailing whitespace from a string
    inline std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \n\r\t\f\v");
        if (start == std::string::npos) {
            return ""; // String is empty or contains only whitespaces
        }
        size_t end = s.find_last_not_of(" \n\r\t\f\v");
        return s.substr(start, end - start + 1);
    }

    // splits a string into a list by a delimiter (e.g., hyphen in dates)
    inline std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    // checks if a string starts with a specific substring
    inline bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }

    // reads a file completely into a single large string
    inline std::string readFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) return "";
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    // filters out empty lines
    inline std::vector<std::string> nonEmptyLines(const std::string& text) {
        std::vector<std::string> lines = split(text, '\n');
        std::vector<std::string> result;
        for (const auto& line : lines) {
            std::string trimmed = trim(line);
            if (!trimmed.empty()) {
                result.push_back(trimmed);
            }
        }
        return result;
    }

    // splits a file into records by a delimiter (e.g., $$$$)
    inline std::vector<std::string> splitRecords(const std::string& text, const std::string& delimiter) {
        std::vector<std::string> records;
        size_t start = 0;
        size_t end = text.find(delimiter);
        while (end != std::string::npos) {
            records.push_back(trim(text.substr(start, end - start)));
            start = end + delimiter.length();
            end = text.find(delimiter, start);
        }
        records.push_back(trim(text.substr(start)));
        return records;
    }

}