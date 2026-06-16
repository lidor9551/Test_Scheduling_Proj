#include "presentation/AppController.h"
#include "infrastructure/InputParser.h"
#include "scheduling/Preprocessor.h"
#include <QFileInfo>
#include <QUrl>
#include <QDir>
#include <string>
#include <utility>
#include <QVariant>
#include <QDebug>

/*
 * Constructs the main application controller.
 *
 * The controller connects the asynchronous scheduling service signals
 * to local slots so the UI can be updated when generation succeeds or fails.
 */
AppController::AppController(QObject* parent)
    : QObject(parent) {
    
    // Connect the signals from the scheduling service to the appropriate slots in the controller
    /*
     * When the scheduling service finishes successfully, the controller receives
     * the generated solutions through onSchedulingFinished().
     */
    connect(&m_schedulingService, &SchedulingService::generationFinished,
            this, &AppController::onSchedulingFinished);
            
    /*
     * When the scheduling service fails, the controller receives the error message
     * through onSchedulingFailed().
     */
    connect(&m_schedulingService, &SchedulingService::generationFailed,
            this, &AppController::onSchedulingFailed);
}

/*
 * Returns the currently selected courses file path.
 */
QString AppController::coursesFilePath() const {
    return coursesFilePath_;
}

/*
 * Returns the currently selected exam periods file path.
 */
QString AppController::examPeriodsFilePath() const {
    return examPeriodsFilePath_;
}

/*
 * Returns the current status message shown in the UI.
 */
QString AppController::statusMessage() const {
    return statusMessage_;
}

/*
 * Returns the current error message shown in the UI.
 */
QString AppController::errorMessage() const {
    return errorMessage_;
}

/*
 * Returns the number of courses currently stored in the scheduling session.
 */
int AppController::courseCount() const {
    return session_.courseCount();
}

/*
 * Returns the number of exam periods currently stored in the scheduling session.
 */
int AppController::examPeriodCount() const {
    return session_.examPeriodCount();
}

/*
 * Returns true if the scheduling session contains loaded data.
 */
bool AppController::hasData() const {
    return session_.hasData();
}

/*
 * Stores the selected courses file path.
 *
 * The path is normalized so it can be used safely by the parser on the local
 * operating system.
 */
void AppController::setCoursesFilePath(const QString& path) {
    coursesFilePath_ = normalizePath(path);
    emit coursesFilePathChanged();
}

/*
 * Stores the selected exam periods file path.
 *
 * The path is normalized before being stored.
 */
void AppController::setExamPeriodsFilePath(const QString& path) {
    examPeriodsFilePath_ = normalizePath(path);
    emit examPeriodsFilePathChanged();
}

/*
 * Returns the model used by QML to display courses by program.
 */
ProgramCourseModel* AppController::programCourseModel() {
    return &programCourseModel_;
}

/*
 * Replaces all existing loaded data with data parsed from the selected files.
 *
 * This is used when the user wants to start from a clean dataset instead of
 * appending to the current session.
 */
void AppController::replaceData() {
    clearMessages();

    /*
     * Do not parse files unless both file paths are valid.
     */
    if (!validateSelectedFiles()) {
        return;
    }

    try {
        InputParser parser;

        /*
         * Parse courses from the selected courses file.
         */
        std::vector<Course> loadedCourses =
            parser.parseCourses(coursesFilePath_.toStdString());

        /*
         * Parse exam periods from the selected periods file.
         */
        std::vector<ExamPeriod> loadedPeriods =
            parser.parseExamPeriods(examPeriodsFilePath_.toStdString());

        /*
         * Replace the session data with the newly loaded data.
         *
         * std::move avoids unnecessary copying of the parsed vectors.
         */
        session_.replaceData(std::move(loadedCourses), std::move(loadedPeriods));

        /*
         * Synchronize presentation helpers with the new session data.
         */
        programCourseModel_.setCourses(session_.courses());
        m_outputManager.setCourses(session_.courses());
        m_outputManager.setProgramsMap(getInternalProgramsMap());

        /*
         * Notify QML that the visible data changed.
         */
        emit dataChanged();

        /*
         * Show a success message with the amount of loaded data.
         */
        setStatus(
            QString("Data replaced successfully. Loaded %1 courses and %2 exam periods.")
                .arg(courseCount())
                .arg(examPeriodCount())
        );
    } catch (const std::exception& ex) {
        /*
         * Convert parsing or validation errors into a user-facing message.
         */
        setError(QString("Failed to replace data: %1").arg(ex.what()));
    }
}

/*
 * Appends parsed data to the existing scheduling session.
 *
 * Duplicate courses or periods are skipped by SchedulingSession.
 */
void AppController::appendData() {
    clearMessages();

    /*
     * The append operation also requires both files to be selected and valid.
     */
    if (!validateSelectedFiles()) {
        return;
    }

    try {
        InputParser parser;

        /*
         * Parse additional courses.
         */
        std::vector<Course> loadedCourses =
            parser.parseCourses(coursesFilePath_.toStdString());

        /*
         * Parse additional exam periods.
         */
        std::vector<ExamPeriod> loadedPeriods =
            parser.parseExamPeriods(examPeriodsFilePath_.toStdString());

        /*
         * Append data into the existing session.
         *
         * The result contains counts for added and skipped duplicate entries.
         */
        SchedulingSession::AppendResult result =
            session_.appendData(loadedCourses, loadedPeriods);

        /*
         * Refresh models and output helpers after the session changed.
         */
        programCourseModel_.setCourses(session_.courses());
        m_outputManager.setCourses(session_.courses());
        m_outputManager.setProgramsMap(getInternalProgramsMap());

        /*
         * Notify QML that the loaded data changed.
         */
        emit dataChanged();

        /*
         * Display a detailed append summary to the user.
         */
        setStatus(
            QString("Append completed. Added %1 courses, skipped %2 duplicate courses. Added %3 periods, skipped %4 duplicate periods.")
                .arg(result.addedCourses)
                .arg(result.skippedDuplicateCourses)
                .arg(result.addedExamPeriods)
                .arg(result.skippedDuplicateExamPeriods)
        );
    } catch (const std::exception& ex) {
        /*
         * Convert parser/session exceptions into a visible error message.
         */
        setError(QString("Failed to append data: %1").arg(ex.what()));
    }
}

/*
 * Clears both status and error messages.
 *
 * Signals are emitted only when a message actually changed.
 */
void AppController::clearMessages() {
    if (!statusMessage_.isEmpty()) {
        statusMessage_.clear();
        emit statusMessageChanged();
    }

    if (!errorMessage_.isEmpty()) {
        errorMessage_.clear();
        emit errorMessageChanged();
    }
}

/*
 * Converts a raw path received from QML into a local native file-system path.
 *
 * QML file dialogs may return file URLs, so local file URLs are converted
 * into regular paths before being used by C++ file APIs.
 */
QString AppController::normalizePath(const QString& rawPath) const {
    if (rawPath.isEmpty()) {
        return {};
    }

    const QUrl url(rawPath);

    if (url.isLocalFile()) {
        return QDir::toNativeSeparators(url.toLocalFile());
    }

    return QDir::toNativeSeparators(rawPath);
}

/*
 * Validates that both required input files were selected and exist on disk.
 *
 * Returns false and sets a user-facing error message when validation fails.
 */
bool AppController::validateSelectedFiles() {
    if (coursesFilePath_.isEmpty()) {
        setError("Courses file was not selected.");
        return false;
    }

    if (examPeriodsFilePath_.isEmpty()) {
        setError("Exam periods file was not selected.");
        return false;
    }

    if (!QFileInfo::exists(coursesFilePath_)) {
        setError(QString("Courses file does not exist: %1").arg(coursesFilePath_));
        return false;
    }

    if (!QFileInfo::exists(examPeriodsFilePath_)) {
        setError(QString("Exam periods file does not exist: %1").arg(examPeriodsFilePath_));
        return false;
    }

    return true;
}

/*
 * Updates the status message and notifies QML.
 */
void AppController::setStatus(const QString& message) {
    statusMessage_ = message;
    emit statusMessageChanged();
}

/*
 * Updates the error message and notifies QML.
 */
void AppController::setError(const QString& message) {
    errorMessage_ = message;
    emit errorMessageChanged();
}

/*
 * Converts the loaded courses into a QVariantList for QML.
 *
 * Each Course object is wrapped with QVariant::fromValue().
 */
QVariantList AppController::getCoursesVariant() const {
    QVariantList list;

    for (const Course& c : session_.courses()) {
        list.append(QVariant::fromValue(c));
    }

    return list;
}

/*
 * Converts the loaded exam periods into a QVariantList for QML.
 *
 * Each ExamPeriod object is wrapped with QVariant::fromValue().
 */
QVariantList AppController::getExamPeriodsVariant() const {
    QVariantList list;

    for (const ExamPeriod& p : session_.examPeriods()) {
        list.append(QVariant::fromValue(p));
    }

    return list;
}

/*
 * Returns the list of available academic program IDs.
 *
 * If no custom program list was loaded into m_availablePrograms,
 * the method returns the default hardcoded list used by the application.
 */
QStringList AppController::availablePrograms() const {
    if (m_availablePrograms.isEmpty()) {
        return QStringList{"83101", "83102", "83103", "83104", "83105", "83107", "83108", "83109", "83115", "83182"};
    }
    return m_availablePrograms;
}

/*
 * Returns the currently selected program IDs as a QStringList for QML.
 *
 * The SchedulingSession stores selected programs as std::string values,
 * so each ID is converted to QString before being returned to the UI.
 */
QStringList AppController::selectedPrograms() const {
    QStringList result;

    for (const std::string& programId : session_.selectedPrograms()) {
        result.append(QString::fromStdString(programId));
    }

    return result;
}

/*
 * Toggles the selection state of an academic program.
 *
 * If the program is already selected, it is removed.
 * If it is not selected, the controller tries to add it.
 *
 * The SchedulingSession enforces the maximum number of selected programs.
 */
void AppController::toggleProgram(const QString& programId) {
    const std::string id = programId.toStdString();

    bool changed = false;

    /*
     * Remove the program if it is already selected.
     */
    if (session_.isProgramSelected(id)) {
        changed = session_.deselectProgram(id);
    } else {
        /*
         * Try to select the program.
         *
         * This may fail if the maximum allowed number of selected programs
         * has already been reached.
         */
        changed = session_.selectProgram(id);

        if (!changed) {
            qDebug() << "Cannot select more than"
                     << static_cast<int>(SchedulingSession::MaxSelectedPrograms)
                     << "programs.";
        }
    }

    /*
     * Notify QML only when the selected program list actually changed.
     */
    if (changed) {
        emit selectedProgramsChanged();
    }
}

/*
 * Returns courses that belong to a specific academic program.
 *
 * Optional filters:
 * - year == -1 means no year filtering.
 * - semester == -1 means no semester filtering.
 *
 * The returned data is formatted as QVariantMap objects so QML can display it
 * easily without knowing the internal Course structure.
 */
QVariantList AppController::getCoursesForProgram(const QString& programId, int year, int semester) {
    QVariantList courseList;
    std::string pid = programId.toStdString();

    /*
     * Scan all loaded courses in the current scheduling session.
     */
    for (const auto& course : session_.courses()) {
        const auto& programs = course.getPrograms();
        
        /*
         * A course may belong to multiple programs, years, and semesters.
         * Each program assignment is checked separately.
         */
        for (const auto& p : programs) {
            // basic filtering by program ID
            bool match = (p.programID == pid);
            
            // if year or semester are specified (not -1), also filter by them
            /*
             * Apply optional year and semester filters only when requested.
             */
            if (year != -1) match &= (yearToInt(p.year) == year);
            if (semester != -1) match &= (static_cast<int>(p.semester) == semester);

            if (match) {
                // convert Course to QVariant and add to list
                /*
                 * Create a lightweight map for QML display.
                 *
                 * Only the fields needed by the UI are included here.
                 */
                QVariantMap courseData;
                courseData["name"] = QString::fromStdString(course.getCourseName());

                // course requirement (obligatory/elective)
                /*
                 * Convert the requirement enum into a Hebrew display string.
                 */
                courseData["req"] = (p.requirement == Requirement::OBLIGATORY) ? "חובה" : "בחירה";

                // evaluation method (exam/project/attendance)
                /*
                 * Convert the evaluation enum into a Hebrew display string.
                 */
                Evaluation eval = course.getEvaluationMethod();
                QString evalText;
                if (eval == Evaluation::EXAM) evalText = "מבחן";
                else if (eval == Evaluation::PROJECT) evalText = "פרויקט";
                else evalText = "נוכחות";
                courseData["eval"] = evalText;

                /*
                 * Add the formatted course entry to the list returned to QML.
                 */
                courseList.append(courseData);
            }
        }
    }
    return courseList;
}

/*
 * Returns the output manager exposed to QML.
 *
 * The output manager is responsible for preparing generated schedules
 * for display and export.
 */
ScheduleOutputManager* AppController::outputManager() {
    return &m_outputManager;
}

/*
 * Starts the full schedule generation flow.
 *
 * This method is called from QML after the user selected programs and loaded
 * the required input files.
 *
 * The flow includes:
 * - clearing previous messages
 * - syncing edited exam periods from CalendarManager
 * - validating the scheduling session
 * - filtering exam-only courses
 * - building scheduling blocks
 * - exposing available periods to the output manager
 * - starting generation for the first available block
 */
void AppController::generateSchedules() {
    clearMessages();
    qDebug() << "=========================================";
    qDebug() << ">>> generateSchedules STARTED! <<<";
    qDebug() << "Selected programs count:" << session_.selectedProgramCount();

    // override original output with new instance to reset previous state
    /*
     * Before generating schedules, synchronize possible calendar edits.
     *
     * If the user edited exam periods through the CalendarManager,
     * those changes must replace the original parsed exam periods in the session.
     */
    if (m_calendarManager && !m_calendarManager->getPeriods().empty()) {
        m_calendarManager->saveChanges();
        session_.replaceExamPeriods(m_calendarManager->getPeriods());
        qDebug() << "[SYNC] Successfully synced updated periods from CalendarManager.";
    } else {
        qDebug() << "[SYNC] CalendarManager is empty or not ready. Using original parsed periods.";
    }

    /*
     * Validate that the session is ready for schedule generation.
     *
     * Validation checks conditions such as loaded data and selected programs.
     */
    SchedulingSession::ValidationResult validation =
        session_.validateBeforeGeneration();

    if (!validation.isValid()) {
        setError(QString::fromStdString(validation.message()));
        qDebug() << "[VALIDATION] Schedule generation blocked:"
                << QString::fromStdString(validation.message());
        return;
    }

    /*
     * Inform the UI that generation has started.
     */
    setStatus("Generating schedules...");

    // Keep a reference to the full course list for preprocessing
    /*
     * The preprocessor receives the full course list, while the output manager
     * later receives only courses that actually require exams.
     */
    const std::vector<Course>& allCourses = session_.courses();
    const std::vector<ExamPeriod>& examPeriods = session_.examPeriods();

    // Create a filtered list of courses that require exams
    /*
     * Only courses evaluated by exams should participate in the scheduling output.
     *
     * Project and attendance courses do not need exam dates.
     */
    std::vector<Course> examOnlyCourses;
    for (const auto& course : allCourses) {
        if (course.getEvaluationMethod() == Evaluation::EXAM) {
            examOnlyCourses.push_back(course);
        }
    }

    /*
     * Debug output helps verify which courses are passed into the scheduling flow.
     */
    qDebug() << ">>> Courses exposed to algorithm:";
    for (const auto& course : examOnlyCourses) {
        qDebug() << "    -" << QString::fromStdString(course.getCourseName());
    }

    // Expose the exam-only courses to the output manager  
    /*
     * The output manager uses this list to display generated schedules in the UI.
     */
    m_outputManager.setCourses(examOnlyCourses);

    // Build the scheduling blocks based on the selected programs
    /*
     * Convert selected programs from the session into a std::vector.
     *
     * The preprocessor expects standard strings.
     */
    std::vector<std::string> selectedProgs;
    for (const std::string& prog : session_.selectedPrograms()) {
        selectedProgs.push_back(prog);
    }

    // Build all blocks and save them to the class member (m_allBlocks)
    /*
     * Build scheduling blocks from courses, exam periods, and selected programs.
     *
     * Each block usually represents a specific semester and moed combination.
     */
    SchedulingPreprocessor preprocessor(allCourses, examPeriods, selectedProgs);
    m_allBlocks = preprocessor.buildBlocks();

    /*
     * Stop generation if preprocessing did not create any schedulable blocks.
     */
    if (m_allBlocks.empty()) {
        setError("No scheduling blocks were created for the selected programs and exam periods.");
        qDebug() << "[VALIDATION] Schedule generation blocked: no scheduling blocks were created.";
        return;
    }

    // Extract available semesters and moedim from the blocks to populate the UI dropdowns
    /*
     * Collect available semesters and moed values from the generated blocks.
     *
     * These values are later used by the UI to let the user switch between periods.
     */
    QStringList availableSemesters;
    QStringList availableMoeds;
    
    for (const auto& block : m_allBlocks) {
        QString sem = QString::fromStdString(block.semester);
        QString moed = QString::fromStdString(block.moed);
        
        if (!availableSemesters.contains(sem)) {
            availableSemesters.append(sem);
        }
        if (!availableMoeds.contains(moed)) {
            availableMoeds.append(moed);
        }
    }

    // Expose the available periods to the output manager for UI dropdowns
    /*
     * Store the available periods in the output manager so QML can display them.
     */
    m_outputManager.setAvailablePeriods(availableSemesters, availableMoeds);

    // Trigger the algorithm automatically for the first valid block (default view)
    /*
     * Use the first available scheduling block as the default generated view.
     */
    QString firstSem = QString::fromStdString(m_allBlocks[0].semester);
    QString firstMoed = QString::fromStdString(m_allBlocks[0].moed);
    
    // This function will now handle the Thread and Worker creation via the Service!
    /*
     * Start generation for the first semester/moed pair.
     *
     * The actual asynchronous execution is delegated to SchedulingService.
     */
    generateForPeriod(firstSem, firstMoed);
}

// This slot will be called when the scheduling service emits the generationFinished signal with the results
/*
 * Handles successful asynchronous schedule generation.
 *
 * The results are transferred to the output manager so the UI can display them.
 */
void AppController::onSchedulingFinished(const std::vector<ScheduleGenerationResult>& solutions) {
    qDebug() << "--- FINAL ALGORITHM OUTPUT ---";
    qDebug() << "Total solutions calculated:" << solutions.size();
    setStatus("Scheduling completed!");

    // Filter the courses to only those that require exams, as the scheduling is based on exam periods
    /*
     * Rebuild the exam-only course list for the output manager.
     *
     * This keeps the displayed output aligned with the scheduling algorithm.
     */
    std::vector<Course> examOnlyCourses;
    for (const auto& course : session_.courses()) {
        if (course.getEvaluationMethod() == Evaluation::EXAM) {
            examOnlyCourses.push_back(course);
        }
    }

    // Expose the solutions to the output manager so it can prepare the data for the UI
    /*
     * Pass the generated solutions, relevant courses, and exam periods to the
     * output manager for formatting and presentation.
     */
    m_outputManager.setSchedulingData(solutions, examOnlyCourses, session_.examPeriods());
}

/*
 * Handles asynchronous schedule generation failure.
 *
 * The error is converted into a user-facing message.
 */
void AppController::onSchedulingFailed(QString message) {
    setError("Scheduling failed: " + message);
}

/*
 * Generates schedules for a specific semester and moed.
 *
 * This method searches for the matching scheduling block and asks
 * SchedulingService to run the algorithm asynchronously for that block.
 */
void AppController::generateForPeriod(const QString& semester, const QString& moed) {
    std::string targetSem = semester.toStdString();
    std::string targetMoed = moed.toStdString();

    SchedulingBlock* selectedBlock = nullptr;
    
    // search for the block that matches the selected semester and moed
    /*
     * Find the block that matches the requested semester and moed.
     */
    for (auto& block : m_allBlocks) {
        if (block.semester == targetSem && block.moed == targetMoed) {
            selectedBlock = &block;
            break;
        }
    }

    /*
     * If the requested block does not exist, clear the output data and stop.
     */
    if (!selectedBlock) {
        qDebug() << "No block found for" << semester << moed;
        m_outputManager.setSchedulingData({}, session_.courses(), session_.examPeriods());
        return;
    }

    /*
     * Notify the UI which period is currently being generated.
     */
    setStatus("Generating schedules for " + semester + " " + moed + "...");

    //update the output manager with the current period filter so it can prepare the UI accordingly
    /*
     * Store the selected period filter in the output manager.
     *
     * This allows the UI to show results for the currently selected semester/moed.
     */
    m_outputManager.setPeriodFilter(semester, moed);

    // Now we call the scheduling service to start the generation process for the selected block
    /*
     * Start asynchronous generation for the selected block.
     *
     * The second parameter limits the number of generated solutions.
     */
    m_schedulingService.startAsyncGeneration(*selectedBlock, 100000); 
}

// C++ function to create the internal map of program IDs to names
/*
 * Creates the internal program-name map used by C++ components.
 *
 * The key is the academic program ID.
 * The value is the Hebrew display name shown in the UI.
 *
 * This map is used mainly by the output manager and by QML-facing conversion
 * functions that need readable program names instead of numeric IDs.
 */
QMap<QString, QString> AppController::getInternalProgramsMap() const {
    QMap<QString, QString> map;

    /*
     * Program ID to display-name mapping.
     *
     * Keep these values synchronized with the valid program IDs used by
     * the parser and scheduling session.
     */
    map["83101"] = QStringLiteral("הנדסת מחשבים");
    map["83102"] = QStringLiteral("הנדסת חשמל");
    map["83104"] = QStringLiteral("הנדסת תעשיה ומערכות מידע");
    map["83107"] = QStringLiteral("הנדסת נתונים");
    map["83108"] = QStringLiteral("הנדסת תוכנה");
    map["83109"] = QStringLiteral("הנדסת חומרים");
    map["83105"] = QStringLiteral("הנדסת מחשבים – מגמת חומרת מחשבים");
    map["83182"] = QStringLiteral("הנדסת חשמל – מגמת הנדסה קוונטית");
    map["83103"] = QStringLiteral("הנדסת חשמל – מגמת נוירו הנדסה");

    /*
     * Keep the existing 83115 value exactly as it appears in your file.
     */
    map["83115"] = QStringLiteral("הנדסת חשמל – מגמת הנדסה ביו-רפואית");
    /*
     * Return the completed map to the caller.
     */
    return map;
}

/*
 * Converts the internal program-name map into a QVariantMap for QML.
 *
 * QML works naturally with QVariantMap, so this function adapts the C++ QMap
 * without exposing internal implementation details to the UI layer.
 */
QVariantMap AppController::getProgramNamesMap() const {
    QVariantMap variantMap;

    /*
     * Reuse the internal C++ map as the single source of truth.
     */
    QMap<QString, QString> internalMap = getInternalProgramsMap();

    /*
     * Copy every program ID and display name into the QVariantMap.
     */
    for (auto it = internalMap.begin(); it != internalMap.end(); ++it) {
        variantMap[it.key()] = it.value();
    }

    return variantMap;
}

/**
 * Stores the enabled state and threshold k for each of the five hard constraints.
 *
 * This is pure in-memory storage. No engine logic is triggered and no data is
 * written to disk. Integration with the scheduling algorithm is deferred to a
 * future subtask.
 */
void AppController::saveHardConstraints(bool r21Enabled, int r21K,
                                         bool r22Enabled, int r22K,
                                         bool r23Enabled, int r23K,
                                         bool r24Enabled, int r24K,
                                         bool r25Enabled, int r25K) {
    m_rule21Enabled = r21Enabled;
    m_rule21K       = r21K;

    m_rule22Enabled = r22Enabled;
    m_rule22K       = r22K;

    m_rule23Enabled = r23Enabled;
    m_rule23K       = r23K;

    m_rule24Enabled = r24Enabled;
    m_rule24K       = r24K;

    m_rule25Enabled = r25Enabled;
    m_rule25K       = r25K;

    qDebug() << "[HardConstraints] Saved:"
             << "r21=" << m_rule21Enabled << "k=" << m_rule21K
             << "r22=" << m_rule22Enabled << "k=" << m_rule22K
             << "r23=" << m_rule23Enabled << "k=" << m_rule23K
             << "r24=" << m_rule24Enabled << "k=" << m_rule24K
             << "r25=" << m_rule25Enabled << "k=" << m_rule25K;
}

/**
 * Returns the stored hard constraint settings as a QVariantMap for QML.
 *
 * Keys match the QML property names defined in SettingsScreen.qml exactly,
 * allowing Component.onCompleted to copy values directly into local properties.
 */
QVariantMap AppController::getHardConstraints() const {
    QVariantMap map;

    map["rule21Enabled"] = m_rule21Enabled;
    map["rule21K"]       = m_rule21K;

    map["rule22Enabled"] = m_rule22Enabled;
    map["rule22K"]       = m_rule22K;

    map["rule23Enabled"] = m_rule23Enabled;
    map["rule23K"]       = m_rule23K;

    map["rule24Enabled"] = m_rule24Enabled;
    map["rule24K"]       = m_rule24K;

    map["rule25Enabled"] = m_rule25Enabled;
    map["rule25K"]       = m_rule25K;

    return map;
}