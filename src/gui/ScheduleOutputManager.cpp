#include "ScheduleOutputManager.h"
#include <QDebug>
#include <QDate>
#include <set>

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

int ScheduleOutputManager::getCurrentScheduleIndex() const { return m_currentIndex; }
int ScheduleOutputManager::getTotalSchedulesCount() const { return m_solutions.size(); }
QStringList ScheduleOutputManager::getAvailableSemesters() const { return m_semesters; }
QStringList ScheduleOutputManager::getAvailableMoeds() const { return m_moeds; }
QVariantList ScheduleOutputManager::getCurrentCalendarData() const { return m_calendarData; }

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
        uniqueSemesters.insert(QString::fromStdString(period.getSemester()));
        uniqueMoeds.insert(QString::fromStdString(period.getMoed()));
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

    const ExamPeriod* activePeriod = nullptr;
    for (const auto& period : m_periods) {
        if (QString::fromStdString(period.getSemester()) == m_selectedSemester &&
            QString::fromStdString(period.getMoed()) == m_selectedMoed) {
            activePeriod = &period;
            break;
        }
    }

    if (!activePeriod) {
        qDebug() << "[CALENDAR] ERROR: Could not find matching period in m_periods!";
        emit currentCalendarDataChanged();
        return;
    }

    Date start = activePeriod->getStartDate();
    qDebug() << "[CALENDAR] Success! Found period starting on month:" << start.getMonth();
    Date end = activePeriod->getEndDate();
    QDate periodStart(start.getYear(), start.getMonth(), start.getDay());
    QDate currentMonthStart(periodStart.year(), periodStart.month(), 1);
    int daysInMonth = currentMonthStart.daysInMonth();
    int firstDayOfWeek = currentMonthStart.dayOfWeek() % 7; 

    // adding ALL keys to avoid 'undefined'
    for (int i = 0; i < firstDayOfWeek; ++i) {
        QVariantMap emptyDay;
        emptyDay["dayText"] = "";
        emptyDay["isExcluded"] = false;
        emptyDay["hasExam"] = false;
        emptyDay["examName"] = "";
        emptyDay["courseId"] = "";
        emptyDay["req"] = "";
        emptyDay["program"] = "";
        m_calendarData.append(emptyDay);
    }

    std::vector<Date> allowed = activePeriod->allowedDates();
    const auto& currentSolution = m_solutions[m_currentIndex - 1];

    for (int day = 1; day <= daysInMonth; ++day) {
        Date currentDate(day, currentMonthStart.month(), currentMonthStart.year());

        QVariantMap dayData;
        dayData["dayText"] = QString::number(day);
        dayData["isExcluded"] = (currentDate < start || currentDate > end);
        dayData["hasExam"] = false;
        dayData["examName"] = "";
        dayData["courseId"] = "";
        dayData["req"] = "";
        dayData["program"] = "";

        bool isOutsidePeriod = (currentDate < start || currentDate > end);
        bool isAllowed = false;
        for (const auto& d : allowed) {
            if (d == currentDate) { isAllowed = true; break; }
        }
        dayData["isExcluded"] = isOutsidePeriod || !isAllowed;

                // 4. Map the scheduling solution to the current date
        // FIXED: Iterate over m_courses.size() to catch ALL courses
        for (size_t courseIdx = 0; courseIdx < m_courses.size(); ++courseIdx) {
            const Course& currentCourse = m_courses[courseIdx];
            // Check if the course evaluation is exam
            if (currentCourse.getEvaluationMethod() != Evaluation::EXAM) { 
                continue; 
            }
            int scheduledDateIndex = -1; // Default: not scheduled

            // Check if the solution has an index for this course
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
                        dayData["program"] = QString::fromStdString(progDetails.programID);
                        dayData["req"] = (progDetails.requirement == Requirement::OBLIGATORY) ? "חובה" : "בחירה";
                    } else {
                        dayData["program"] = "כללי";
                        dayData["req"] = "-";
                    }
                }
            } else {
                // Here you can handle the case where the course is NOT scheduled
                // For example: log it or show it in a special "unscheduled" list
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