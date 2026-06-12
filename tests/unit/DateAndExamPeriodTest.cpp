#include "dates/Date.h"
#include "model/ExamPeriod.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#define EXPECT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAILED: " << #condition \
                      << " at line " << __LINE__ << std::endl; \
            return EXIT_FAILURE; \
        } \
    } while (false)

#define EXPECT_EQ(actual, expected) \
    do { \
        if (!((actual) == (expected))) { \
            std::cerr << "FAILED: " << #actual << " == " << #expected \
                      << " at line " << __LINE__ << std::endl; \
            return EXIT_FAILURE; \
        } \
    } while (false)

int main() {
    // Date construction and getters
    Date date(1, 2, 2026);
    EXPECT_EQ(date.getDay(), 1);
    EXPECT_EQ(date.getMonth(), 2);
    EXPECT_EQ(date.getYear(), 2026);
    EXPECT_TRUE(date.isValid());

    // Date parsing and formatting
    Date parsed = Date::parse("01-02-2026");
    EXPECT_EQ(parsed.getDay(), 1);
    EXPECT_EQ(parsed.getMonth(), 2);
    EXPECT_EQ(parsed.getYear(), 2026);
    EXPECT_EQ(parsed.toString(), std::string("01-02-2026"));

    // Date comparison operators
    Date first(1, 2, 2026);
    Date second(2, 2, 2026);

    EXPECT_TRUE(first < second);
    EXPECT_TRUE(first <= second);
    EXPECT_TRUE(second > first);
    EXPECT_TRUE(second >= first);
    EXPECT_TRUE(first == Date(1, 2, 2026));
    EXPECT_TRUE(first != second);

    // Date nextDay logic
    Date next = first.nextDay();
    EXPECT_EQ(next, Date(2, 2, 2026));

    // ExcludedRange contains logic
    ExcludedRange range;
    range.start = Date(3, 2, 2026);
    range.end = Date(5, 2, 2026);
    range.reason = "Test exclusion";

    EXPECT_TRUE(range.contains(Date(3, 2, 2026)));
    EXPECT_TRUE(range.contains(Date(4, 2, 2026)));
    EXPECT_TRUE(range.contains(Date(5, 2, 2026)));
    EXPECT_TRUE(!range.contains(Date(2, 2, 2026)));
    EXPECT_TRUE(!range.contains(Date(6, 2, 2026)));

    // ExamPeriod stores data correctly
    std::vector<ExcludedRange> exclusions = { range };

    ExamPeriod period(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 2, 2026),
        Date(7, 2, 2026),
        exclusions
    );

    EXPECT_TRUE(period.getSemester() == Semester::FALL);
    EXPECT_TRUE(period.getMoed() == Moed::ALEPH);
    EXPECT_EQ(semesterToString(period.getSemester()), std::string("FALL"));
    EXPECT_EQ(moedToString(period.getMoed()), std::string("Aleph"));
    EXPECT_EQ(period.getStartDate(), Date(1, 2, 2026));
    EXPECT_EQ(period.getEndDate(), Date(7, 2, 2026));
    EXPECT_EQ(period.getExcludedRanges().size(), static_cast<std::size_t>(1));

    // ExamPeriod allowedDates should exclude dates inside excluded ranges
    std::vector<Date> allowed = period.allowedDates();

    for (const Date& allowedDate : allowed) {
        EXPECT_TRUE(!range.contains(allowedDate));
    }

    EXPECT_TRUE(!allowed.empty());

    std::cout << "DateAndExamPeriodTest passed." << std::endl;
    return EXIT_SUCCESS;
}