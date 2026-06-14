#include "presentation/ScheduleOutputManager.h"
#include "domain/DateAvailabilityPolicy.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QDate>
#include <set>
#include <QFile>
#include <QTextStream>

ScheduleOutputManager::ScheduleOutputManager(QObject* parent) 
    : QObject(parent), m_currentIndex(1) {}

void ScheduleOutputManager::setSchedulingData(const std::vector<ScheduleGenerationResult>& solutions,
                                              const std::vector<Course>& courses,
                                              const std::vector<ExamPeriod>& periods) {
    m_solutions = solutions;
    m_courses = courses;
    m_periods = periods;
    m_currentIndex = 1;

    updateCalendarData();

    emit totalSchedulesCountChanged();
    emit currentScheduleIndexChanged();
}

int ScheduleOutputManager::getCurrentScheduleIndex() const {
    return m_currentIndex;
}

int ScheduleOutputManager::getTotalSchedulesCount() const {
    return static_cast<int>(m_solutions.size());
}

QStringList ScheduleOutputManager::getAvailableSemesters() const {
    return m_semesters;
}

QStringList ScheduleOutputManager::getAvailableMoeds() const {
    return m_moeds;
}

QVariantList ScheduleOutputManager::getCurrentCalendarData() const {
    return m_calendarData;
}

void ScheduleOutputManager::nextSchedule() {
    if (m_currentIndex < m_solutions.size()) {
        m_currentIndex++;
        emit currentScheduleIndexChanged();
        updateCalendarData();
    }
}

void ScheduleOutputManager::previousSchedule() {
    if (m_currentIndex > 1) {
        m_currentIndex--;
        emit currentScheduleIndexChanged();
        updateCalendarData();
    }
}

void ScheduleOutputManager::setPeriodFilter(const QString& semester, const QString& moed) {
    m_selectedSemester = semester;
    m_selectedMoed = moed;

    qDebug() << "[MANAGER] Filter updated to -> Semester:" << m_selectedSemester << "Moed:" << m_selectedMoed;
    updateCalendarData();
}

void ScheduleOutputManager::exportCurrentSchedule() {
    qDebug() << "Exporting schedule index:" << m_currentIndex;
}

void ScheduleOutputManager::extractAvailableFilters() {
    std::set<QString> uniqueSemesters;
    std::set<QString> uniqueMoeds;

    for (const auto& period : m_periods) {
        uniqueSemesters.insert(QString::fromStdString(semesterToString(period.getSemester())));
        uniqueMoeds.insert(QString::fromStdString(moedToString(period.getMoed())));
    }

    m_semesters = QStringList(uniqueSemesters.begin(), uniqueSemesters.end());
    m_moeds = QStringList(uniqueMoeds.begin(), uniqueMoeds.end());

    // Set default selection to the first available option
    if (!m_semesters.isEmpty()) m_selectedSemester = m_semesters.first();
    if (!m_moeds.isEmpty()) m_selectedMoed = m_moeds.first();

    emit availableSemestersChanged();
    emit availableMoedsChanged();
}

void ScheduleOutputManager::updateCalendarData() {
    if (m_courses.empty()) {
        return;
    }

    m_calendarData.clear();

    if (m_solutions.empty() || m_periods.empty() || m_courses.empty()) {
        emit currentCalendarDataChanged();
        return;
    }

    qDebug() << "[CALENDAR] Trying to draw board for -> Sem:" << m_selectedSemester << "Moed:" << m_selectedMoed;

    // 1. Find the selected ExamPeriod based on the UI dropdowns
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
    if (!activePeriod) {
        qDebug() << "[CALENDAR] ERROR: Could not find matching period in m_periods!";
        emit currentCalendarDataChanged();
        return;
    }

    // 2. Define the exact boundaries of the period
    Date start = activePeriod->getStartDate();
    Date end = activePeriod->getEndDate();
    
    // Convert to QDate to easily calculate continuous date ranges
    QDate qStart(start.getYear(), start.getMonth(), start.getDay());
    QDate qEnd(end.getYear(), end.getMonth(), end.getDay());

    // --- Calculate Grid Anchors ---
    int startDow = qStart.dayOfWeek() % 7; 
    QDate gridStart = qStart.addDays(-startDow);

    int endDow = qEnd.dayOfWeek() % 7;
    QDate gridEnd = qEnd.addDays(6 - endDow);

    std::vector<Date> allowed = activePeriod->allowedDates();
    
    // 3. Now we have the exact grid range (gridStart to gridEnd) and the list of allowed dates within the period
    const auto& currentSolution = m_solutions[m_currentIndex - 1];
    const auto& assignments = currentSolution.getAssignments();

    for (QDate current = gridStart; current <= gridEnd; current = current.addDays(1)) {
        QVariantMap dayData;

        // Check: Are we in the padding cells?
        if (current < qStart || current > qEnd) {
            dayData["dayText"] = ""; 
            dayData["isExcluded"] = false;
            dayData["hasExam"] = false;
            dayData["examName"] = "";
            dayData["courseId"] = "";
            dayData["req"] = "";
            dayData["program"] = "";
        } 
        else {
            // Active day inside the period
            Date currentDate(current.day(), current.month(), current.year());

            if (current.day() == 1 || current == qStart) {
                dayData["dayText"] = QString::number(current.day()) + "/" + QString::number(current.month());
            } else {
                dayData["dayText"] = QString::number(current.day());
            }

            // Check date availability using the shared policy.
            const bool isAllowed =
                DateAvailabilityPolicy::isAllowedExamDate(
                    currentDate,
                    activePeriod->getExcludedRanges()
                );

            dayData["isExcluded"] = !isAllowed;
            dayData["hasExam"] = false;
            dayData["examName"] = "";
            dayData["courseId"] = "";
            dayData["req"] = "";
            dayData["program"] = "";

            // Check if there's an exam on this date in the current solution
            for (const ExamAssignment& assignment : assignments) {
                if (assignment.examDate == currentDate) {
                    dayData["hasExam"] = true;
                    dayData["examName"] = QString::fromStdString(assignment.course->getCourseName()); 
                    dayData["courseId"] = QString::fromStdString(assignment.course->getCourseNumber()); 
                    
                    if (!assignment.course->getPrograms().empty()) {
                        const auto& progDetails = assignment.course->getPrograms().front();
                        QString progId = QString::fromStdString(progDetails.programID);

                        if (m_programsMap.contains(progId)) {
                            dayData["program"] = m_programsMap.value(progId);
                        } else {
                            dayData["program"] = progId; 
                        }
                    } else {
                        dayData["program"] = "כללי";
                    }
                    
                    dayData["req"] = assignment.isObligatory ? "חובה" : "בחירה";
                    
                    break;
                }
            }
        }
        
        m_calendarData.append(dayData);
    }
    
    emit currentCalendarDataChanged();
}

void ScheduleOutputManager::setAvailablePeriods(const QStringList& semesters, const QStringList& moeds) {
    m_semesters = semesters;
    m_moeds = moeds;
    
    emit availableSemestersChanged(); 
    emit availableMoedsChanged();
}

void ScheduleOutputManager::clearData() {
    m_solutions.clear();
    m_courses.clear();
    m_periods.clear();
    m_calendarData.clear();
    
    m_currentIndex = 0;
    m_selectedSemester = "";
    m_selectedMoed = "";

    emit totalSchedulesCountChanged();
    emit currentScheduleIndexChanged();
    emit currentCalendarDataChanged();
    
    qDebug() << "[MANAGER] Data cleared successfully.";
}

bool ScheduleOutputManager::isCurrentScheduleIndexValid() const {
    return !m_solutions.empty() &&
           m_currentIndex >= 1 &&
           m_currentIndex <= static_cast<int>(m_solutions.size());
}

bool ScheduleOutputManager::isOutputPathValid(const QString& filePath,
                                              QString* errorMessage) const {
    const QString trimmedPath = filePath.trimmed();

    if (trimmedPath.isEmpty()) {
        if (errorMessage) {
            *errorMessage = "Export path is empty.";
        }
        return false;
    }

    const QFileInfo fileInfo(trimmedPath);

    if (fileInfo.exists() && fileInfo.isDir()) {
        if (errorMessage) {
            *errorMessage = "Export path points to a directory, not a file.";
        }
        return false;
    }

    const QDir parentDir = fileInfo.absoluteDir();
    if (!parentDir.exists()) {
        if (errorMessage) {
            *errorMessage = "Export directory does not exist.";
        }
        return false;
    }

    return true;
}

bool ScheduleOutputManager::canExportCurrentSchedule(QString* errorMessage) const {
    if (m_solutions.empty()) {
        if (errorMessage) {
            *errorMessage = "Cannot export because no generated schedules are available.";
        }
        return false;
    }

    if (!isCurrentScheduleIndexValid()) {
        if (errorMessage) {
            *errorMessage = "Cannot export because the current schedule index is invalid.";
        }
        return false;
    }

    return true;
}

bool ScheduleOutputManager::saveCurrentScheduleToFile(const QString& filePath) {
    QString errorMessage;

    if (!canExportCurrentSchedule(&errorMessage)) {
        qWarning() << "[EXPORT] Export blocked:" << errorMessage;
        return false;
    }

    if (!isOutputPathValid(filePath, &errorMessage)) {
        qWarning() << "[EXPORT] Export blocked:" << errorMessage;
        return false;
    }

    QFile file(filePath.trimmed());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[EXPORT] Failed to open file for writing:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out << "Schedule Export - " << m_selectedSemester << " " << m_selectedMoed << "\n";
    out << "------------------------------------------\n";

    const auto& currentSolution = m_solutions[m_currentIndex - 1];
    const auto& assignments = currentSolution.getAssignments();

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

    file.close();
    return true;
}