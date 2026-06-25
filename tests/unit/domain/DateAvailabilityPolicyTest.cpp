#include "domain/DateAvailabilityPolicy.h"
#include "TestFactories.h"
#include "TestMacros.h"

#include <iostream>
#include <vector>

namespace {

bool containsDate(const std::vector<Date>& dates, const Date& expected) {
    for (const Date& date : dates) {
        if (date == expected) {
            return true;
        }
    }

    return false;
}

} // namespace

int main() {
    Date saturday(3, 1, 2026);
    Date sunday(4, 1, 2026);

    TEST_EXPECT_TRUE(DateAvailabilityPolicy::isSaturday(saturday));
    TEST_EXPECT_FALSE(DateAvailabilityPolicy::isSaturday(sunday));

    ExcludedRange singleDayExcluded = TestFactory::excludedRange(
        Date(5, 1, 2026),
        Date(5, 1, 2026),
        "manual"
    );

    std::vector<ExcludedRange> singleDayExclusions = { singleDayExcluded };

    TEST_EXPECT_TRUE(DateAvailabilityPolicy::isAllowedExamDate(Date(1, 1, 2026), singleDayExclusions));
    TEST_EXPECT_FALSE(DateAvailabilityPolicy::isAllowedExamDate(Date(3, 1, 2026), singleDayExclusions));
    TEST_EXPECT_FALSE(DateAvailabilityPolicy::isAllowedExamDate(Date(5, 1, 2026), singleDayExclusions));

    std::vector<Date> allowed =
        DateAvailabilityPolicy::allowedDates(
            Date(1, 1, 2026),
            Date(7, 1, 2026),
            singleDayExclusions
        );

    for (const Date& date : allowed) {
        TEST_EXPECT_FALSE(DateAvailabilityPolicy::isSaturday(date));
        TEST_EXPECT_NE(date, Date(5, 1, 2026));
    }

    TEST_EXPECT_EQ(allowed.size(), static_cast<std::size_t>(5));

    // Multi-day excluded ranges should be inclusive.
    ExcludedRange multiDayExcluded = TestFactory::excludedRange(
        Date(5, 1, 2026),
        Date(7, 1, 2026),
        "multi-day manual exclusion"
    );

    std::vector<ExcludedRange> multiDayExclusions = { multiDayExcluded };

    TEST_EXPECT_FALSE(DateAvailabilityPolicy::isAllowedExamDate(Date(5, 1, 2026), multiDayExclusions));
    TEST_EXPECT_FALSE(DateAvailabilityPolicy::isAllowedExamDate(Date(6, 1, 2026), multiDayExclusions));
    TEST_EXPECT_FALSE(DateAvailabilityPolicy::isAllowedExamDate(Date(7, 1, 2026), multiDayExclusions));

    TEST_EXPECT_TRUE(DateAvailabilityPolicy::isAllowedExamDate(Date(4, 1, 2026), multiDayExclusions));
    TEST_EXPECT_TRUE(DateAvailabilityPolicy::isAllowedExamDate(Date(8, 1, 2026), multiDayExclusions));

    std::vector<Date> allowedWithMultiDayExclusion =
        DateAvailabilityPolicy::allowedDates(
            Date(1, 1, 2026),
            Date(10, 1, 2026),
            multiDayExclusions
        );

    TEST_EXPECT_FALSE(containsDate(allowedWithMultiDayExclusion, Date(3, 1, 2026)));
    TEST_EXPECT_FALSE(containsDate(allowedWithMultiDayExclusion, Date(5, 1, 2026)));
    TEST_EXPECT_FALSE(containsDate(allowedWithMultiDayExclusion, Date(6, 1, 2026)));
    TEST_EXPECT_FALSE(containsDate(allowedWithMultiDayExclusion, Date(7, 1, 2026)));
    TEST_EXPECT_FALSE(containsDate(allowedWithMultiDayExclusion, Date(10, 1, 2026)));

    TEST_EXPECT_TRUE(containsDate(allowedWithMultiDayExclusion, Date(1, 1, 2026)));
    TEST_EXPECT_TRUE(containsDate(allowedWithMultiDayExclusion, Date(2, 1, 2026)));
    TEST_EXPECT_TRUE(containsDate(allowedWithMultiDayExclusion, Date(4, 1, 2026)));
    TEST_EXPECT_TRUE(containsDate(allowedWithMultiDayExclusion, Date(8, 1, 2026)));
    TEST_EXPECT_TRUE(containsDate(allowedWithMultiDayExclusion, Date(9, 1, 2026)));

    TEST_EXPECT_EQ(allowedWithMultiDayExclusion.size(), static_cast<std::size_t>(5));

    std::cout << "DateAvailabilityPolicy test passed." << std::endl;
    return EXIT_SUCCESS;
}