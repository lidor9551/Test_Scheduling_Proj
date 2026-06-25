#include "presentation/CalendarManager.h"
#include "TestMacros.h"

#include <QCoreApplication>
#include <QDate>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

ExamPeriod makePeriod(
    Semester semester,
    Moed moed,
    const Date& start,
    const Date& end,
    const std::vector<ExcludedRange>& exclusions = {}
) {
    return ExamPeriod(semester, moed, start, end, exclusions);
}

ExcludedRange makeExcludedRange(
    const Date& start,
    const Date& end,
    const std::string& reason = "manual"
) {
    ExcludedRange range;
    range.start = start;
    range.end = end;
    range.reason = reason;
    return range;
}

QVariantList makePeriodList(const std::vector<ExamPeriod>& periods) {
    QVariantList list;

    for (const ExamPeriod& period : periods) {
        list.append(QVariant::fromValue(period));
    }

    return list;
}

QVariantMap findDay(const QVariantList& days, const QString& dateText) {
    const QDate expectedDate = QDate::fromString(dateText, "yyyy-MM-dd");

    for (const QVariant& value : days) {
        QVariantMap day = value.toMap();

        if (day.value("date").toDate() == expectedDate) {
            return day;
        }
    }

    return {};
}

int dayStatus(const QVariantList& days, const QString& dateText) {
    QVariantMap day = findDay(days, dateText);

    if (day.isEmpty()) {
        TEST_FAIL("FAILED: date was not found in CalendarManager days: " << dateText.toStdString());
    }

    return day.value("status").toInt();
}

bool containsDate(const QVariantList& days, const QString& dateText) {
    return !findDay(days, dateText).isEmpty();
}

void testDefaultState() {
    CalendarManager manager;

    TEST_EXPECT_TRUE(manager.getDays().isEmpty());
    TEST_EXPECT_TRUE(manager.getCurrentSemester().isEmpty());
    TEST_EXPECT_TRUE(manager.getCurrentStartDate().isEmpty());
    TEST_EXPECT_TRUE(manager.getCurrentEndDate().isEmpty());
    TEST_EXPECT_TRUE(manager.getSemesterList().isEmpty());
    TEST_EXPECT_TRUE(manager.getPeriodTree().isEmpty());
    TEST_EXPECT_TRUE(manager.getPeriods().empty());
}

void testSetDataBuildsDaysAndCurrentPeriodMetadata() {
    ExcludedRange excluded = makeExcludedRange(
        Date(5, 1, 2026),
        Date(5, 1, 2026),
        "single day exclusion"
    );

    ExamPeriod period = makePeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(7, 1, 2026),
        { excluded }
    );

    CalendarManager manager;
    manager.setData(makePeriodList({ period }), QVariantList{});

    TEST_EXPECT_EQ(manager.getCurrentSemester(), QString("FALL - Aleph"));
    TEST_EXPECT_EQ(manager.getCurrentStartDate(), QString("2026-01-01"));
    TEST_EXPECT_EQ(manager.getCurrentEndDate(), QString("2026-01-07"));

    QVariantList days = manager.getDays();

    TEST_EXPECT_EQ(days.size(), 7);

    TEST_EXPECT_TRUE(containsDate(days, "2026-01-01"));
    TEST_EXPECT_TRUE(containsDate(days, "2026-01-02"));
    TEST_EXPECT_TRUE(containsDate(days, "2026-01-03"));
    TEST_EXPECT_TRUE(containsDate(days, "2026-01-04"));
    TEST_EXPECT_TRUE(containsDate(days, "2026-01-05"));
    TEST_EXPECT_TRUE(containsDate(days, "2026-01-06"));
    TEST_EXPECT_TRUE(containsDate(days, "2026-01-07"));

    // 03-01-2026 is Saturday and should be excluded by DateAvailabilityPolicy.
    TEST_EXPECT_EQ(dayStatus(days, "2026-01-03"), 2);

    // 05-01-2026 is explicitly excluded.
    TEST_EXPECT_EQ(dayStatus(days, "2026-01-05"), 2);

    // Regular days should be active.
    TEST_EXPECT_EQ(dayStatus(days, "2026-01-01"), 1);
    TEST_EXPECT_EQ(dayStatus(days, "2026-01-02"), 1);
    TEST_EXPECT_EQ(dayStatus(days, "2026-01-04"), 1);
    TEST_EXPECT_EQ(dayStatus(days, "2026-01-06"), 1);
    TEST_EXPECT_EQ(dayStatus(days, "2026-01-07"), 1);
}

void testSemesterListAndPeriodTree() {
    ExamPeriod fallAleph = makePeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(7, 1, 2026)
    );

    ExamPeriod springBet = makePeriod(
        Semester::SPRI,
        Moed::BET,
        Date(1, 2, 2026),
        Date(3, 2, 2026)
    );

    CalendarManager manager;
    manager.setData(makePeriodList({ fallAleph, springBet }), QVariantList{});

    QVariantList semesterList = manager.getSemesterList();

    TEST_EXPECT_EQ(semesterList.size(), 2);
    TEST_EXPECT_EQ(semesterList[0].toString(), QString("FALL - Aleph"));
    TEST_EXPECT_EQ(semesterList[1].toString(), QString("SPRI - Bet"));

    QVariantList tree = manager.getPeriodTree();

    TEST_EXPECT_EQ(tree.size(), 2);

    QVariantMap first = tree[0].toMap();
    TEST_EXPECT_EQ(first.value("year").toInt(), 2026);
    TEST_EXPECT_EQ(first.value("semester").toString(), QString("FALL"));
    TEST_EXPECT_EQ(first.value("moed").toString(), QString("Aleph"));
    TEST_EXPECT_EQ(first.value("index").toInt(), 0);

    QVariantMap second = tree[1].toMap();
    TEST_EXPECT_EQ(second.value("year").toInt(), 2026);
    TEST_EXPECT_EQ(second.value("semester").toString(), QString("SPRI"));
    TEST_EXPECT_EQ(second.value("moed").toString(), QString("Bet"));
    TEST_EXPECT_EQ(second.value("index").toInt(), 1);
}

void testSelectPeriodAndNavigation() {
    ExamPeriod fallAleph = makePeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(2, 1, 2026)
    );

    ExamPeriod springBet = makePeriod(
        Semester::SPRI,
        Moed::BET,
        Date(1, 2, 2026),
        Date(3, 2, 2026)
    );

    CalendarManager manager;
    manager.setData(makePeriodList({ fallAleph, springBet }), QVariantList{});

    TEST_EXPECT_EQ(manager.getCurrentSemester(), QString("FALL - Aleph"));
    TEST_EXPECT_EQ(manager.getDays().size(), 2);

    manager.selectPeriod(1);

    TEST_EXPECT_EQ(manager.getCurrentSemester(), QString("SPRI - Bet"));
    TEST_EXPECT_EQ(manager.getCurrentStartDate(), QString("2026-02-01"));
    TEST_EXPECT_EQ(manager.getCurrentEndDate(), QString("2026-02-03"));
    TEST_EXPECT_EQ(manager.getDays().size(), 3);

    manager.previousSemester();

    TEST_EXPECT_EQ(manager.getCurrentSemester(), QString("FALL - Aleph"));
    TEST_EXPECT_EQ(manager.getDays().size(), 2);

    manager.nextSemester();

    TEST_EXPECT_EQ(manager.getCurrentSemester(), QString("SPRI - Bet"));
    TEST_EXPECT_EQ(manager.getDays().size(), 3);

    // Out-of-range selection should not change the current period.
    manager.selectPeriod(99);

    TEST_EXPECT_EQ(manager.getCurrentSemester(), QString("SPRI - Bet"));
}

void testToggleDayChangesVisibleStatus() {
    ExamPeriod period = makePeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(4, 1, 2026)
    );

    CalendarManager manager;
    manager.setData(makePeriodList({ period }), QVariantList{});

    TEST_EXPECT_EQ(dayStatus(manager.getDays(), "2026-01-02"), 1);

    manager.toggleDay("2026-01-02");

    TEST_EXPECT_EQ(dayStatus(manager.getDays(), "2026-01-02"), 2);

    manager.toggleDay("2026-01-02");

    TEST_EXPECT_EQ(dayStatus(manager.getDays(), "2026-01-02"), 1);
}

void testToggleInvalidOrMissingDayDoesNotChangeCalendar() {
    ExamPeriod period = makePeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(4, 1, 2026)
    );

    CalendarManager manager;
    manager.setData(makePeriodList({ period }), QVariantList{});

    QVariantList before = manager.getDays();

    manager.toggleDay("not-a-date");
    manager.toggleDay("2026-01-20");

    QVariantList after = manager.getDays();

    TEST_EXPECT_EQ(after.size(), before.size());

    TEST_EXPECT_EQ(dayStatus(after, "2026-01-01"), dayStatus(before, "2026-01-01"));
    TEST_EXPECT_EQ(dayStatus(after, "2026-01-02"), dayStatus(before, "2026-01-02"));
    TEST_EXPECT_EQ(dayStatus(after, "2026-01-03"), dayStatus(before, "2026-01-03"));
    TEST_EXPECT_EQ(dayStatus(after, "2026-01-04"), dayStatus(before, "2026-01-04"));
}

void testSaveChangesFlushesExcludedDaysBackToPeriods() {
    ExamPeriod period = makePeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(4, 1, 2026)
    );

    CalendarManager manager;
    manager.setData(makePeriodList({ period }), QVariantList{});

    manager.toggleDay("2026-01-02");
    manager.saveChanges();

    std::vector<ExamPeriod> periods = manager.getPeriods();

    TEST_EXPECT_EQ(periods.size(), static_cast<std::size_t>(1));

    const std::vector<ExcludedRange>& exclusions = periods[0].getExcludedRanges();

    TEST_EXPECT_EQ(exclusions.size(), static_cast<std::size_t>(2));

    bool hasToggledDate = false;
    bool hasSaturday = false;

    for (const ExcludedRange& exclusion : exclusions) {
        if (exclusion.contains(Date(2, 1, 2026))) {
            hasToggledDate = true;
        }

        if (exclusion.contains(Date(3, 1, 2026))) {
            hasSaturday = true;
        }
    }

    TEST_EXPECT_TRUE(hasToggledDate);
    TEST_EXPECT_TRUE(hasSaturday);
}

void testShiftPeriodChangesBoundsAndRebuildsDays() {
    ExamPeriod period = makePeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(7, 1, 2026)
    );

    CalendarManager manager;
    manager.setData(makePeriodList({ period }), QVariantList{});

    manager.shiftPeriod("FALL", "2026-01-02", "2026-01-04");

    TEST_EXPECT_EQ(manager.getCurrentStartDate(), QString("2026-01-02"));
    TEST_EXPECT_EQ(manager.getCurrentEndDate(), QString("2026-01-04"));

    QVariantList days = manager.getDays();

    TEST_EXPECT_EQ(days.size(), 3);

    TEST_EXPECT_TRUE(containsDate(days, "2026-01-02"));
    TEST_EXPECT_TRUE(containsDate(days, "2026-01-03"));
    TEST_EXPECT_TRUE(containsDate(days, "2026-01-04"));

    TEST_EXPECT_FALSE(containsDate(days, "2026-01-01"));
    TEST_EXPECT_FALSE(containsDate(days, "2026-01-05"));

    // Saturday remains excluded after rebuilding.
    TEST_EXPECT_EQ(dayStatus(days, "2026-01-03"), 2);
}

void testInvalidShiftPeriodDoesNotChangeBounds() {
    ExamPeriod period = makePeriod(
        Semester::FALL,
        Moed::ALEPH,
        Date(1, 1, 2026),
        Date(7, 1, 2026)
    );

    CalendarManager manager;
    manager.setData(makePeriodList({ period }), QVariantList{});

    manager.shiftPeriod("FALL", "invalid-date", "2026-01-04");

    TEST_EXPECT_EQ(manager.getCurrentStartDate(), QString("2026-01-01"));
    TEST_EXPECT_EQ(manager.getCurrentEndDate(), QString("2026-01-07"));
    TEST_EXPECT_EQ(manager.getDays().size(), 7);
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    testDefaultState();
    testSetDataBuildsDaysAndCurrentPeriodMetadata();
    testSemesterListAndPeriodTree();
    testSelectPeriodAndNavigation();
    testToggleDayChangesVisibleStatus();
    testToggleInvalidOrMissingDayDoesNotChangeCalendar();
    testSaveChangesFlushesExcludedDaysBackToPeriods();
    testShiftPeriodChangesBoundsAndRebuildsDays();
    testInvalidShiftPeriodDoesNotChangeBounds();

    std::cout << "CalendarManagerTest passed." << std::endl;
    return EXIT_SUCCESS;
}