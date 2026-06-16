#pragma once

#include <QObject>
#include <QString>
#include <vector>
#include "domain/Course.h"
#include "domain/ExamPeriod.h"
#include "presentation/ProgramCourseModel.h"
#include <QVariant>
#include <QVariantList>
#include "presentation/ScheduleOutputManager.h"
#include "scheduling/Preprocessor.h"
#include "presentation/CalendarManager.h"
#include "application/SchedulingSession.h"
#include "domain/ScheduleGenerationResult.h"
#include "scheduling/SchedulingService.h"

/*
 * AppController is the main presentation-layer controller.
 *
 * It connects the QML user interface with the application logic, scheduling
 * session, parser, calendar manager, scheduling service, and output manager.
 *
 * Most functions are exposed to QML through Q_PROPERTY or Q_INVOKABLE.
 */
class AppController : public QObject {
    Q_OBJECT

    /*
     * File path properties exposed to QML.
     *
     * QML can read these values and react when the selected files change.
     */
    Q_PROPERTY(QString coursesFilePath READ coursesFilePath NOTIFY coursesFilePathChanged)
    Q_PROPERTY(QString examPeriodsFilePath READ examPeriodsFilePath NOTIFY examPeriodsFilePathChanged)

    /*
     * User-facing status and error messages.
     *
     * These are used by the UI to display success, progress, and failure messages.
     */
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

    /*
     * Summary information about the currently loaded scheduling data.
     */
    Q_PROPERTY(int courseCount READ courseCount NOTIFY dataChanged)
    Q_PROPERTY(int examPeriodCount READ examPeriodCount NOTIFY dataChanged)
    Q_PROPERTY(bool hasData READ hasData NOTIFY dataChanged)

    /*
     * Full loaded data exposed to QML as QVariantList objects.
     *
     * These lists allow QML to display courses and exam periods.
     */
    Q_PROPERTY(QVariantList courses READ getCoursesVariant NOTIFY dataChanged)
    Q_PROPERTY(QVariantList examPeriods READ getExamPeriodsVariant NOTIFY dataChanged)

    // For the program course model
    /*
     * List of all academic programs that can be selected in the UI.
     */
    Q_PROPERTY(QStringList availablePrograms READ availablePrograms CONSTANT)

    // only selected program IDs, not the full course data, since the model will handle that dynamically
    /*
     * List of currently selected program IDs.
     *
     * The program course model uses these IDs to filter and display relevant courses.
     */
    Q_PROPERTY(QStringList selectedPrograms READ selectedPrograms NOTIFY selectedProgramsChanged)

    /*
     * Exposes the model that supplies program-course data to QML views.
     */
    Q_PROPERTY(ProgramCourseModel* programCourseModel READ programCourseModel CONSTANT)

    // the output manager instance to be used across the app and exposed to QML
    /*
     * Exposes the output manager to QML.
     *
     * The output manager prepares generated schedules for display and export.
     */
    Q_PROPERTY(ScheduleOutputManager* outputManager READ outputManager CONSTANT)

public:
    /*
     * Creates the controller and connects internal scheduling-service signals.
     */
    explicit AppController(QObject* parent = nullptr);

    /*
     * Converts loaded courses into QVariantList for QML consumption.
     */
    QVariantList getCoursesVariant() const;

    /*
     * Converts loaded exam periods into QVariantList for QML consumption.
     */
    QVariantList getExamPeriodsVariant() const;

    /*
     * Returns the selected courses input file path.
     */
    QString coursesFilePath() const;

    /*
     * Returns the selected exam periods input file path.
     */
    QString examPeriodsFilePath() const;

    /*
     * Returns the latest status message.
     */
    QString statusMessage() const;

    /*
     * Returns the latest error message.
     */
    QString errorMessage() const;

    /*
     * Returns the number of loaded courses.
     */
    int courseCount() const;

    /*
     * Returns the number of loaded exam periods.
     */
    int examPeriodCount() const;

    /*
     * Returns true when the session contains loaded data.
     */
    bool hasData() const;

    /*
     * Receives and stores the courses file path from QML.
     */
    Q_INVOKABLE void setCoursesFilePath(const QString& path);

    /*
     * Receives and stores the exam periods file path from QML.
     */
    Q_INVOKABLE void setExamPeriodsFilePath(const QString& path);

    /*
     * Replaces the current session data with newly parsed input files.
     */
    Q_INVOKABLE void replaceData();

    /*
     * Appends newly parsed data to the existing session data.
     */
    Q_INVOKABLE void appendData();

    /*
     * Clears current status and error messages.
     */
    Q_INVOKABLE void clearMessages();

    /*
     * Returns the available program IDs shown in the UI.
     */
    QStringList availablePrograms() const;

    /*
     * Returns the currently selected program IDs.
     */
    QStringList selectedPrograms() const;

    // to present in the UI the relevant courses for the selected program
    /*
     * Returns courses that match the selected program, year, and semester filters.
     *
     * The year and semester parameters are optional.
     * A value of -1 means the filter should not be applied.
     */
    Q_INVOKABLE QVariantList getCoursesForProgram(const QString& programId, int year = -1, int semester = -1);

    /*
     * Starts schedule generation for a specific semester and moed.
     */
    Q_INVOKABLE void generateForPeriod(const QString& semester, const QString& moed);

    //qml checboxes will call this to toggle program selection
    /*
     * Toggles a program selection from QML.
     *
     * If the program is already selected, it is removed.
     * Otherwise, the controller attempts to add it to the selected programs.
     */
    Q_INVOKABLE void toggleProgram(const QString& programId);

    /*
     * Returns the program-course model instance exposed to QML.
     */
    ProgramCourseModel* programCourseModel();
    
    // getter for the output manager
    /*
     * Returns the output manager instance exposed to QML.
     */
    ScheduleOutputManager* outputManager();

    // the function called from QML to start the scheduling algorithm
    /*
     * Starts the full scheduling generation workflow.
     *
     * This includes validation, preprocessing, block creation, and asynchronous
     * scheduling through SchedulingService.
     */
    Q_INVOKABLE void generateSchedules();

    /**
     * Stores the enabled flag and threshold k for each of the five hard constraints.
     *
     * Called from SettingsScreen.qml when the user presses "Save".
     * Pure in-memory storage — no engine logic, no QSettings.
     * Integration with the scheduler is deferred to a future subtask.
     */
    Q_INVOKABLE void saveHardConstraints(bool r21Enabled, int r21K,
                                         bool r22Enabled, int r22K,
                                         bool r23Enabled, int r23K,
                                         bool r24Enabled, int r24K,
                                         bool r25Enabled, int r25K);

    /**
     * Returns the stored hard constraint settings as a QVariantMap for QML.
     *
     * Keys match the QML property names defined in SettingsScreen.qml exactly,
     * allowing Component.onCompleted to copy values directly into local properties.
     */
    Q_INVOKABLE QVariantMap getHardConstraints() const;

    // to set the calendar manager instance for the output manager to use
    /*
     * Stores the CalendarManager instance created in main.cpp.
     *
     * The controller uses it to synchronize edited exam periods before generation.
     */
    void setCalendarManager(CalendarManager* cm) { m_calendarManager = cm; }

public slots:
    /*
     * Slot called when asynchronous schedule generation completes successfully.
     */
    void onSchedulingFinished(const std::vector<ScheduleGenerationResult>& solutions);

    /*
     * Slot called when asynchronous schedule generation fails.
     */
    void onSchedulingFailed(QString message);

signals:
    /*
     * Emitted when the courses file path changes.
     */
    void coursesFilePathChanged();

    /*
     * Emitted when the exam periods file path changes.
     */
    void examPeriodsFilePathChanged();

    /*
     * Emitted when the status message changes.
     */
    void statusMessageChanged();

    /*
     * Emitted when the error message changes.
     */
    void errorMessageChanged();

    /*
     * Emitted when loaded data changes.
     */
    void dataChanged();

    /*
     * Emitted when the selected program list changes.
     */
    void selectedProgramsChanged();

private:
    /*
     * Converts a raw path or file URL into a native local file-system path.
     */
    QString normalizePath(const QString& rawPath) const;

    /*
     * Validates that both selected input files exist before parsing.
     */
    bool validateSelectedFiles();

    /*
     * Updates the status message and notifies QML.
     */
    void setStatus(const QString& message);

    /*
     * Updates the error message and notifies QML.
     */
    void setError(const QString& message);
    
    /*
     * Model used by QML to display courses grouped by selected programs.
     */
    ProgramCourseModel programCourseModel_;

    /*
     * Holds the currently loaded courses, exam periods, and selected programs.
     */
    SchedulingSession session_;

    // output manager instance to be used across the app and exposed to QML
    /*
     * Manages generated scheduling output for UI display and export.
     */
    ScheduleOutputManager m_outputManager;
    
    /*
     * Service responsible for running schedule generation asynchronously.
     */
    SchedulingService m_schedulingService;

    //for the QML to get the program names map for display purposes
    /*
     * Returns a QVariantMap that maps program IDs to display names for QML.
     */
    Q_INVOKABLE QVariantMap getProgramNamesMap() const;

    // for the C++
    /*
     * Returns the internal Qt map of program IDs to Hebrew program names.
     */
    QMap<QString, QString> getInternalProgramsMap() const;

private:
    /*
     * Path to the selected courses file.
     */
    QString coursesFilePath_;

    /*
     * Path to the selected exam periods file.
     */
    QString examPeriodsFilePath_;

    /*
     * Last status message shown to the user.
     */
    QString statusMessage_;

    /*
     * Last error message shown to the user.
     */
    QString errorMessage_;

    /*
     * Scheduling blocks created during preprocessing.
     *
     * These blocks are reused when the user chooses a specific semester and moed.
     */
    std::vector<SchedulingBlock> m_allBlocks;

    /*
     * Available academic program IDs.
     */
    QStringList m_availablePrograms;

    /*
     * Calendar manager used to synchronize edited exam periods before generation.
     *
     * This pointer is set from main.cpp after both objects are created.
     */
    CalendarManager* m_calendarManager = nullptr;

    /**
     * Hard constraint storage — five enabled flags and five threshold values.
     *
     * Set by saveHardConstraints() when the user saves the Settings screen.
     * Read by the scheduling engine in a future subtask.
     */
    bool m_rule21Enabled = false;
    int  m_rule21K       = 1;

    bool m_rule22Enabled = false;
    int  m_rule22K       = 1;

    bool m_rule23Enabled = false;
    int  m_rule23K       = 1;

    bool m_rule24Enabled = false;
    int  m_rule24K       = 1;

    bool m_rule25Enabled = false;
    int  m_rule25K       = 1;
};