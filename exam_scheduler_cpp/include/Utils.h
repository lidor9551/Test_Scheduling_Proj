#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Utils {

inline std::string trim(const std::string& value) {
    std::size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

inline std::vector<std::string> split(const std::string& text, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(text);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
}

inline std::string readFile(const std::string& filePath) {
    std::ifstream input(filePath);
    if (!input) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

inline bool startsWith(const std::string& text, const std::string& prefix) {
    return text.rfind(prefix, 0) == 0;
}

inline std::vector<std::string> splitRecords(const std::string& text, const std::string& delimiter) {
    std::vector<std::string> records;
    std::size_t start = 0;

    while (true) {
        std::size_t pos = text.find(delimiter, start);
        std::string record = (pos == std::string::npos)
            ? text.substr(start)
            : text.substr(start, pos - start);

        record = trim(record);
        if (!record.empty()) {
            records.push_back(record);
        }

        if (pos == std::string::npos) {
            break;
        }
        start = pos + delimiter.size();
    }

    return records;
}

inline std::vector<std::string> nonEmptyLines(const std::string& record) {
    std::vector<std::string> result;
    std::stringstream ss(record);
    std::string line;

    while (std::getline(ss, line)) {
        line = trim(line);
        if (!line.empty()) {
            result.push_back(line);
        }
    }

    return result;
}

} // namespace Utils

#endif
