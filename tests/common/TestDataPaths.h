#pragma once

#include <string>

namespace TestDataPaths {

inline std::string validSimple() {
    return "tests/data/valid/simple";
}

inline std::string validIntegrationTest() {
    return "tests/data/valid/integration_test";
}

inline std::string validOriginTest() {
    return "tests/data/valid/origin_test";
}

inline std::string invalidRoot() {
    return "tests/data/invalid";
}

inline std::string noSolutionRoot() {
    return "tests/data/no_solution";
}

inline std::string overlapRoot() {
    return "tests/data/overlap";
}

inline std::string stage3Root() {
    return "tests/data/stage3";
}

inline std::string performanceRoot() {
    return "tests/data/performance";
}

} // namespace TestDataPaths