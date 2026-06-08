#pragma once

#include <QObject>
#include <QString>
#include <vector>
#include "model/Course.h"
#include "model/ExamPeriod.h"
#include "gui/ProgramCourseModel.h"
#include <QVariant>
#include <QVariantList>
#include "ScheduleOutputManager.h"

class AppController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString coursesFilePath READ coursesFilePath NOTIFY coursesFilePathChanged)
    Q_PROPERTY(QString examPeriodsFilePath READ examPeriodsFilePath NOTIFY examPeriodsFilePathChanged)

    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

    Q_PROPERTY(int courseCount READ courseCount NOTIFY dataChanged)
    Q_PROPERTY(int examPeriodCount READ examPeriodCount NOTIFY dataChanged)
    Q_PROPERTY(bool hasData READ hasData NOTIFY dataChanged)

    Q_PROPERTY(QVariantList courses READ getCoursesVariant NOTIFY dataChanged)
    Q_PROPERTY(QVariantList examPeriods READ getExamPeriodsVariant NOTIFY dataChanged)

    // For the program course model
    Q_PROPERTY(QStringList availablePrograms READ availablePrograms CONSTANT)
    // only selected program IDs, not the full course data, since the model will handle that dynamically
    Q_PROPERTY(QStringList selectedPrograms READ selectedPrograms NOTIFY selectedProgramsChanged)

    Q_PROPERTY(ProgramCourseModel* programCourseModel READ programCourseModel CONSTANT)

    // the output manager instance to be used across the app and exposed to QML
    Q_PROPERTY(ScheduleOutputManager* outputManager READ outputManager CONSTANT)

public:
    explicit AppController(QObject* parent = nullptr);

    QVariantList getCoursesVariant() const;
    QVariantList getExamPeriodsVariant() const;

    QString coursesFilePath() const;
    QString examPeriodsFilePath() const;

    QString statusMessage() const;
    QString errorMessage() const;

    int courseCount() const;
    int examPeriodCount() const;
    bool hasData() const;

    Q_INVOKABLE void setCoursesFilePath(const QString& path);
    Q_INVOKABLE void setExamPeriodsFilePath(const QString& path);

    Q_INVOKABLE void replaceData();
    Q_INVOKABLE void appendData();
    Q_INVOKABLE void clearMessages();

    QStringList availablePrograms() const;
    QStringList selectedPrograms() const;

    // to present in the UI the relevant courses for the selected program
    Q_INVOKABLE QVariantList getCoursesForProgram(const QString& programId, int year = -1, int semester = -1);

    //qml checboxes will call this to toggle program selection
    Q_INVOKABLE void toggleProgram(const QString& programId);

    ProgramCourseModel* programCourseModel();
    
    // getter for the output manager
    ScheduleOutputManager* outputManager();

    // the function called from QML to start the scheduling algorithm
    Q_INVOKABLE void generateSchedules();

public slots:
    // to capture the results from the scheduling algorithm and pass them to the output manager
    void onSchedulingFinished(const std::vector<std::vector<int>>& solutions);
    void onSchedulingFailed(QString message);

signals:
    void coursesFilePathChanged();
    void examPeriodsFilePathChanged();

    void statusMessageChanged();
    void errorMessageChanged();

    void dataChanged();
    void selectedProgramsChanged();

private:
    QString normalizePath(const QString& rawPath) const;

    bool validateSelectedFiles();
    void setStatus(const QString& message);
    void setError(const QString& message);
    
    ProgramCourseModel programCourseModel_;
    
    // output manager instance to be used across the app and exposed to QML
    ScheduleOutputManager m_outputManager;

private:
    QString coursesFilePath_;
    QString examPeriodsFilePath_;

    QString statusMessage_;
    QString errorMessage_;

    std::vector<Course> courses_;
    std::vector<ExamPeriod> examPeriods_;

    QStringList m_availablePrograms;
    QStringList m_selectedPrograms;
};