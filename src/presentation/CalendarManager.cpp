#include "CalendarManager.h"
#include <QDebug>
#include "domain/DateAvailabilityPolicy.h"

/*
 * Creates a CalendarManager instance.
 *
 * The first exam period is selected by default once data is loaded.
 */
CalendarManager::CalendarManager(QObject* parent)
    : QObject(parent), currentPeriodIndex_(0) {}

/*
 * Loads exam periods and courses into the calendar manager.
 *
 * The data arrives as QVariantList because it is passed through the Qt/QML layer.
 * Only values that can be converted back to the expected C++ domain types are used.
 */
void CalendarManager::setData(QVariantList periods, QVariantList courses)
{
    /*
     * Clear old data before loading the new dataset.
     */
    periods_.clear();
    courses_.clear();

    /*
     * Convert QVariant values into ExamPeriod objects.
     */
    for (const QVariant& v : periods) {
        if (v.canConvert<ExamPeriod>())
            periods_.push_back(v.value<ExamPeriod>());
    }

    /*
     * Convert QVariant values into Course objects.
     */
    for (const QVariant& v : courses) {
        if (v.canConvert<Course>())
            courses_.push_back(v.value<Course>());
    }

    /*
     * Start from the first period and rebuild the calendar view.
     */
    currentPeriodIndex_ = 0;
    rebuildDays();

    /*
     * Notify QML that both the selected period and day list changed.
     */
    emit semesterChanged();
    emit daysChanged();
}

/*
 * Converts internal DayInfo objects into QVariantMap entries for QML.
 */
QVariantList CalendarManager::getDays() const {
    QVariantList result;

    /*
     * Each calendar day is exposed as a map with fields used by the QML view.
     */
    for (const DayInfo& d : days_) {
        QVariantMap map;
        map["date"]   = d.date;
        map["status"] = d.status;
        map["label"]  = d.label;
        result.append(map);
    }
    return result;
}

/*
 * Returns a display label for the currently selected exam period.
 */
QString CalendarManager::getCurrentSemester() const {
    if (periods_.empty()) return "";

    const ExamPeriod& period = periods_[currentPeriodIndex_];

    return QString::fromStdString(
        semesterToString(period.getSemester())
        + " - "
        + moedToString(period.getMoed())
    );
}

/*
 * Returns the start date of the selected period as a QML-friendly string.
 */
QString CalendarManager::getCurrentStartDate() const {
    if (periods_.empty()) return "";
    const Date& d = periods_[currentPeriodIndex_].getStartDate();
    return QDate(d.getYear(), d.getMonth(), d.getDay()).toString("yyyy-MM-dd");
}

/*
 * Returns the end date of the selected period as a QML-friendly string.
 */
QString CalendarManager::getCurrentEndDate() const {
    if (periods_.empty()) return "";
    const Date& d = periods_[currentPeriodIndex_].getEndDate();
    return QDate(d.getYear(), d.getMonth(), d.getDay()).toString("yyyy-MM-dd");
}

/*
 * Returns a flat list of all available exam period labels.
 */
QVariantList CalendarManager::getSemesterList() const {
    QVariantList list;

    /*
     * Each label combines semester and moed, for example: FALL - Aleph.
     */
    for (const ExamPeriod& p : periods_) {
        list.append(QString::fromStdString(
            semesterToString(p.getSemester())
            + " - "
            + moedToString(p.getMoed())
        ));
    }
    return list;
}

/*
 * Returns a structured list of periods for the sidebar.
 *
 * Each entry contains:
 * - year
 * - semester
 * - moed
 * - index
 */
QVariantList CalendarManager::getPeriodTree() const {
    QVariantList tree;

    /*
     * The index is included so QML can select the exact period later.
     */
    for (int i = 0; i < (int)periods_.size(); i++) {
        const ExamPeriod& p = periods_[i];
        QVariantMap entry;
        entry["year"]     = p.getStartDate().getYear();
        entry["semester"] = QString::fromStdString(semesterToString(p.getSemester()));
        entry["moed"]     = QString::fromStdString(moedToString(p.getMoed()));
        entry["index"]    = i;
        tree.append(entry);
    }
    return tree;
}

/*
 * Toggles a calendar day between active and excluded.
 *
 * This method is usually called when the user clicks a day in the QML calendar.
 */
void CalendarManager::toggleDay(const QString& dateStr) {
    /*
     * Convert the QML date string into a QDate.
     */
    QDate targetDate = QDate::fromString(dateStr, "yyyy-MM-dd");
    if (!targetDate.isValid()) {
        qDebug() << ">>> INVALID DATE:" << dateStr;
        return;
    }

    bool found = false;

    /*
     * Find the matching day and switch its status.
     *
     * 1 becomes 2, and any non-1 value becomes 1.
     */
    for (DayInfo& d : days_) {
        if (d.date == targetDate) {
            found = true;
            d.status = (d.status == 1) ? 2 : 1;
            break;
        }
    }

    qDebug() << ">>> Date found and modified in C++:" << found;

    /*
     * Notify QML to redraw the calendar.
     */
    emit daysChanged();
}

/*
 * Updates the date range of the currently selected exam period.
 *
 * The semester parameter is currently unused because the selected period is
 * determined by currentPeriodIndex_.
 */
void CalendarManager::shiftPeriod(const QString& semester, const QString& newStartStr,const QString& newEndStr){

    Q_UNUSED(semester);
    if (periods_.empty()) return;

    /*
     * Parse the new date bounds from QML strings.
     */
    QDate newStart = QDate::fromString(newStartStr, "yyyy-MM-dd");
    QDate newEnd   = QDate::fromString(newEndStr,   "yyyy-MM-dd");

    if (!newStart.isValid() || !newEnd.isValid()) {
        qDebug() << ">>> shiftPeriod: invalid dates" << newStartStr << newEndStr;
        return;
    }

    /*
     * Rebuild the selected ExamPeriod with the new bounds while preserving
     * semester, moed, and current exclusions.
     */
    ExamPeriod& p = periods_[currentPeriodIndex_];
    Date start(newStart.day(), newStart.month(), newStart.year());
    Date end(newEnd.day(),     newEnd.month(),   newEnd.year());
    p = ExamPeriod(p.getSemester(), p.getMoed(), start, end, p.getExcludedRanges());

    /*
     * Recreate the visible day list according to the updated bounds.
     */
    rebuildDays();
    emit semesterChanged();
    emit daysChanged();
}

/*
 * Selects a specific period by index.
 *
 * Before switching, current day exclusions are flushed back into the old period.
 */
void CalendarManager::selectPeriod(int index) {
    if (index < 0 || index >= (int)periods_.size()) return;

    /*
     * Save current edits before moving to another period.
     */
    flushDaysToPeriod();

    currentPeriodIndex_ = index;
    rebuildDays();

    /*
     * Notify QML that both metadata and day data changed.
     */
    emit semesterChanged();
    emit daysChanged();
}

/*
 * Moves to the next period if possible.
 */
void CalendarManager::nextSemester() {
    if (currentPeriodIndex_ < (int)periods_.size() - 1) {
        /*
         * Persist current edits before navigating.
         */
        flushDaysToPeriod();

        currentPeriodIndex_++;
        rebuildDays();

        emit daysChanged();
        emit semesterChanged();
    }
}

/*
 * Moves to the previous period if possible.
 */
void CalendarManager::previousSemester() {
    if (currentPeriodIndex_ > 0) {
        /*
         * Persist current edits before navigating.
         */
        flushDaysToPeriod();

        currentPeriodIndex_--;
        rebuildDays();

        emit daysChanged();
        emit semesterChanged();
    }
}

/*
 * Saves the current calendar edits into the selected ExamPeriod.
 */
void CalendarManager::saveChanges() {
    flushDaysToPeriod();
    qDebug() << ">>> Changes saved for period:" << getCurrentSemester();
    emit daysChanged();
}

/*
 * Writes the current day status list back into the selected ExamPeriod.
 *
 * Every day with status 2 becomes an excluded single-day range.
 */
void CalendarManager::flushDaysToPeriod() {
    if (periods_.empty()) return;

    ExamPeriod& p = periods_[currentPeriodIndex_];
    std::vector<ExcludedRange> newExclusions;

    /*
     * Convert excluded calendar days into ExcludedRange domain objects.
     */
    for (const DayInfo& d : days_) {
        if (d.status == 2) {
            Date dt(d.date.day(), d.date.month(), d.date.year());
            ExcludedRange r;
            r.start  = dt;
            r.end    = dt;
            r.reason = "manual";
            newExclusions.push_back(r);
        }
    }

    /*
     * Replace the current ExamPeriod with a new one that contains the updated
     * exclusion list.
     */
    p = ExamPeriod(p.getSemester(), p.getMoed(),
                   p.getStartDate(), p.getEndDate(),
                   newExclusions);
}

/*
 * Rebuilds the visible calendar day list for the selected exam period.
 *
 * Each date between the period start and end is checked against
 * DateAvailabilityPolicy to decide whether it is active or excluded.
 */
void CalendarManager::rebuildDays() {
    days_.clear();
    if (periods_.empty()) return;

    const ExamPeriod& period = periods_[currentPeriodIndex_];
    Date start = period.getStartDate();
    Date end   = period.getEndDate();

    /*
     * Create one DayInfo object for every date in the selected period.
     */
    for (Date current = start; current <= end; current = current.nextDay()) {
        DayInfo info;
        info.date  = QDate(current.getYear(), current.getMonth(), current.getDay());
        info.label = "";

        /*
         * The date availability policy checks excluded ranges and other
         * domain rules for whether the day may be used for an exam.
         */
        const bool isAllowed =
            DateAvailabilityPolicy::isAllowedExamDate(
                current,
                period.getExcludedRanges()
            );

        /*
         * Status 1 means active day, status 2 means excluded day.
         */
        info.status = isAllowed ? 1 : 2;
        days_.push_back(info);
    }
}