#pragma once

#include <cstdlib>
#include <iostream>

#define TEST_FAIL(message)                                              \
    do {                                                                \
        std::cerr << message                                            \
                  << " at " << __FILE__ << ":" << __LINE__              \
                  << std::endl;                                         \
        std::exit(EXIT_FAILURE);                                        \
    } while (false)

#define TEST_EXPECT_TRUE(condition)                                     \
    do {                                                                \
        if (!(condition)) {                                             \
            TEST_FAIL("FAILED: " << #condition);                        \
        }                                                               \
    } while (false)

#define TEST_EXPECT_FALSE(condition)                                    \
    do {                                                                \
        if ((condition)) {                                              \
            TEST_FAIL("FAILED: expected false: " << #condition);        \
        }                                                               \
    } while (false)

#define TEST_EXPECT_EQ(actual, expected)                                \
    do {                                                                \
        if (!((actual) == (expected))) {                                \
            TEST_FAIL("FAILED: " << #actual << " == " << #expected);    \
        }                                                               \
    } while (false)

#define TEST_EXPECT_NE(actual, expected)                                \
    do {                                                                \
        if (!((actual) != (expected))) {                                \
            TEST_FAIL("FAILED: " << #actual << " != " << #expected);    \
        }                                                               \
    } while (false)