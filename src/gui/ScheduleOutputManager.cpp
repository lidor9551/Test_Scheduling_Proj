#include "ScheduleOutputManager.h"
#include <QDebug>
#include <QDate>
#include <set>
#include <QFile>
#include <QTextStream>

ScheduleOutputManager::ScheduleOutputManager(QObject* parent) 
    : QObject(parent), m_currentIndex(1) {}

void ScheduleOutputManager::setSchedulingData(const std::vector<std::vector<int>>& solutions,
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
    // Future implementation: Logic for creating a PDF or saving an image
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
    // In Qt: 7 is Sunday, 1 is Monday... 6 is Saturday.
    // We always want to start the grid on Sunday and end on Saturday.
    int startDow = qStart.dayOfWeek() % 7; 
    QDate gridStart = qStart.addDays(-startDow);

    int endDow = qEnd.dayOfWeek() % 7;
    QDate gridEnd = qEnd.addDays(6 - endDow);

    std::vector<Date> allowed = activePeriod->allowedDates();
    const auto& currentSolution = m_solutions[m_currentIndex - 1];

    // --- Continuous loop from the grid start to the grid end ---
    for (QDate current = gridStart; current <= gridEnd; current = current.addDays(1)) {
        QVariantMap dayData;

        // Check: Are we in the padding cells (before the period starts or after it ends)?
        if (current < qStart || current > qEnd) {
            dayData["dayText"] = ""; // QML will recognize this empty string and make the cell transparent
            dayData["isExcluded"] = false;
            dayData["hasExam"] = false;
            dayData["examName"] = "";
            dayData["courseId"] = "";
            dayData["req"] = "";
            dayData["program"] = "";
        } 
        else {
            // Active day inside the period (convert back to custom Date for checks)
            Date currentDate(current.day(), current.month(), current.year());

            // UX improvement: On the first day of a month or the first day of the period, show the month (e.g., "1/2")
            if (current.day() == 1 || current == qStart) {
                dayData["dayText"] = QString::number(current.day()) + "/" + QString::number(current.month());
            } else {
                dayData["dayText"] = QString::number(current.day());
            }

            // Check exclusions (is the day in the allowed dates list?)
            bool isAllowed = false;
            for (const auto& d : allowed) {
                if (d == currentDate) { isAllowed = true; break; }
            }
            // Saturday (6) is also considered visually excluded on the calendar
            dayData["isExcluded"] = !isAllowed || (current.dayOfWeek() == 6);
            dayData["hasExam"] = false;
            dayData["examName"] = "";
            dayData["courseId"] = "";
            dayData["req"] = "";
            dayData["program"] = "";

            // --- Original code mapping the scheduled exams to the current date ---
            for (size_t courseIdx = 0; courseIdx < m_courses.size(); ++courseIdx) {
                const Course& currentCourse = m_courses[courseIdx];
                if (currentCourse.getEvaluationMethod() != Evaluation::EXAM) { 
                    continue; 
                }
                int scheduledDateIndex = -1;

                if (courseIdx < currentSolution.size()) {
                    scheduledDateIndex = currentSolution[courseIdx];
                } 

                if (scheduledDateIndex >= 0 && scheduledDateIndex < (int)allowed.size()) {
                    Date examDate = allowed[scheduledDateIndex];
                    
                    if (examDate == currentDate) {
                        dayData["hasExam"] = true;
                        dayData["examName"] = QString::fromStdString(currentCourse.getCourseName()); 
                        dayData["courseId"] = QString::fromStdString(currentCourse.getCourseNumber()); 
                        
                        if (!currentCourse.getPrograms().empty()) {
                            const auto& progDetails = currentCourse.getPrograms().front();
                            QString progId = QString::fromStdString(progDetails.programID);

                            if (m_programsMap.contains(progId)) {
                                dayData["program"] = m_programsMap.value(progId);
                            } else {
                                dayData["program"] = progId; 
                            }

                            dayData["req"] = (progDetails.requirement == Requirement::OBLIGATORY) ? "חובה" : "בחירה";
                        } else {
                            dayData["program"] = "כללי";
                            dayData["req"] = "-";
                        }
                    }
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
    // empty all data containers
    m_solutions.clear();
    m_courses.clear();
    m_periods.clear();
    m_calendarData.clear();
    
    // set default state
    m_currentIndex = 0;
    m_selectedSemester = "";
    m_selectedMoed = "";

    // refresh the UI by emitting the necessary signals
    emit totalSchedulesCountChanged();
    emit currentScheduleIndexChanged();
    emit currentCalendarDataChanged();
    
    qDebug() << "[MANAGER] Data cleared successfully.";
}


bool ScheduleOutputManager::saveCurrentScheduleToFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream out(&file);
    out << "Schedule Export - " << m_selectedSemester << " " << m_selectedMoed << "\n";
    out << "------------------------------------------\n";

    // currently showing schedule at m_currentIndex, get the corresponding solution
    const auto& currentSolution = m_solutions[m_currentIndex - 1];
    
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

    if (!activePeriod) return false;
    const auto& allowed = activePeriod->allowedDates();

    // list all courses with their scheduled dates
    for (size_t i = 0; i < m_courses.size(); ++i) {
        if (m_courses[i].getEvaluationMethod() != Evaluation::EXAM) continue;
        
        int dateIdx = currentSolution[i];
        out << "Course: " << QString::fromStdString(m_courses[i].getCourseName()) 
            << " | ID: " << QString::fromStdString(m_courses[i].getCourseNumber()) << "\n";
        
        if (dateIdx >= 0 && dateIdx < (int)allowed.size()) {
            out << "Date: " << allowed[dateIdx].toString().c_str() << "\n";
        } else {
            out << "Date: Not Scheduled\n";
        }
        out << "------------------------------------------\n";
    }

    file.close();
    return true;
}