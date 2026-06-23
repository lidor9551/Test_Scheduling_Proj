#pragma once

#include <cstdlib>
#include <iostream>

#define TEST_EXPECT_TRUE(condition)                                      \
    do {                                                                \
        if (!(condition)) {                                             \
            std::cerr << "FAILED: " << #condition                       \
                      << " at " << __FILE__ << ":" << __LINE__          \
                      << std::endl;                                     \
            return EXIT_FAILURE;                                        \
        }                                                               \
    } while (false)

#define TEST_EXPECT_FALSE(condition)                                     \
    do {                                                                \
        if ((condition)) {                                              \
            std::cerr << "FAILED: expected false: " << #condition        \
                      << " at " << __FILE__ << ":" << __LINE__          \
                      << std::endl;                                     \
            return EXIT_FAILURE;                                        \
        }                                                               \
    } while (false)

#define TEST_EXPECT_EQ(actual, expected)                                \
    do {                                                                \
        if (!((actual) == (expected))) {                                \
            std::cerr << "FAILED: " << #actual << " == " << #expected   \
                      << " at " << __FILE__ << ":" << __LINE__          \
                      << std::endl;                                     \
            return EXIT_FAILURE;                                        \
        }                                                               \
    } while (false)

#define TEST_EXPECT_NE(actual, expected)                                \
    do {                                                                \
        if (!((actual) != (expected))) {                                \
            std::cerr << "FAILED: " << #actual << " != " << #expected   \
                      << " at " << __FILE__ << ":" << __LINE__          \
                      << std::endl;                                     \
            return EXIT_FAILURE;                                        \
        }                                                               \
    } while (false)