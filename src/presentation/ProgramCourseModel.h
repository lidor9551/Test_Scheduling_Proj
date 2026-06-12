#pragma once

#include <QAbstractListModel>
#include <QString>
#include <vector>
#include "domain/Course.h"

class ProgramCourseModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(QString programId READ programId WRITE setProgramId NOTIFY filtersChanged)
    Q_PROPERTY(int year READ year WRITE setYear NOTIFY filtersChanged)
    Q_PROPERTY(QString semester READ semester WRITE setSemester NOTIFY filtersChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Role {
        CourseNumberRole = Qt::UserRole + 1,
        CourseNameRole,
        InstructorNameRole,
        ProgramIdRole,
        YearRole,
        SemesterRole,
        RequirementRole,
        EvaluationRole
    };
    Q_ENUM(Role)

    explicit ProgramCourseModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString programId() const;
    int year() const;
    QString semester() const;
    int count() const;

    void setCourses(const std::vector<Course>& courses);

    Q_INVOKABLE void setProgramId(const QString& programId);
    Q_INVOKABLE void setYear(int year);
    Q_INVOKABLE void setSemester(const QString& semester);

    Q_INVOKABLE void setFilters(
        const QString& programId,
        int year,
        const QString& semester
    );

    Q_INVOKABLE void clearFilters();

signals:
    void filtersChanged();
    void countChanged();

private:
    struct CourseRow {
        QString courseNumber;
        QString courseName;
        QString instructorName;
        QString programId;
        int year;
        QString semester;
        QString requirement;
        QString evaluation;
    };

    void rebuild();

    static QString semesterToString(Semester semester);
    static QString requirementToString(Requirement requirement);
    static QString evaluationToString(Evaluation evaluation);

    std::vector<Course> sourceCourses_;
    std::vector<CourseRow> visibleRows_;

    QString programId_;
    int year_ = 0;
    QString semester_;
};