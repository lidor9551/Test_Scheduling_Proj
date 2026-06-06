#include "gui/ProgramCourseModel.h"

namespace {

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

ProgramCourseModel::ProgramCourseModel(QObject* parent)
    : QAbstractListModel(parent) {
}

int ProgramCourseModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }

    return static_cast<int>(visibleRows_.size());
}

QVariant ProgramCourseModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()
        || index.row() < 0
        || index.row() >= static_cast<int>(visibleRows_.size())) {
        return {};
    }

    const CourseRow& row = visibleRows_[index.row()];

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

QString ProgramCourseModel::programId() const {
    return programId_;
}

int ProgramCourseModel::year() const {
    return year_;
}

QString ProgramCourseModel::semester() const {
    return semester_;
}

int ProgramCourseModel::count() const {
    return static_cast<int>(visibleRows_.size());
}

void ProgramCourseModel::setCourses(const std::vector<Course>& courses) {
    sourceCourses_ = courses;
    rebuild();
}

void ProgramCourseModel::setProgramId(const QString& programId) {
    setFilters(programId, year_, semester_);
}

void ProgramCourseModel::setYear(int year) {
    setFilters(programId_, year, semester_);
}

void ProgramCourseModel::setSemester(const QString& semester) {
    setFilters(programId_, year_, semester);
}

void ProgramCourseModel::setFilters(
    const QString& programId,
    int year,
    const QString& semester
) {
    const QString normalizedProgramId = programId.trimmed();
    const int normalizedYear = year >= 1 && year <= 4 ? year : 0;
    const QString normalizedSemester = normalizeSemester(semester);

    if (programId_ == normalizedProgramId
        && year_ == normalizedYear
        && semester_ == normalizedSemester) {
        return;
    }

    programId_ = normalizedProgramId;
    year_ = normalizedYear;
    semester_ = normalizedSemester;

    rebuild();
    emit filtersChanged();
}

void ProgramCourseModel::clearFilters() {
    setFilters({}, 0, {});
}

void ProgramCourseModel::rebuild() {
    const int previousCount = count();

    beginResetModel();
    visibleRows_.clear();

    for (const Course& course : sourceCourses_) {
        for (const ProgramDetails& program : course.getPrograms()) {
            const QString rowProgramId =
                QString::fromStdString(program.programID);

            const QString rowSemester =
                semesterToString(program.semester);

            if (!programId_.isEmpty() && rowProgramId != programId_) {
                continue;
            }

            if (year_ != 0 && program.year != year_) {
                continue;
            }

            if (!semester_.isEmpty() && rowSemester != semester_) {
                continue;
            }

            visibleRows_.push_back({
                QString::fromStdString(course.getCourseNumber()),
                QString::fromStdString(course.getCourseName()),
                QString::fromStdString(course.getInstructorName()),
                rowProgramId,
                program.year,
                rowSemester,
                requirementToString(program.requirement),
                evaluationToString(course.getEvaluationMethod())
            });
        }
    }

    endResetModel();

    if (previousCount != count()) {
        emit countChanged();
    }
}

QString ProgramCourseModel::semesterToString(Semester semester) {
    switch (semester) {
        case Semester::FALL:
            return "FALL";

        case Semester::SPRI:
            return "SPRI";

        case Semester::SUMM:
            return "SUMM";
    }

    return "UNKNOWN";
}

QString ProgramCourseModel::requirementToString(Requirement requirement) {
    switch (requirement) {
        case Requirement::OBLIGATORY:
            return "Obligatory";

        case Requirement::ELECTIVE:
            return "Elective";
    }

    return "Unknown";
}

QString ProgramCourseModel::evaluationToString(Evaluation evaluation) {
    switch (evaluation) {
        case Evaluation::EXAM:
            return "Exam";

        case Evaluation::PROJECT:
            return "Project";

        case Evaluation::ATTENDANCE:
            return "Attendance";
    }

    return "Unknown";
}