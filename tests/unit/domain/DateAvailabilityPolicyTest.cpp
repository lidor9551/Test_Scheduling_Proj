#include "domain/DateAvailabilityPolicy.h"
#include "TestMacros.h"

#include <iostream>
#include <vector>

int main() {
    Date saturday(3, 1, 2026);
    Date sunday(4, 1, 2026);

    TEST_EXPECT_TRUE(DateAvailabilityPolicy::isSaturday(saturday));
    TEST_EXPECT_FALSE(DateAvailabilityPolicy::isSaturday(sunday));

    ExcludedRange excluded;
    excluded.start = Date(5, 1, 2026);
    excluded.end = Date(5, 1, 2026);
    excluded.reason = "manual";

    std::vector<ExcludedRange> exclusions = { excluded };

    TEST_EXPECT_TRUE(DateAvailabilityPolicy::isAllowedExamDate(Date(1, 1, 2026), exclusions));
    TEST_EXPECT_FALSE(DateAvailabilityPolicy::isAllowedExamDate(Date(3, 1, 2026), exclusions));
    TEST_EXPECT_FALSE(DateAvailabilityPolicy::isAllowedExamDate(Date(5, 1, 2026), exclusions));

    std::vector<Date> allowed =
        DateAvailabilityPolicy::allowedDates(
            Date(1, 1, 2026),
            Date(7, 1, 2026),
            exclusions
        );

    for (const Date& date : allowed) {
        TEST_EXPECT_NE(date, Date(3, 1, 2026));
        TEST_EXPECT_NE(date, Date(5, 1, 2026));
    }

    TEST_EXPECT_EQ(allowed.size(), static_cast<std::size_t>(5));

    std::cout << "DateAvailabilityPolicy test passed." << std::endl;
    return EXIT_SUCCESS;
}