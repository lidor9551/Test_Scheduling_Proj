#include "presentation/ProgramCourseModel.h"

/*
 * Local helper functions used only by this implementation file.
 */
namespace {

/*
 * Normalizes a semester string received from QML.
 *
 * The UI may pass friendly names such as SPRING or SUMMER.
 * The model stores the same compact values used by the domain layer:
 * - FALL
 * - SPRI
 * - SUMM
 *
 * "ALL" is normalized to an empty string, which means no semester filter.
 */
QString normalizeSemester(QString semester) {
    semester = semester.trimmed().toUpper();

    if (semester == "SPRING") {
        return "SPRI";
    }

    if (semester == "SUMMER") {
        return "SUMM";
    }

    if (semester == "ALL") {
        return {};
    }

    return semester;
}

} // namespace

/*
 * Creates an empty list model.
 */
ProgramCourseModel::ProgramCourseModel(QObject* parent)
    : QAbstractListModel(parent) {
}

/*
 * Returns the number of visible rows.
 *
 * Qt calls this method when QML needs to know how many items to render.
 */
int ProgramCourseModel::rowCount(const QModelIndex& parent) const {
    /*
     * This is a flat list model, so child indexes are not supported.
     */
    if (parent.isValid()) {
        return 0;
    }

    return static_cast<int>(visibleRows_.size());
}

/*
 * Returns the value for a given row and role.
 */
QVariant ProgramCourseModel::data(const QModelIndex& index, int role) const {
    /*
     * Reject invalid indexes to avoid out-of-bounds access.
     */
    if (!index.isValid()
        || index.row() < 0
        || index.row() >= static_cast<int>(visibleRows_.size())) {
        return {};
    }

    const CourseRow& row = visibleRows_[index.row()];

    /*
     * Map each custom role to the matching CourseRow field.
     */
    switch (role) {
        case CourseNumberRole:
            return row.courseNumber;

        case CourseNameRole:
            return row.courseName;

        case InstructorNameRole:
            return row.instructorName;

        case ProgramIdRole:
            return row.programId;

        case YearRole:
            return row.year;

        case SemesterRole:
            return row.semester;

        case RequirementRole:
            return row.requirement;

        case EvaluationRole:
            return row.evaluation;

        default:
            return {};
    }
}

/*
 * Returns role names exposed to QML.
 *
 * QML delegates can use names such as "courseName" and "semester"
 * instead of numeric role IDs.
 */
QHash<int, QByteArray> ProgramCourseModel::roleNames() const {
    return {
        {CourseNumberRole, "courseNumber"},
        {CourseNameRole, "courseName"},
        {InstructorNameRole, "instructorName"},
        {ProgramIdRole, "programId"},
        {YearRole, "year"},
        {SemesterRole, "semester"},
        {RequirementRole, "requirement"},
        {EvaluationRole, "evaluation"}
    };
}

/*
 * Returns the active program ID filter.
 */
QString ProgramCourseModel::programId() const {
    return programId_;
}

/*
 * Returns the active year filter.
 */
int ProgramCourseModel::year() const {
    return year_;
}

/*
 * Returns the active semester filter.
 */
QString ProgramCourseModel::semester() const {
    return semester_;
}

/*
 * Returns the number of currently visible rows.
 */
int ProgramCourseModel::count() const {
    return static_cast<int>(visibleRows_.size());
}

/*
 * Sets the source course list and rebuilds the visible rows.
 */
void ProgramCourseModel::setCourses(const std::vector<Course>& courses) {
    sourceCourses_ = courses;
    rebuild();
}

/*
 * Updates the program ID filter while keeping the other filters unchanged.
 */
void ProgramCourseModel::setProgramId(const QString& programId) {
    setFilters(programId, year_, semester_);
}

/*
 * Updates the year filter while keeping the other filters unchanged.
 */
void ProgramCourseModel::setYear(int year) {
    setFilters(programId_, year, semester_);
}

/*
 * Updates the semester filter while keeping the other filters unchanged.
 */
void ProgramCourseModel::setSemester(const QString& semester) {
    setFilters(programId_, year_, semester);
}

/*
 * Updates all filter values together.
 *
 * Invalid years are normalized to 0, which means "all years".
 */
void ProgramCourseModel::setFilters(
    const QString& programId,
    int year,
    const QString& semester
) {
    /*
     * Normalize all filter values before comparing or storing them.
     */
    const QString normalizedProgramId = programId.trimmed();
    const int normalizedYear = year >= 1 && year <= 4 ? year : 0;
    const QString normalizedSemester = normalizeSemester(semester);

    /*
     * Avoid unnecessary model resets when the filters did not change.
     */
    if (programId_ == normalizedProgramId
        && year_ == normalizedYear
        && semester_ == normalizedSemester) {
        return;
    }

    programId_ = normalizedProgramId;
    year_ = normalizedYear;
    semester_ = normalizedSemester;

    /*
     * Rebuild the visible rows according to the new filters.
     */
    rebuild();
    emit filtersChanged();
}

/*
 * Clears all filters.
 */
void ProgramCourseModel::clearFilters() {
    setFilters({}, 0, {});
}

/*
 * Rebuilds the model rows from the source courses.
 *
 * Each ProgramDetails entry inside each Course can become one visible row,
 * unless it is filtered out by program ID, year, or semester.
 */
void ProgramCourseModel::rebuild() {
    const int previousCount = count();

    /*
     * Notify Qt views that the model is about to be reset.
     */
    beginResetModel();
    visibleRows_.clear();

    /*
     * Flatten Course objects into QML-friendly CourseRow entries.
     */
    for (const Course& course : sourceCourses_) {
        for (const ProgramDetails& program : course.getPrograms()) {
            const QString rowProgramId =
                QString::fromStdString(program.programID);

            const QString rowSemester =
                semesterToString(program.semester);

            /*
             * Apply program ID filter when it is active.
             */
            if (!programId_.isEmpty() && rowProgramId != programId_) {
                continue;
            }

            /*
             * Apply year filter when it is active.
             */
            if (year_ != 0 && yearToInt(program.year) != year_) {
                continue;
            }

            /*
             * Apply semester filter when it is active.
             */
            if (!semester_.isEmpty() && rowSemester != semester_) {
                continue;
            }

            /*
             * Add the flattened row to the visible model data.
             */
            visibleRows_.push_back({
                QString::fromStdString(course.getCourseNumber()),
                QString::fromStdString(course.getCourseName()),
                QString::fromStdString(course.getInstructorName()),
                rowProgramId,
                yearToInt(program.year),
                rowSemester,
                requirementToString(program.requirement),
                evaluationToString(course.getEvaluationMethod())
            });
        }
    }

    /*
     * Notify Qt views that the reset is complete.
     */
    endResetModel();

    /*
     * Emit countChanged only if the visible row count actually changed.
     */
    if (previousCount != count()) {
        emit countChanged();
    }
}

/*
 * Converts a Semester enum into a QString.
 */
QString ProgramCourseModel::semesterToString(Semester semester) {
    return QString::fromStdString(::semesterToString(semester));
}

/*
 * Converts a Requirement enum into a QString.
 */
QString ProgramCourseModel::requirementToString(Requirement requirement) {
    return QString::fromStdString(::requirementToString(requirement));
}

/*
 * Converts an Evaluation enum into a QString.
 */
QString ProgramCourseModel::evaluationToString(Evaluation evaluation) {
    return QString::fromStdString(::evaluationToString(evaluation));
}