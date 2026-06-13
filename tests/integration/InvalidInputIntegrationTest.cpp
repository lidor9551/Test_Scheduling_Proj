#include "infrastructure/InputParser.h"
#include <cstdlib>
#include <iostream>
#include <string>

#define EXPECT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAILED: " << #condition \
                      << " at line " << __LINE__ << std::endl; \
            return EXIT_FAILURE; \
        } \
    } while (false)

int main() {
    const std::string basePath = "../tests/data/invalid/missing-fields/";

    InputParser parser;

    bool exceptionThrown = false;

    try {
        parser.parseCourses(basePath + "courses.txt");
    } catch (const ParseException& ex) {
        exceptionThrown = true;
    } catch (const std::exception& ex) {
        exceptionThrown = true;
    }

    EXPECT_TRUE(exceptionThrown);

    std::cout << "InvalidInputIntegrationTest passed." << std::endl;
    return EXIT_SUCCESS;
}