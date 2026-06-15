#pragma once

#include <QAbstractListModel>
#include <QString>
#include <vector>
#include "domain/Course.h"

/*
 * ProgramCourseModel is a Qt model exposed to QML.
 *
 * It presents course rows filtered by academic program, year, and semester.
 * The model converts the internal Course domain objects into simple row data
 * that QML views can display through named roles.
 */
class ProgramCourseModel : public QAbstractListModel {
    Q_OBJECT

    /*
     * Filtering properties used by QML.
     *
     * When any filter changes, the visible course rows are rebuilt.
     */
    Q_PROPERTY(QString programId READ programId WRITE setProgramId NOTIFY filtersChanged)
    Q_PROPERTY(int year READ year WRITE setYear NOTIFY filtersChanged)
    Q_PROPERTY(QString semester READ semester WRITE setSemester NOTIFY filtersChanged)

    /*
     * Number of rows currently visible after filtering.
     */
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    /*
     * Custom roles used by QML delegates.
     *
     * Each role exposes one field from CourseRow.
     */
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

    /*
     * Creates an empty course model.
     */
    explicit ProgramCourseModel(QObject* parent = nullptr);

    /*
     * Returns the number of visible rows in the model.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /*
     * Returns data for a specific row and role.
     */
    QVariant data(const QModelIndex& index, int role) const override;

    /*
     * Returns the mapping between role IDs and role names used in QML.
     */
    QHash<int, QByteArray> roleNames() const override;

    /*
     * Returns the active program ID filter.
     */
    QString programId() const;

    /*
     * Returns the active year filter.
     *
     * A value of 0 means no year filter.
     */
    int year() const;

    /*
     * Returns the active semester filter.
     *
     * An empty string means no semester filter.
     */
    QString semester() const;

    /*
     * Returns the number of visible rows.
     */
    int count() const;

    /*
     * Replaces the source course list and rebuilds the filtered rows.
     */
    void setCourses(const std::vector<Course>& courses);

    /*
     * Updates only the program ID filter.
     */
    Q_INVOKABLE void setProgramId(const QString& programId);

    /*
     * Updates only the year filter.
     */
    Q_INVOKABLE void setYear(int year);

    /*
     * Updates only the semester filter.
     */
    Q_INVOKABLE void setSemester(const QString& semester);

    /*
     * Updates all filters together.
     */
    Q_INVOKABLE void setFilters(
        const QString& programId,
        int year,
        const QString& semester
    );

    /*
     * Clears all filters and shows all available course rows.
     */
    Q_INVOKABLE void clearFilters();

signals:
    /*
     * Emitted when one or more filter values change.
     */
    void filtersChanged();

    /*
     * Emitted when the number of visible rows changes.
     */
    void countChanged();

private:
    /*
     * A flattened, QML-friendly course row.
     *
     * A single Course may produce multiple CourseRow entries because a course
     * can belong to multiple programs, years, or semesters.
     */
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

    /*
     * Rebuilds visibleRows_ from sourceCourses_ according to the active filters.
     */
    void rebuild();

    /*
     * Converts the Semester enum into a display string.
     */
    static QString semesterToString(Semester semester);

    /*
     * Converts the Requirement enum into a display string.
     */
    static QString requirementToString(Requirement requirement);

    /*
     * Converts the Evaluation enum into a display string.
     */
    static QString evaluationToString(Evaluation evaluation);

    /*
     * Original loaded courses from the scheduling session.
     */
    std::vector<Course> sourceCourses_;

    /*
     * Flattened and filtered rows currently visible in QML.
     */
    std::vector<CourseRow> visibleRows_;

    /*
     * Active program ID filter.
     *
     * Empty means all programs.
     */
    QString programId_;

    /*
     * Active year filter.
     *
     * 0 means all years.
     */
    int year_ = 0;

    /*
     * Active semester filter.
     *
     * Empty means all semesters.
     */
    QString semester_;
};