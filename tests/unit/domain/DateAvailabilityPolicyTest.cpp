#include "domain/DateAvailabilityPolicy.h"

#include <cassert>
#include <iostream>
#include <vector>

int main() {
    Date saturday(3, 1, 2026);
    Date sunday(4, 1, 2026);

    assert(DateAvailabilityPolicy::isSaturday(saturday));
    assert(!DateAvailabilityPolicy::isSaturday(sunday));

    ExcludedRange excluded;
    excluded.start = Date(5, 1, 2026);
    excluded.end = Date(5, 1, 2026);
    excluded.reason = "manual";

    std::vector<ExcludedRange> exclusions = {excluded};

    assert(DateAvailabilityPolicy::isAllowedExamDate(Date(1, 1, 2026), exclusions));
    assert(!DateAvailabilityPolicy::isAllowedExamDate(Date(3, 1, 2026), exclusions));
    assert(!DateAvailabilityPolicy::isAllowedExamDate(Date(5, 1, 2026), exclusions));

    std::vector<Date> allowed =
        DateAvailabilityPolicy::allowedDates(
            Date(1, 1, 2026),
            Date(7, 1, 2026),
            exclusions
        );

    for (const Date& date : allowed) {
        assert(date != Date(3, 1, 2026));
        assert(date != Date(5, 1, 2026));
    }

    assert(allowed.size() == 5);

    std::cout << "DateAvailabilityPolicy test passed." << std::endl;
    return 0;
}