#pragma once

#include <QObject>
#include <QDate>
#include <QVariantList>
#include <QVariantMap>
#include <vector>
#include <string>

#include "domain/ExamPeriod.h"
#include "domain/Course.h"

/**
 * @brief Describes the display state of a single calendar day.
 *
 * Status values:
 * 0 = outside exam period (gray)
 * 1 = active exam day (green)
 * 2 = excluded day (red striped)
 */
struct DayInfo {
    QDate   date;
    int     status;
    QString label;
};

/**
 * @brief QObject bridge between the exam period data and the QML calendar view.
 *
 * Exposes the list of days as a Q_PROPERTY so QML can render the calendar.
 * Provides Q_INVOKABLE methods for toggling exclusions and shifting period bounds.
 */
class CalendarManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList days READ getDays NOTIFY daysChanged)
    Q_PROPERTY(QString currentSemester READ getCurrentSemester NOTIFY semesterChanged)
    Q_PROPERTY(QString currentStartDate READ getCurrentStartDate NOTIFY semesterChanged)
    Q_PROPERTY(QString currentEndDate READ getCurrentEndDate NOTIFY semesterChanged)
    Q_PROPERTY(QVariantList semesterList READ getSemesterList NOTIFY semesterChanged)

public:

    std::vector<ExamPeriod> getPeriods() const { return periods_; }
    
    explicit CalendarManager(QObject* parent = nullptr);

    /** @brief Load exam periods and courses into the manager. */
    Q_INVOKABLE void setData(QVariantList periods,
                             QVariantList courses);

    /** @brief Returns the full list of days as QVariantList for QML. */
    QVariantList getDays() const;

    /** @brief Returns the currently displayed semester label. */
    QString getCurrentSemester() const;

    /** @brief Returns the start date of the current period as yyyy-MM-dd string. */
    QString getCurrentStartDate() const;

    /** @brief Returns the end date of the current period as yyyy-MM-dd string. */
    QString getCurrentEndDate() const;

    /** @brief Returns the list of all semester labels for the sidebar. */
    QVariantList getSemesterList() const;
   /** @brief Returns the tree structure for the sidebar as a QVariantList.
    *  Each item: { year, semester, moed, index }
    */
    Q_INVOKABLE QVariantList getPeriodTree() const;
    /**
     * @brief Toggles a day between Active (1) and Excluded (2).
     * @param dateStr The day to toggle, formatted as yyyy-MM-dd.
     */
    Q_INVOKABLE void toggleDay(const QString& dateStr);

    /**
     * @brief Shifts the exam period bounds for a given semester.
     * @param semester     Semester identifier, e.g. "FALL" or "SPRI".
     * @param newStartStr  New start date for the period (yyyy-MM-dd).
     * @param newEndStr    New end date for the period (yyyy-MM-dd).
     */
    Q_INVOKABLE void shiftPeriod(const QString& semester,
                                  const QString& newStartStr,
                                  const QString& newEndStr);

    /**
     * @brief Selects a period directly by index (used by the sidebar).
     * @param index Zero-based index into the periods list.
     */
    Q_INVOKABLE void selectPeriod(int index);

    /** @brief Navigate to the next semester in the list. */
    Q_INVOKABLE void nextSemester();

    /** @brief Navigate to the previous semester in the list. */
    Q_INVOKABLE void previousSemester();

    /** @brief Marks current state as confirmed and ready for the solver. */
    Q_INVOKABLE void saveChanges();

signals:
    /** @brief Emitted whenever the days list changes (toggle or shift). */
    void daysChanged();

    /** @brief Emitted when the active semester changes. */
    void semesterChanged();

private:
    std::vector<ExamPeriod> periods_;
    std::vector<Course>     courses_;
    std::vector<DayInfo>    days_;
    int                     currentPeriodIndex_;

    /** @brief Rebuilds the days_ list from the current period. */
    void rebuildDays();
    /** @brief Persists current days_ toggle state back into periods_[currentPeriodIndex_]. */
    void flushDaysToPeriod();
};