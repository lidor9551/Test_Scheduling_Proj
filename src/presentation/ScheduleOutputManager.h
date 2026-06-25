#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <vector>
#include <QMap>
#include <functional>

#include "domain/Course.h"
#include "domain/ExamPeriod.h"
#include "domain/ScheduleGenerationResult.h"

/*
 * ScheduleOutputManager belongs to the presentation layer.
 *
 * It prepares generated schedules for the QML output screen.
 * The manager keeps the current schedule index, available semester/moed filters,
 * calendar data for display, and export-related functionality.
 */
class ScheduleOutputManager : public QObject {
    Q_OBJECT

    // Expose variables to QML (The visual contract)
    /*
     * Current schedule number shown in the output screen.
     *
     * This is 1-based, so the first schedule is index 1.
     */
    Q_PROPERTY(int currentScheduleIndex READ getCurrentScheduleIndex NOTIFY currentScheduleIndexChanged)

    /*
     * Total number of generated schedules currently available.
     */
    Q_PROPERTY(int totalSchedulesCount READ getTotalSchedulesCount NOTIFY totalSchedulesCountChanged)

    /*
     * Available semester values used by the QML filter controls.
     */
    Q_PROPERTY(QStringList availableSemesters READ getAvailableSemesters NOTIFY availableSemestersChanged)

    /*
     * Available moed values used by the QML filter controls.
     */
    Q_PROPERTY(QStringList availableMoeds READ getAvailableMoeds NOTIFY availableMoedsChanged)

    /*
     * Calendar-style data for the currently selected schedule and period.
     */
    Q_PROPERTY(QVariantList currentCalendarData READ getCurrentCalendarData NOTIFY currentCalendarDataChanged)

    /*
     * Current schedule metrics exposed to QML.
     *
     * These values are recalculated after a successful drag-and-drop move.
     */
    Q_PROPERTY(QVariantMap currentMetrics READ getCurrentMetrics NOTIFY currentMetricsChanged)

public:
    /*
     * Creates an empty output manager.
     */
    explicit ScheduleOutputManager(QObject* parent = nullptr);

    // Setters
    /*
     * Loads scheduling results and related domain data into the manager.
     *
     * This is called after the scheduling algorithm finishes.
     */
    void setSchedulingData(const std::vector<ScheduleGenerationResult>& solutions,
                           const std::vector<Course>& courses,
                           const std::vector<ExamPeriod>& periods);

    // Getters
    /*
     * Returns the currently selected schedule index.
     */
    int getCurrentScheduleIndex() const;

    /*
     * Returns the number of generated schedules.
     */
    int getTotalSchedulesCount() const;

    /*
     * Returns the currently stored generated schedules.
     */
    const std::vector<ScheduleGenerationResult>& getSolutions() const;

    /*
     * Returns available semester filter values.
     */
    QStringList getAvailableSemesters() const;

    /*
     * Returns available moed filter values.
     */
    QStringList getAvailableMoeds() const;

    /*
     * Returns the calendar data currently displayed by QML.
     */
    QVariantList getCurrentCalendarData() const;

    /*
     * Returns the metrics of the currently selected schedule for QML display.
     */
    QVariantMap getCurrentMetrics() const;
    
    /*
     * Stores the courses used for output display.
     */
    void setCourses(const std::vector<Course>& courses) {
        m_courses = courses;
    }

    /*
     * Sets the available period filters manually.
     */
    void setAvailablePeriods(const QStringList& semesters, const QStringList& moeds);

    /*
     * Stores a program-ID to program-name map for display purposes.
     */
    void setProgramsMap(const QMap<QString, QString>& map) {
        m_programsMap = map;
    }

    /**
     * @brief Sorts the internal list of generated schedules dynamically.
     * @param priorityList A list of criteria ordered by user priority (index 0 is most important).
     */
    void sortSchedules(const std::vector<std::string>& priorityList);

    // set the schedule Metrics
    void setMetricsUpdater(std::function<ScheduleMetrics(const ScheduleGenerationResult&)> updater) {
        m_metricsUpdater = updater;
    }

    /**
     * @brief Defines the signature for the validation callback.
     * * This callback bridges the gap between the UI layer and the Domain rules.
     * It takes the proposed schedule, the course ID being moved, and the target date.
     * * @return true if the move strictly obeys scheduling constraints, false otherwise.
     */
    using ValidatorCallback = std::function<bool(const ScheduleGenerationResult&, const std::string&, const Date&)>;
    
    /**
     * @brief Registers the validation callback to be used during drag-and-drop operations.
     * * By injecting this validator (typically from the Controller), we ensure the Manager 
     * remains decoupled from the complex Conflict Rules engine.
     * * @param validator The callback function wrapping the rules engine.
     */
    void setMoveValidator(ValidatorCallback validator) { 
        m_moveValidator = validator; 
    }

    // Actions triggered by the user from the QML interface
    /*
     * Moves to the next generated schedule.
     */
    Q_INVOKABLE void nextSchedule();

    /*
     * Moves to the previous generated schedule.
     */
    Q_INVOKABLE void previousSchedule();

    /*
     * Changes the active semester/moed filter.
     */
    Q_INVOKABLE void setPeriodFilter(const QString& semester, const QString& moed);

    /*
     * Placeholder action for exporting the current schedule.
     */
    Q_INVOKABLE void exportCurrentSchedule();

    /*
     * Clears all scheduling output data.
     */
    Q_INVOKABLE void clearData();
    
    //to save and export the current schedule as txt file
    /*
     * Saves the currently selected schedule to a text file.
     *
     * Returns true when export succeeds and false when validation or writing fails.
     */
    Q_INVOKABLE bool saveCurrentScheduleToFile(const QString& filePath);

    /*
     * Returns a map of dates and their validity status (true/false) for a specific course,
     * based strictly on Hard Constraints validation.
     */
    Q_INVOKABLE QVariantMap getValidDatesForCourse(const QString& courseId);

    /*
     * Attempts to move a course to a new date. 
     * Returns a map with "status": 1 on success, or 0 on failure.
     */
    Q_INVOKABLE QVariantMap requestMove(const QString& courseId, const QString& newDate);

signals:
    /*
     * Emitted when the current schedule index changes.
     */
    void currentScheduleIndexChanged();

    /*
     * Emitted when the total schedule count changes.
     */
    void totalSchedulesCountChanged();

    /*
     * Emitted when available semester filters change.
     */
    void availableSemestersChanged();

    /*
     * Emitted when available moed filters change.
     */
    void availableMoedsChanged();

    /*
     * Emitted when calendar data should be refreshed in QML.
     */
    void currentCalendarDataChanged();

    /*
     * Emitted when current schedule metrics were recalculated or changed.
     */
    void currentMetricsChanged();

    /*
     * Emitted when the stored schedule results were modified in-place.
     */
    void solutionsChanged();

private:
    /*
     * Checks whether there is a valid current schedule that can be exported.
     */
    bool canExportCurrentSchedule(QString* errorMessage = nullptr) const;

    /*
     * Checks whether m_currentIndex points to an existing solution.
     */
    bool isCurrentScheduleIndexValid() const;

    /*
     * Validates the export path before writing a file.
     */
    bool isOutputPathValid(const QString& filePath, QString* errorMessage = nullptr) const;
    
    // Builds the calendar UI data for the specific month/period    
    /*
     * Rebuilds m_calendarData according to the selected schedule and period filter.
     */
    void updateCalendarData(); 

    // Extracts unique semesters and moeds from the ExamPeriod data
    /*
     * Extracts available semester and moed values from the loaded exam periods.
     */
    void extractAvailableFilters(); 

    // Map to convert programID to programName for display purposes
    /*
     * Maps academic program IDs to readable Hebrew program names.
     */
    QMap<QString, QString> m_programsMap;

    // The core data structures holding the scheduling results and related info
    /*
     * Generated schedule solutions returned from the scheduling algorithm.
     */
    std::vector<ScheduleGenerationResult> m_solutions;
    
    /*
     * Courses relevant to output display.
     */
    std::vector<Course> m_courses;

    /*
     * Exam periods relevant to the generated output.
     */
    std::vector<ExamPeriod> m_periods;

    // get the schedule and return the updated metrics
    std::function<ScheduleMetrics(const ScheduleGenerationResult&)> m_metricsUpdater;

    // Current state of the output screen
    /*
     * Current schedule index shown in the UI.
     *
     * This value is 1-based.
     */
    int m_currentIndex = 1;

    /**
     * @brief Stores the registered validation logic injected by the external Controller.
     */
    ValidatorCallback m_moveValidator;

    /*
     * Currently selected semester filter.
     */
    QString m_selectedSemester;

    /*
     * Currently selected moed filter.
     */
    QString m_selectedMoed;

    /*
     * Available semester filter values.
     */
    QStringList m_semesters;

    /*
     * Available moed filter values.
     */
    QStringList m_moeds;

    /*
     * Calendar entries prepared for the QML calendar output view.
     */
    QVariantList m_calendarData;

};
