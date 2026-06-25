#include "TestFactories.h"
#include "TestMacros.h"
#include "ScheduleAssertions.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expectInvalidDateConstruction(int day, int month, int year) {
    try {
        Date invalidDate(day, month, year);
        (void)invalidDate;

        TEST_FAIL("FAILED: expected std::invalid_argument for Date("
                  << day << ", " << month << ", " << year << ")");
    } catch (const std::invalid_argument&) {
        // Expected path.
    } catch (...) {
        TEST_FAIL("FAILED: expected std::invalid_argument for invalid Date construction");
    }
}

void expectInvalidDateParse(const std::string& input) {
    try {
        Date parsed = Date::parse(input);
        (void)parsed;

        TEST_FAIL("FAILED: expected std::invalid_argument for Date::parse(\""
                  << input << "\")");
    } catch (const std::invalid_argument&) {
        // Expected path.
    } catch (...) {
        TEST_FAIL("FAILED: expected std::invalid_argument for invalid Date parse input");
    }
}

} // namespace

int main() {
    // Date construction and getters
    Date date(1, 2, 2026);
    TEST_EXPECT_EQ(date.getDay(), 1);
    TEST_EXPECT_EQ(date.getMonth(), 2);
    TEST_EXPECT_EQ(date.getYear(), 2026);
    TEST_EXPECT_TRUE(date.isValid());

    // Date parsing and formatting
    Date parsed = Date::parse("01-02-2026");
    TEST_EXPECT_EQ(parsed.getDay(), 1);
    TEST_EXPECT_EQ(parsed.getMonth(), 2);
    TEST_EXPECT_EQ(parsed.getYear(), 2026);
    TEST_EXPECT_EQ(parsed.toString(), std::string("01-02-2026"));

    // Date parsing should trim outer whitespace
    Date parsedWithSpaces = Date::parse(" 01-02-2026 ");
    TEST_EXPECT_EQ(parsedWithSpaces, Date(1, 2, 2026));

    // Invalid Date construction should throw
    expectInvalidDateConstruction(0, 1, 2026);
    expectInvalidDateConstruction(31, 4, 2026);
    expectInvalidDateConstruction(29, 2, 2025);
    expectInvalidDateConstruction(1, 13, 2026);

    // Invalid Date parse inputs should throw
    expectInvalidDateParse("01/02/2026");
    expectInvalidDateParse("01-02");
    expectInvalidDateParse("not-a-date");
    expectInvalidDateParse("31-04-2026");

    // Date comparison operators
    Date first(1, 2, 2026);
    Date second(2, 2, 2026);

    TEST_EXPECT_TRUE(first < second);
    TEST_EXPECT_TRUE(first <= second);
    TEST_EXPECT_TRUE(second > first);
    TEST_EXPECT_TRUE(second >= first);
    TEST_EXPECT_TRUE(first == Date(1, 2, 2026));
    TEST_EXPECT_TRUE(first != second);

    // Date nextDay logic
    Date next = first.nextDay();
    TEST_EXPECT_EQ(next, Date(2, 2, 2026));

    // Date nextDay should handle month boundaries
    TEST_EXPECT_EQ(Date(28, 2, 2026).nextDay(), Date(1, 3, 2026));
    TEST_EXPECT_EQ(Date(31, 12, 2026).nextDay(), Date(1, 1, 2027));

    // Date nextDay should handle leap years
    TEST_EXPECT_EQ(Date(28, 2, 2024).nextDay(), Date(29, 2, 2024));
    TEST_EXPECT_EQ(Date(29, 2, 2024).nextDay(), Date(1, 3, 2024));

    // daysTo should be absolute and symmetric
    Date start(1, 1, 2026);
    Date end(10, 1, 2026);

    TEST_EXPECT_EQ(start.daysTo(end), 9);
    TEST_EXPECT_EQ(end.daysTo(start), 9);
    TEST_EXPECT_EQ(start.daysTo(start), 0);

    // ExcludedRange contains logic
    ExcludedRange range = TestFactory::excludedRange(
        Date(3, 2, 2026),
        Date(5, 2, 2026),
        "Test exclusion"
    );

    TEST_EXPECT_TRUE(range.contains(Date(3, 2, 2026)));
    TEST_EXPECT_TRUE(range.contains(Date(4, 2, 2026)));
    TEST_EXPECT_TRUE(range.contains(Date(5, 2, 2026)));
    TEST_EXPECT_FALSE(range.contains(Date(2, 2, 2026)));
    TEST_EXPECT_FALSE(range.contains(Date(6, 2, 2026)));

    // ExamPeriod stores data correctly
    std::vector<ExcludedRange> exclusions = { range };

    ExamPeriod period = TestFactory::examPeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 2, 2026),
        Date(7, 2, 2026),
        exclusions
    );

    TEST_EXPECT_TRUE(period.getSemester() == Semester::FALL);
    TEST_EXPECT_TRUE(period.getMoed() == Moed::ALEPH);
    TEST_EXPECT_EQ(semesterToString(period.getSemester()), std::string("FALL"));
    TEST_EXPECT_EQ(moedToString(period.getMoed()), std::string("Aleph"));
    TEST_EXPECT_EQ(period.getStartDate(), Date(1, 2, 2026));
    TEST_EXPECT_EQ(period.getEndDate(), Date(7, 2, 2026));
    TEST_EXPECT_EQ(period.getExcludedRanges().size(), static_cast<std::size_t>(1));

    // ExamPeriod allowedDates should exclude dates inside excluded ranges
    std::vector<Date> allowed = period.allowedDates();

    TEST_EXPECT_TRUE(!allowed.empty());
    TEST_EXPECT_TRUE(ScheduleAssertions::allDatesInsidePeriod(allowed, period));
    TEST_EXPECT_TRUE(ScheduleAssertions::noDateInsideExcludedRange(allowed, range));

    std::cout << "DateAndExamPeriodTest passed." << std::endl;
    return EXIT_SUCCESS;
}