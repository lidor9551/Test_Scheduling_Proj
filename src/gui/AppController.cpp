#include "gui/AppController.h"

#include "parser/InputParser.h"

#include <QFileInfo>
#include <QUrl>
#include <QDir>

#include <set>
#include <string>
#include <utility>

AppController::AppController(QObject* parent)
    : QObject(parent) {
}

QString AppController::coursesFilePath() const {
    return coursesFilePath_;
}

QString AppController::examPeriodsFilePath() const {
    return examPeriodsFilePath_;
}

QString AppController::statusMessage() const {
    return statusMessage_;
}

QString AppController::errorMessage() const {
    return errorMessage_;
}

int AppController::courseCount() const {
    return static_cast<int>(courses_.size());
}

int AppController::examPeriodCount() const {
    return static_cast<int>(examPeriods_.size());
}

bool AppController::hasData() const {
    return !courses_.empty() || !examPeriods_.empty();
}

void AppController::setCoursesFilePath(const QString& path) {
    coursesFilePath_ = normalizePath(path);
    emit coursesFilePathChanged();
}

void AppController::setExamPeriodsFilePath(const QString& path) {
    examPeriodsFilePath_ = normalizePath(path);
    emit examPeriodsFilePathChanged();
}

ProgramCourseModel* AppController::programCourseModel() {
    return &programCourseModel_;
}

void AppController::replaceData() {
    clearMessages();

    if (!validateSelectedFiles()) {
        return;
    }

    try {
        InputParser parser;

        std::vector<Course> loadedCourses =
            parser.parseCourses(coursesFilePath_.toStdString());

        std::vector<ExamPeriod> loadedPeriods =
            parser.parseExamPeriods(examPeriodsFilePath_.toStdString());

        courses_ = std::move(loadedCourses);
        examPeriods_ = std::move(loadedPeriods);
        programCourseModel_.setCourses(courses_);

        emit dataChanged();

        setStatus(
            QString("Data replaced successfully. Loaded %1 courses and %2 exam periods.")
                .arg(courseCount())
                .arg(examPeriodCount())
        );
    } catch (const std::exception& ex) {
        setError(QString("Failed to replace data: %1").arg(ex.what()));
    }
}

void AppController::appendData() {
    clearMessages();

    if (!validateSelectedFiles()) {
        return;
    }

    try {
        InputParser parser;

        std::vector<Course> loadedCourses =
            parser.parseCourses(coursesFilePath_.toStdString());

        std::vector<ExamPeriod> loadedPeriods =
            parser.parseExamPeriods(examPeriodsFilePath_.toStdString());

        std::set<std::string> existingCourseNumbers;
        for (const Course& course : courses_) {
            existingCourseNumbers.insert(course.getCourseNumber());
        }

        int addedCourses = 0;
        int skippedCourses = 0;

        for (const Course& course : loadedCourses) {
            const std::string& courseNumber = course.getCourseNumber();

            if (existingCourseNumbers.insert(courseNumber).second) {
                courses_.push_back(course);
                addedCourses++;
            } else {
                skippedCourses++;
            }
        }

        std::set<std::pair<std::string, std::string>> existingPeriods;
        for (const ExamPeriod& period : examPeriods_) {
            existingPeriods.insert({period.getSemester(), period.getMoed()});
        }

        int addedPeriods = 0;
        int skippedPeriods = 0;

        for (const ExamPeriod& period : loadedPeriods) {
            std::pair<std::string, std::string> key = {
                period.getSemester(),
                period.getMoed()
            };

            if (existingPeriods.insert(key).second) {
                examPeriods_.push_back(period);
                addedPeriods++;
            } else {
                skippedPeriods++;
            }
        }
        programCourseModel_.setCourses(courses_);
        
        emit dataChanged();

        setStatus(
            QString("Append completed. Added %1 courses, skipped %2 duplicate courses. Added %3 periods, skipped %4 duplicate periods.")
                .arg(addedCourses)
                .arg(skippedCourses)
                .arg(addedPeriods)
                .arg(skippedPeriods)
        );
    } catch (const std::exception& ex) {
        setError(QString("Failed to append data: %1").arg(ex.what()));
    }
}

void AppController::clearMessages() {
    if (!statusMessage_.isEmpty()) {
        statusMessage_.clear();
        emit statusMessageChanged();
    }

    if (!errorMessage_.isEmpty()) {
        errorMessage_.clear();
        emit errorMessageChanged();
    }
}

QString AppController::normalizePath(const QString& rawPath) const {
    if (rawPath.isEmpty()) {
        return {};
    }

    const QUrl url(rawPath);

    if (url.isLocalFile()) {
        return QDir::toNativeSeparators(url.toLocalFile());
    }

    return QDir::toNativeSeparators(rawPath);
}

bool AppController::validateSelectedFiles() {
    if (coursesFilePath_.isEmpty()) {
        setError("Courses file was not selected.");
        return false;
    }

    if (examPeriodsFilePath_.isEmpty()) {
        setError("Exam periods file was not selected.");
        return false;
    }

    if (!QFileInfo::exists(coursesFilePath_)) {
        setError(QString("Courses file does not exist: %1").arg(coursesFilePath_));
        return false;
    }

    if (!QFileInfo::exists(examPeriodsFilePath_)) {
        setError(QString("Exam periods file does not exist: %1").arg(examPeriodsFilePath_));
        return false;
    }

    return true;
}

void AppController::setStatus(const QString& message) {
    statusMessage_ = message;
    emit statusMessageChanged();
}

void AppController::setError(const QString& message) {
    errorMessage_ = message;
    emit errorMessageChanged();
}

QVariantList AppController::getCoursesVariant() const {
    QVariantList list;
    for (const Course& c : courses_)
        list.append(QVariant::fromValue(c));
    return list;
}

QVariantList AppController::getExamPeriodsVariant() const {
    QVariantList list;
    for (const ExamPeriod& p : examPeriods_)
        list.append(QVariant::fromValue(p));
    return list;
}