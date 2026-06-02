#pragma once

#include <QObject>
#include <QDate>
#include <QVariantList>
#include <QVariantMap>
#include <vector>
#include <string>

#include "../model/ExamPeriod.h"
#include "../model/Course.h"

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

public:
    explicit CalendarManager(QObject* parent = nullptr);

    /** @brief Load exam periods and courses into the manager. */
    Q_INVOKABLE void setData( QVariantList periods,
                              QVariantList courses);

    /** @brief Returns the full list of days as QVariantList for QML. */
    QVariantList getDays() const;

    /** @brief Returns the currently displayed semester label. */
    QString getCurrentSemester() const;

    /**
     * @brief Toggles a day between Active (1) and Excluded (2).
     * @param date The day to toggle.
     */
    Q_INVOKABLE void toggleDay (const QString& dateStr) ;

    /**
     * @brief Shifts the exam period bounds for a given semester.
     * @param semester  Semester identifier, e.g. "FALL" or "SPRI".
     * @param newStartStr  New start date for the period (yyyy-MM-dd).
     * @param newEndStr    New end date for the period (yyyy-MM-dd).
     */
    Q_INVOKABLE void shiftPeriod(const QString& semester,
                              const QString& newStartStr,
                              const QString& newEndStr);
    /**
     * @brief Navigate to the next semester in the list.
     */
    Q_INVOKABLE void nextSemester();

    /**
     * @brief Navigate to the previous semester in the list.
     */
    Q_INVOKABLE void previousSemester();

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
};