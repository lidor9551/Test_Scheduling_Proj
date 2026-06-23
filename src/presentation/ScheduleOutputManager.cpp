#include "presentation/ScheduleOutputManager.h"
#include "domain/DateAvailabilityPolicy.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QDate>
#include <set>
#include <QFile>
#include <QTextStream>

/*
 * Creates an empty ScheduleOutputManager.
 *
 * The current schedule index starts at 1 because the UI presents schedules
 * to the user using 1-based numbering.
 */
ScheduleOutputManager::ScheduleOutputManager(QObject* parent) 
    : QObject(parent), m_currentIndex(1) {}

/*
 * Stores generated scheduling results and related domain data.
 *
 * This method is called after the scheduling service finishes successfully.
 * After storing the data, the manager rebuilds the calendar view and notifies QML.
 */
void ScheduleOutputManager::setSchedulingData(const std::vector<ScheduleGenerationResult>& solutions,
                                              const std::vector<Course>& courses,
                                              const std::vector<ExamPeriod>& periods) {
    /*
     * Replace old output data with the newly generated data.
     */
    m_solutions = solutions;
    m_courses = courses;
    m_periods = periods;

    /*
     * Reset the UI to the first generated schedule.
     */
    m_currentIndex = 1;

    /*
     * Rebuild calendar output for the selected schedule and selected period.
     */
    updateCalendarData();

    /*
     * Notify QML that the amount of schedules and current index changed.
     */
    emit totalSchedulesCountChanged();
    emit currentScheduleIndexChanged();
}

/*
 * Returns the current 1-based schedule index.
 */
int ScheduleOutputManager::getCurrentScheduleIndex() const {
    return m_currentIndex;
}

/*
 * Returns the total number of generated schedules.
 */
int ScheduleOutputManager::getTotalSchedulesCount() const {
    return static_cast<int>(m_solutions.size());
}

/*
 * Returns all available semesters for the output filter UI.
 */
QStringList ScheduleOutputManager::getAvailableSemesters() const {
    return m_semesters;
}

/*
 * Returns all available moeds for the output filter UI.
 */
QStringList ScheduleOutputManager::getAvailableMoeds() const {
    return m_moeds;
}

/*
 * Returns the current calendar data prepared for QML.
 */
QVariantList ScheduleOutputManager::getCurrentCalendarData() const {
    return m_calendarData;
}

/*
 * Moves the output screen to the next schedule if possible.
 */
void ScheduleOutputManager::nextSchedule() {
    if (m_currentIndex < m_solutions.size()) {
        m_currentIndex++;
        emit currentScheduleIndexChanged();

        /*
         * The visible calendar depends on the selected schedule,
         * so it must be rebuilt after changing the index.
         */
        updateCalendarData();
    }
}

/*
 * Moves the output screen to the previous schedule if possible.
 */
void ScheduleOutputManager::previousSchedule() {
    if (m_currentIndex > 1) {
        m_currentIndex--;
        emit currentScheduleIndexChanged();

        /*
         * Rebuild the calendar for the newly selected schedule.
         */
        updateCalendarData();
    }
}

/*
 * Updates the active semester and moed filters.
 *
 * The calendar output is rebuilt according to the new selected period.
 */
void ScheduleOutputManager::setPeriodFilter(const QString& semester, const QString& moed) {
    m_selectedSemester = semester;
    m_selectedMoed = moed;

    qDebug() << "[MANAGER] Filter updated to -> Semester:" << m_selectedSemester << "Moed:" << m_selectedMoed;

    /*
     * Rebuild the calendar after changing the selected period filter.
     */
    updateCalendarData();
}

/*
 * Placeholder method for export action from QML.
 *
 * The actual text-file export is implemented in saveCurrentScheduleToFile().
 */
void ScheduleOutputManager::exportCurrentSchedule() {
    qDebug() << "Exporting schedule index:" << m_currentIndex;
}

/*
 * Extracts unique semester and moed values from loaded exam periods.
 *
 * The extracted values are used by QML dropdowns or filters.
 */
void ScheduleOutputManager::extractAvailableFilters() {
    std::set<QString> uniqueSemesters;
    std::set<QString> uniqueMoeds;

    /*
     * Use sets to avoid duplicate filter values.
     */
    for (const auto& period : m_periods) {
        uniqueSemesters.insert(QString::fromStdString(semesterToString(period.getSemester())));
        uniqueMoeds.insert(QString::fromStdString(moedToString(period.getMoed())));
    }

    /*
     * Convert the sets into QStringList values for QML.
     */
    m_semesters = QStringList(uniqueSemesters.begin(), uniqueSemesters.end());
    m_moeds = QStringList(uniqueMoeds.begin(), uniqueMoeds.end());

    // Set default selection to the first available option
    /*
     * Select default filter values when possible.
     */
    if (!m_semesters.isEmpty()) m_selectedSemester = m_semesters.first();
    if (!m_moeds.isEmpty()) m_selectedMoed = m_moeds.first();

    /*
     * Notify QML that available filter options changed.
     */
    emit availableSemestersChanged();
    emit availableMoedsChanged();
}

/*
 * Rebuilds the calendar data shown in the output screen.
 *
 * The method creates a full calendar grid for the selected semester/moed period
 * and marks each day as:
 * - padding cell
 * - excluded day
 * - regular day
 * - exam day
 */
void ScheduleOutputManager::updateCalendarData() {
    /*
     * If there are no courses, there is nothing meaningful to render.
     */
    if (m_courses.empty()) {
        return;
    }

    /*
     * Clear previous calendar entries before rebuilding the current view.
     */
    m_calendarData.clear();

    /*
     * If required scheduling data is missing, notify QML that the calendar
     * should be refreshed as empty.
     */
    if (m_solutions.empty() || m_periods.empty() || m_courses.empty()) {
        emit currentCalendarDataChanged();
        return;
    }

    qDebug() << "[CALENDAR] Trying to draw board for -> Sem:" << m_selectedSemester << "Moed:" << m_selectedMoed;

    // 1. Find the selected ExamPeriod based on the UI dropdowns
    /*
     * Locate the exam period that matches the selected semester and moed.
     */
    const ExamPeriod* activePeriod = nullptr;
    for (const auto& period : m_periods) {
        const QString periodSemester =
            QString::fromStdString(semesterToString(period.getSemester()));
        const QString periodMoed =
            QString::fromStdString(moedToString(period.getMoed()));

        if (periodSemester == m_selectedSemester &&
            periodMoed == m_selectedMoed) {
            activePeriod = &period;
            break;
        }
    }

    // If no active period is found, return an empty calendar
    /*
     * Without a matching period, the output screen cannot build a calendar.
     */
    if (!activePeriod) {
        qDebug() << "[CALENDAR] ERROR: Could not find matching period in m_periods!";
        emit currentCalendarDataChanged();
        return;
    }

    // 2. Define the exact boundaries of the period
    /*
     * Get the domain-level start and end dates of the active exam period.
     */
    Date start = activePeriod->getStartDate();
    Date end = activePeriod->getEndDate();
    
    // Convert to QDate to easily calculate continuous date ranges
    /*
     * Convert Date objects to QDate so Qt date operations can be used.
     */
    QDate qStart(start.getYear(), start.getMonth(), start.getDay());
    QDate qEnd(end.getYear(), end.getMonth(), end.getDay());

    // --- Calculate Grid Anchors ---
    /*
     * Calculate the first visible day in the calendar grid.
     *
     * The grid starts on Sunday, so dates before qStart may appear as padding.
     */
    int startDow = qStart.dayOfWeek() % 7; 
    QDate gridStart = qStart.addDays(-startDow);

    /*
     * Calculate the last visible day in the calendar grid.
     *
     * The grid ends on Saturday, so dates after qEnd may appear as padding.
     */
    int endDow = qEnd.dayOfWeek() % 7;
    QDate gridEnd = qEnd.addDays(6 - endDow);

    /*
     * Keep the period's allowed dates available if future output logic needs them.
     */
    std::vector<Date> allowed = activePeriod->allowedDates();
    
    // 3. Now we have the exact grid range (gridStart to gridEnd) and the list of allowed dates within the period
    /*
     * Select the currently displayed schedule solution.
     *
     * m_currentIndex is 1-based, while the vector index is 0-based.
     */
    const auto& currentSolution = m_solutions[m_currentIndex - 1];
    const auto& assignments = currentSolution.getAssignments();

    /*
     * Build one QVariantMap entry for every day in the calendar grid.
     */
    for (QDate current = gridStart; current <= gridEnd; current = current.addDays(1)) {
        QVariantMap dayData;

        // Check: Are we in the padding cells?
        /*
         * Days outside the actual exam period are padding cells.
         *
         * They are kept empty so the QML calendar keeps a full weekly grid.
         */
        if (current < qStart || current > qEnd) {
            dayData["dayText"] = "";
            dayData["isExcluded"] = false;
            dayData["hasExam"] = false;
            /**
             * Padding cells lie outside the exam period and carry no real date,
             * so the drag & drop date key is left empty.
             */
            dayData["dateKey"] = "";
            /**
             * Empty exam list: a padding cell never holds any exam. QML iterates
             * this list, so an empty list renders nothing.
             */
            dayData["exams"] = QVariantList();
        }
        else {
            // Active day inside the period
            /*
             * Convert the current QDate back into the domain Date type.
             */
            Date currentDate(current.day(), current.month(), current.year());

            /**
             * Expose the exact cell date in the "dd-MM-yyyy" format expected by
             * the drag & drop API contract, with zero-padded day and month.
             */
            dayData["dateKey"] = current.toString("dd-MM-yyyy");

            /*
             * Show month information on the first day of the month or on the
             * first visible day of the exam period.
             */
            if (current.day() == 1 || current == qStart) {
                dayData["dayText"] = QString::number(current.day()) + "/" + QString::number(current.month());
            } else {
                dayData["dayText"] = QString::number(current.day());
            }

            // Check date availability using the shared policy.
            /*
             * Use the shared date policy so output display matches scheduling rules.
             */
            const bool isAllowed =
                DateAvailabilityPolicy::isAllowedExamDate(
                    currentDate,
                    activePeriod->getExcludedRanges()
                );

            /*
             * Initialize the day as a regular day with no exam.
             */
            dayData["isExcluded"] = !isAllowed;
            dayData["hasExam"] = false;
            /**
             * Collect every exam scheduled on this date. Each entry is a
             * QVariantMap consumed by the QML calendar delegate.
             */
            QVariantList dayExams;

            // Check if there are exams on this date in the current solution
            /*
             * Search the current solution assignments for every exam scheduled
             * on the current calendar date. All matching exams are kept so a day
             * holding several exams shows them all.
             */
            for (const ExamAssignment& assignment : assignments) {
                if (assignment.examDate == currentDate) {
                    QVariantMap examEntry;
                    examEntry["examName"] = QString::fromStdString(assignment.course->getCourseName());
                    examEntry["courseId"] = QString::fromStdString(assignment.course->getCourseNumber());

                    /*
                     * Display the program name when a program mapping exists.
                     * Otherwise, fall back to the raw program ID.
                     */
                    if (!assignment.course->getPrograms().empty()) {
                        const auto& progDetails = assignment.course->getPrograms().front();
                        QString progId = QString::fromStdString(progDetails.programID);

                        if (m_programsMap.contains(progId)) {
                            examEntry["program"] = m_programsMap.value(progId);
                        } else {
                            examEntry["program"] = progId;
                        }
                    } else {
                        /*
                         * Use a generic label when the course has no program details.
                         */
                        examEntry["program"] = "כללי";
                    }

                    /*
                     * Store the requirement text for QML display.
                     */
                    examEntry["req"] = assignment.isObligatory ? "חובה" : "בחירה";

                    dayExams.append(examEntry);
                }
            }

            /*
             * A day has an exam when at least one assignment matched its date.
             */
            dayData["hasExam"] = !dayExams.isEmpty();
            dayData["exams"] = dayExams;
        }
        
        /*
         * Add the completed day entry to the QML calendar data list.
         */
        m_calendarData.append(dayData);
    }
    
    /*
     * Notify QML that the visible calendar data was rebuilt.
     */
    emit currentCalendarDataChanged();
}

/*
 * Sets the available semester and moed filters manually.
 *
 * AppController calls this after preprocessing creates scheduling blocks.
 */
void ScheduleOutputManager::setAvailablePeriods(const QStringList& semesters, const QStringList& moeds) {
    m_semesters = semesters;
    m_moeds = moeds;
    
    emit availableSemestersChanged(); 
    emit availableMoedsChanged();
}

/*
 * Clears all output data and resets the output manager state.
 *
 * This is useful when the user reloads data or starts a new workflow.
 */
void ScheduleOutputManager::clearData() {
    m_solutions.clear();
    m_courses.clear();
    m_periods.clear();
    m_calendarData.clear();
    
    m_currentIndex = 0;
    m_selectedSemester = "";
    m_selectedMoed = "";

    /*
     * Notify QML that all output-related state changed.
     */
    emit totalSchedulesCountChanged();
    emit currentScheduleIndexChanged();
    emit currentCalendarDataChanged();
    
    qDebug() << "[MANAGER] Data cleared successfully.";
}

/*
 * Checks whether the current 1-based schedule index points to a valid solution.
 */
bool ScheduleOutputManager::isCurrentScheduleIndexValid() const {
    return !m_solutions.empty() &&
           m_currentIndex >= 1 &&
           m_currentIndex <= static_cast<int>(m_solutions.size());
}

/*
 * Validates the export output path before writing the schedule file.
 *
 * The path must:
 * - not be empty
 * - not point to an existing directory
 * - have an existing parent directory
 */
bool ScheduleOutputManager::isOutputPathValid(const QString& filePath,
                                              QString* errorMessage) const {
    const QString trimmedPath = filePath.trimmed();

    /*
     * Reject empty export paths.
     */
    if (trimmedPath.isEmpty()) {
        if (errorMessage) {
            *errorMessage = "Export path is empty.";
        }
        return false;
    }

    const QFileInfo fileInfo(trimmedPath);

    /*
     * Reject paths that point to a directory instead of a file.
     */
    if (fileInfo.exists() && fileInfo.isDir()) {
        if (errorMessage) {
            *errorMessage = "Export path points to a directory, not a file.";
        }
        return false;
    }

    /*
     * The parent directory must exist before the file can be created.
     */
    const QDir parentDir = fileInfo.absoluteDir();
    if (!parentDir.exists()) {
        if (errorMessage) {
            *errorMessage = "Export directory does not exist.";
        }
        return false;
    }

    return true;
}

/*
 * Checks whether the current schedule can be exported.
 *
 * This validates that solutions exist and that the current index is valid.
 */
bool ScheduleOutputManager::canExportCurrentSchedule(QString* errorMessage) const {
    /*
     * There must be at least one generated schedule.
     */
    if (m_solutions.empty()) {
        if (errorMessage) {
            *errorMessage = "Cannot export because no generated schedules are available.";
        }
        return false;
    }

    /*
     * The selected schedule index must point to an existing solution.
     */
    if (!isCurrentScheduleIndexValid()) {
        if (errorMessage) {
            *errorMessage = "Cannot export because the current schedule index is invalid.";
        }
        return false;
    }

    return true;
}

/*
 * Saves the currently selected schedule to a text file.
 *
 * The method performs export validation, opens the file, and writes each exam
 * assignment in a readable text format.
 */
bool ScheduleOutputManager::saveCurrentScheduleToFile(const QString& filePath) {
    QString errorMessage;

    /*
     * Block export when there is no valid current schedule.
     */
    if (!canExportCurrentSchedule(&errorMessage)) {
        qWarning() << "[EXPORT] Export blocked:" << errorMessage;
        return false;
    }

    /*
     * Block export when the path is invalid.
     */
    if (!isOutputPathValid(filePath, &errorMessage)) {
        qWarning() << "[EXPORT] Export blocked:" << errorMessage;
        return false;
    }

    /*
     * Open the destination file for writing text.
     */
    QFile file(filePath.trimmed());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[EXPORT] Failed to open file for writing:" << filePath;
        return false;
    }

    QTextStream out(&file);

    /*
     * Write export header.
     */
    out << "Schedule Export - " << m_selectedSemester << " " << m_selectedMoed << "\n";
    out << "------------------------------------------\n";

    /*
     * Select the current generated solution.
     */
    const auto& currentSolution = m_solutions[m_currentIndex - 1];
    const auto& assignments = currentSolution.getAssignments();

    /*
     * Write all exam assignments in the current schedule.
     */
    for (const ExamAssignment& assignment : assignments) {
        if (!assignment.course) {
            qWarning() << "[EXPORT] Skipping assignment with null course pointer.";
            continue;
        }

        out << "Course: " << QString::fromStdString(assignment.course->getCourseName())
            << " | ID: " << QString::fromStdString(assignment.course->getCourseNumber()) << "\n";

        out << "Date: " << assignment.examDate.toString().c_str() << "\n";
        out << "Requirement: " << (assignment.isObligatory ? "Obligatory" : "Elective") << "\n";
        out << "------------------------------------------\n";
    }

    /*
     * Close the file and report success.
     */
    file.close();
    return true;
}

#include "ScheduleOutputManager.h"
#include <algorithm>

void ScheduleOutputManager::sortSchedules(const std::vector<std::string>& priorityList) {
    if (priorityList.empty() || m_solutions.empty()) {
        return; // Nothing to sort
    }

    std::sort(m_solutions.begin(), m_solutions.end(), 
        [&priorityList](const ScheduleGenerationResult& a, const ScheduleGenerationResult& b) {
            
            // Iterate through the user's priority list from most to least important
            for (const std::string& criterion : priorityList) {
                
                if (criterion == "metric_avgDaysObligatory") {
                    // Higher average gap is better
                    if (a.metrics.avgDaysBetweenObligatory != b.metrics.avgDaysBetweenObligatory) {
                        return a.metrics.avgDaysBetweenObligatory > b.metrics.avgDaysBetweenObligatory;
                    }
                } 
                else if (criterion == "metric_avgDaysAll") {
                    // Higher average gap is better
                    if (a.metrics.avgDaysBetweenAll != b.metrics.avgDaysBetweenAll) {
                        return a.metrics.avgDaysBetweenAll > b.metrics.avgDaysBetweenAll;
                    }
                } 
                else if (criterion == "metric_electiveConflicts") {
                    // Lower number of conflicts is better
                    if (a.metrics.totalElectiveConflicts != b.metrics.totalElectiveConflicts) {
                        return a.metrics.totalElectiveConflicts < b.metrics.totalElectiveConflicts;
                    }
                } 
                else if (criterion == "metric_obligatorySpan") {
                    // Larger span generally means mandatory exams are more spread out (less crammed)
                    if (a.metrics.obligatorySpan != b.metrics.obligatorySpan) {
                        return a.metrics.obligatorySpan > b.metrics.obligatorySpan; 
                    }
                } 
                else if (criterion == "metric_maxExamsPerDay") {
                    // Lower maximum exams per day is better (less load on the institution)
                    if (a.metrics.maxExamsInSingleDay != b.metrics.maxExamsInSingleDay) {
                        return a.metrics.maxExamsInSingleDay < b.metrics.maxExamsInSingleDay;
                    }
                }
            }
            
            // If all metrics in the priority list are identical, maintain relative order
            return false; 
        });

    // Refresh UI
    // Reset the view to the first schedule (the new "best" schedule)
    m_currentIndex = 1; 
    
    // Rebuild the calendar data points based on the newly sorted top schedule
    updateCalendarData(); 
    
    // Notify QML bindings to react and redraw
    emit currentScheduleIndexChanged();
    emit currentCalendarDataChanged();

}

/**
 * @brief Evaluates all displayed calendar dates to determine if a specific course can be moved there.
 * * This function simulates the removal of the requested course from the current schedule
 * and tests every available date against the system's Hard Constraints. It is designed 
 * to be called continuously by the QML frontend during a drag-and-drop "hover" state 
 * without modifying the actual underlying schedule memory.
 * * @param courseId The unique identifier of the course being dragged (e.g., "83101").
 * @return QVariantMap A map where keys are date strings ("dd-MM-yyyy") and values are booleans 
 * (true if the date is a valid drop target, false otherwise).
 */
QVariantMap ScheduleOutputManager::getValidDatesForCourse(const QString& courseId) {
    QVariantMap resultMap;
    
    // Ensure we have a valid schedule loaded before attempting any simulations
    if (!isCurrentScheduleIndexValid()) {
        return resultMap;
    }

    // 1. Create an isolated COPY of the current schedule.
    // m_currentIndex is 1-based for the UI, so we subtract 1 to get the correct vector index.
    ScheduleGenerationResult tempSchedule = m_solutions[m_currentIndex - 1];

    // 2. Temporarily extract the dragged exam from this isolated copy.
    // This ensures the algorithm doesn't conflict with the exam's own current placement.
    tempSchedule.removeAssignment(courseId.toStdString());

    // 3. Iterate through all date cells currently rendered on the UI calendar.
    for (const QVariant& cellVar : m_calendarData) {
        QVariantMap cellData = cellVar.toMap();
        QString dateKey = cellData["dateKey"].toString();
        
        // Skip empty padding cells that fall outside the actual exam period
        if (dateKey.isEmpty()) {
            continue; 
        }

        bool isValid = false;

        // 4. Perform foundational constraint checks.
        // Globally excluded days (e.g., Shabbat, national holidays) are strictly invalid.
        if (cellData["isExcluded"].toBool() == true) {
            isValid = false;
        } else {
            // 5. Run advanced Hard Constraints Validation.
            // TODO (Next Task): Integrate your specific ConstraintsValidator here.
            // Example implementation:
            // isValid = ConstraintsValidator::checkHardConstraints(tempSchedule, courseId, dateKey);
            
            // Temporary placeholder: Assume valid unless it is an excluded day, 
            // allowing the UI drag-and-drop mechanics to be tested.
            isValid = true; 
        }

        // Map the result to the corresponding date string
        resultMap[dateKey] = isValid;
    }

    return resultMap;
}

/**
 * @brief Attempts to permanently relocate an exam to a new target date in the schedule's memory.
 * * This function is triggered by the QML frontend upon dropping an exam card. It performs a 
 * secondary, secure validation check on the backend to prevent illegal moves. If the target 
 * date is valid, it parses the date string, updates the actual schedule memory, and prepares 
 * the system for a UI recalculation.
 * * @param courseId The unique identifier of the course being moved.
 * @param newDate The target date formatted as a string ("dd-MM-yyyy").
 * @return QVariantMap A status dictionary containing {"status": 1} on success or {"status": 0} on failure.
 */
QVariantMap ScheduleOutputManager::requestMove(const QString& courseId, const QString& newDate) {
    QVariantMap result;
    result["status"] = 0; // Default initialization to failure state

    // Abort if the schedule index is invalid or the target date string is empty
    if (!isCurrentScheduleIndexValid() || newDate.isEmpty()) {
        return result;
    }

    // 1. Secure Backend Validation.
    // Re-verify the requested target date to prevent illegal moves initiated by UI glitches.
    QVariantMap validDates = getValidDatesForCourse(courseId);
    
    // Check if the exact requested date exists in the valid map and is flagged as true
    if (validDates.contains(newDate) && validDates[newDate].toBool() == true) {
        
        // 2. Date Parsing.
        // Extract day, month, and year from the "dd-MM-yyyy" string format.
        QStringList dateParts = newDate.split("-");
        if (dateParts.size() == 3) {
            // Convert strings to integers and construct the domain Date object
            Date targetDate(dateParts[0].toInt(), dateParts[1].toInt(), dateParts[2].toInt());
            
            // 3. Memory Update.
            // Access the actual schedule instance by reference and apply the new date.
            ScheduleGenerationResult& realSchedule = m_solutions[m_currentIndex - 1];
            realSchedule.updateAssignmentDate(courseId.toStdString(), targetDate);
            
            // Mark the operation as a complete success
            result["status"] = 1; 
            qDebug() << "[Drag&Drop] Successfully moved course" << courseId << "to" << newDate;
            
            // 4. Finalization (Story 1.3 Preparation).
            // TODO: Uncomment these lines in Story 1.3 to trigger metrics recalculation 
            // and notify the QML layer to redraw the updated calendar.
            // updateCalendarData();
            // emit currentCalendarDataChanged();
        }
    } else {
        // Log the rejection for debugging purposes
        qDebug() << "[Drag&Drop] Rejected move for course" << courseId << "to invalid date:" << newDate;
    }

    return result;
}