#pragma once

#include <QObject>
#include <QDate>
#include <QVariantList>
#include <QVariantMap>
#include <vector>
#include <string>

#include "domain/ExamPeriod.h"
#include "domain/Course.h"

/*
 * CalendarManager is part of the presentation layer.
 *
 * It acts as the bridge between the ExamPeriod domain objects and the QML
 * calendar screen. The class converts C++ exam-period data into QVariant-based
 * structures that QML can display and modify.
 */

/**
 * @brief Describes the display state of a single calendar day.
 *
 * Status values:
 * 0 = outside exam period (gray)
 * 1 = active exam day (green)
 * 2 = excluded day (red striped)
 */
struct DayInfo {
    /*
     * The actual calendar date represented by this item.
     */
    QDate   date;

    /*
     * Visual and logical status of the day.
     *
     * In the current implementation:
     * 1 means the day is available for exams.
     * 2 means the day is excluded.
     */
    int     status;

    /*
     * Optional text label shown in the calendar UI.
     */
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

    /*
     * Calendar day list exposed to QML.
     *
     * Each day is returned as a QVariantMap containing date, status, and label.
     */
    Q_PROPERTY(QVariantList days READ getDays NOTIFY daysChanged)

    /*
     * Current selected exam period information exposed to QML.
     */
    Q_PROPERTY(QString currentSemester READ getCurrentSemester NOTIFY semesterChanged)
    Q_PROPERTY(QString currentStartDate READ getCurrentStartDate NOTIFY semesterChanged)
    Q_PROPERTY(QString currentEndDate READ getCurrentEndDate NOTIFY semesterChanged)

    /*
     * List of available semester/moed labels for navigation in QML.
     */
    Q_PROPERTY(QVariantList semesterList READ getSemesterList NOTIFY semesterChanged)

public:

    /*
     * Returns the current exam periods after possible calendar edits.
     *
     * AppController uses this method before schedule generation to sync
     * user changes back into the scheduling session.
     */
    std::vector<ExamPeriod> getPeriods() const { return periods_; }
    
    /*
     * Creates an empty calendar manager with the first period selected by default.
     */
    explicit CalendarManager(QObject* parent = nullptr);

    /** @brief Load exam periods and courses into the manager. */
    /*
     * Loads periods and courses from QVariantList objects received from QML or
     * another presentation component.
     */
    Q_INVOKABLE void setData(QVariantList periods,
                             QVariantList courses);

    /** @brief Returns the full list of days as QVariantList for QML. */
    /*
     * Converts the internal DayInfo list into a QVariantList that QML can render.
     */
    QVariantList getDays() const;

    /** @brief Returns the currently displayed semester label. */
    /*
     * Returns the current period label in the format "SEMESTER - MOED".
     */
    QString getCurrentSemester() const;

    /** @brief Returns the start date of the current period as yyyy-MM-dd string. */
    /*
     * Returns the start date of the selected exam period for date-picker binding.
     */
    QString getCurrentStartDate() const;

    /** @brief Returns the end date of the current period as yyyy-MM-dd string. */
    /*
     * Returns the end date of the selected exam period for date-picker binding.
     */
    QString getCurrentEndDate() const;

    /** @brief Returns the list of all semester labels for the sidebar. */
    /*
     * Returns a flat list of all period labels.
     */
    QVariantList getSemesterList() const;

   /** @brief Returns the tree structure for the sidebar as a QVariantList.
    *  Each item: { year, semester, moed, index }
    */
    /*
     * Returns a structured list used by the sidebar to group periods by year,
     * semester, moed, and internal index.
     */
    Q_INVOKABLE QVariantList getPeriodTree() const;

    /**
     * @brief Toggles a day between Active (1) and Excluded (2).
     * @param dateStr The day to toggle, formatted as yyyy-MM-dd.
     */
    /*
     * Called from QML when the user clicks a calendar day.
     */
    Q_INVOKABLE void toggleDay(const QString& dateStr);

    /**
     * @brief Shifts the exam period bounds for a given semester.
     * @param semester     Semester identifier, e.g. "FALL" or "SPRI".
     * @param newStartStr  New start date for the period (yyyy-MM-dd).
     * @param newEndStr    New end date for the period (yyyy-MM-dd).
     */
    /*
     * Updates the start and end dates of the currently selected period.
     */
    Q_INVOKABLE void shiftPeriod(const QString& semester,
                                  const QString& newStartStr,
                                  const QString& newEndStr);

    /**
     * @brief Selects a period directly by index (used by the sidebar).
     * @param index Zero-based index into the periods list.
     */
    /*
     * Switches the active period and rebuilds the calendar days for that period.
     */
    Q_INVOKABLE void selectPeriod(int index);

    /** @brief Navigate to the next semester in the list. */
    /*
     * Moves to the next exam period if one exists.
     */
    Q_INVOKABLE void nextSemester();

    /** @brief Navigate to the previous semester in the list. */
    /*
     * Moves to the previous exam period if one exists.
     */
    Q_INVOKABLE void previousSemester();

    /** @brief Marks current state as confirmed and ready for the solver. */
    /*
     * Persists current calendar toggles into the selected ExamPeriod.
     */
    Q_INVOKABLE void saveChanges();

signals:
    /** @brief Emitted whenever the days list changes (toggle or shift). */
    /*
     * Notifies QML that the calendar day list should be refreshed.
     */
    void daysChanged();

    /** @brief Emitted when the active semester changes. */
    /*
     * Notifies QML that the selected period or its displayed metadata changed.
     */
    void semesterChanged();

private:
    /*
     * Loaded exam periods managed by the calendar.
     */
    std::vector<ExamPeriod> periods_;

    /*
     * Loaded courses.
     *
     * The current implementation stores them for future calendar-related usage.
     */
    std::vector<Course>     courses_;

    /*
     * Current list of days displayed for the selected exam period.
     */
    std::vector<DayInfo>    days_;

    /*
     * Index of the currently selected exam period.
     */
    int                     currentPeriodIndex_;

    /** @brief Rebuilds the days_ list from the current period. */
    /*
     * Creates DayInfo entries for every date inside the selected exam period.
     */
    void rebuildDays();

    /** @brief Persists current days_ toggle state back into periods_[currentPeriodIndex_]. */
    /*
     * Converts day statuses back into excluded ranges inside the selected ExamPeriod.
     */
    void flushDaysToPeriod();
};