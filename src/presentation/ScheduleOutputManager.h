#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <vector>
#include <QMap>

#include "domain/Course.h"
#include "domain/ExamPeriod.h"
#include "domain/ScheduleGenerationResult.h"

class ScheduleOutputManager : public QObject {
    Q_OBJECT

    // Expose variables to QML (The visual contract)
    Q_PROPERTY(int currentScheduleIndex READ getCurrentScheduleIndex NOTIFY currentScheduleIndexChanged)
    Q_PROPERTY(int totalSchedulesCount READ getTotalSchedulesCount NOTIFY totalSchedulesCountChanged)
    Q_PROPERTY(QStringList availableSemesters READ getAvailableSemesters NOTIFY availableSemestersChanged)
    Q_PROPERTY(QStringList availableMoeds READ getAvailableMoeds NOTIFY availableMoedsChanged)
    Q_PROPERTY(QVariantList currentCalendarData READ getCurrentCalendarData NOTIFY currentCalendarDataChanged)

public:
    explicit ScheduleOutputManager(QObject* parent = nullptr);

    // Setters
    void setSchedulingData(const std::vector<ScheduleGenerationResult>& solutions,
                           const std::vector<Course>& courses,
                           const std::vector<ExamPeriod>& periods);

    // Getters
    int getCurrentScheduleIndex() const;
    int getTotalSchedulesCount() const;
    QStringList getAvailableSemesters() const;
    QStringList getAvailableMoeds() const;
    QVariantList getCurrentCalendarData() const;
    
    void setCourses(const std::vector<Course>& courses) {
        m_courses = courses;
    }
    void setAvailablePeriods(const QStringList& semesters, const QStringList& moeds);

    void setProgramsMap(const QMap<QString, QString>& map) {
        m_programsMap = map;
    }

    // Actions triggered by the user from the QML interface
    Q_INVOKABLE void nextSchedule();
    Q_INVOKABLE void previousSchedule();
    Q_INVOKABLE void setPeriodFilter(const QString& semester, const QString& moed);
    Q_INVOKABLE void exportCurrentSchedule();
    Q_INVOKABLE void clearData();
    
    //to save and export the current schedule as txt file
    Q_INVOKABLE bool saveCurrentScheduleToFile(const QString& filePath);

signals:
    void currentScheduleIndexChanged();
    void totalSchedulesCountChanged();
    void availableSemestersChanged();
    void availableMoedsChanged();
    void currentCalendarDataChanged();

private:
    bool canExportCurrentSchedule(QString* errorMessage = nullptr) const;
    bool isCurrentScheduleIndexValid() const;
    bool isOutputPathValid(const QString& filePath, QString* errorMessage = nullptr) const;
    
    // Builds the calendar UI data for the specific month/period    
    void updateCalendarData(); 
    // Extracts unique semesters and moeds from the ExamPeriod data
    void extractAvailableFilters(); 

    // Map to convert programID to programName for display purposes
    QMap<QString, QString> m_programsMap;

    // The core data structures holding the scheduling results and related info
    std::vector<ScheduleGenerationResult> m_solutions;
    
    std::vector<Course> m_courses;
    std::vector<ExamPeriod> m_periods;

    // Current state of the output screen
    int m_currentIndex = 1;
    QString m_selectedSemester;
    QString m_selectedMoed;

    QStringList m_semesters;
    QStringList m_moeds;
    QVariantList m_calendarData;
};