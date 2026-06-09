#include "gui/AppController.h"
#include "parser/InputParser.h"
#include "scheduler/ScheduleGenerator.h" 
#include "scheduler/SchedulingWorker.h"
#include "preprocessing/Preprocessor.h"
#include <QFileInfo>
#include <QUrl>
#include <QDir>
#include <set>
#include <string>
#include <utility>
#include <QThread>


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
        m_outputManager.setCourses(courses_);

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
        m_outputManager.setCourses(courses_);
        
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

QStringList AppController::availablePrograms() const {
    if (m_availablePrograms.isEmpty()) {
        return QStringList{"83101", "83102", "83103", "83104", "83105", "83107", "83108", "83109", "83115", "83182"};
    }
    return m_availablePrograms;
}

QStringList AppController::selectedPrograms() const {
    return m_selectedPrograms;
}

void AppController::toggleProgram(const QString& programId) {
    if (m_selectedPrograms.contains(programId)) {
        m_selectedPrograms.removeAll(programId);
        emit selectedProgramsChanged();
    } 
    else {
        if (m_selectedPrograms.size() < 5) {
            m_selectedPrograms.append(programId);
            emit selectedProgramsChanged();
        } else {
            qDebug() << "Cannot select more than 5 programs.";
        }
    }
}

QVariantList AppController::getCoursesForProgram(const QString& programId, int year, int semester) {
    QVariantList courseList;
    std::string pid = programId.toStdString();

    for (const auto& course : courses_) { 
        const auto& programs = course.getPrograms();
        
        for (const auto& p : programs) {
            // basic filtering by program ID
            bool match = (p.programID == pid);
            
            // if year or semester are specified (not -1), also filter by them
            if (year != -1) match &= (p.year == year);
            if (semester != -1) match &= (static_cast<int>(p.semester) == semester);

            if (match) {
                // convert Course to QVariant and add to list
                QVariantMap courseData;
                courseData["name"] = QString::fromStdString(course.getCourseName());

                // course requirement (obligatory/elective)
                courseData["req"] = (p.requirement == Requirement::OBLIGATORY) ? "חובה" : "בחירה";

                // evaluation method (exam/project/attendance)
                Evaluation eval = course.getEvaluationMethod();
                QString evalText;
                if (eval == Evaluation::EXAM) evalText = "מבחן";
                else if (eval == Evaluation::PROJECT) evalText = "פרויקט";
                else evalText = "נוכחות";
                courseData["eval"] = evalText;

                courseList.append(courseData);
            }
        }
    }
    return courseList;
}

ScheduleOutputManager* AppController::outputManager() {
    return &m_outputManager;
}

void AppController::generateSchedules() {
    qDebug() << "=========================================";
    qDebug() << ">>> generateSchedules STARTED! <<<";
    qDebug() << "Selected programs count:" << m_selectedPrograms.size();
    setStatus("Generating schedules...");

    // Keep a reference to the full course list for preprocessing
    std::vector<Course>& allCourses = this->courses_; 
    
    // Create a filtered list of courses that require exams
    std::vector<Course> examOnlyCourses;
    for (const auto& course : allCourses) {
        if (course.getEvaluationMethod() == Evaluation::EXAM) {
            examOnlyCourses.push_back(course);
        }
    }

    qDebug() << ">>> Courses exposed to algorithm:";
    for (const auto& course : examOnlyCourses) {
        qDebug() << "    -" << QString::fromStdString(course.getCourseName());
    }

    // Expose the exam-only courses to the output manager  
    m_outputManager.setCourses(examOnlyCourses);

    // Build the scheduling blocks based on the selected programs
    std::vector<std::string> selectedProgs;
    for (const auto& prog : m_selectedPrograms) {
        selectedProgs.push_back(prog.toStdString());
    }

    // Build all blocks and save them to the class member (m_allBlocks)
    SchedulingPreprocessor preprocessor(allCourses, this->examPeriods_, selectedProgs);
    m_allBlocks = preprocessor.buildBlocks();

    if (m_allBlocks.empty()) {
        setError("No valid scheduling blocks found.");
        return;
    }

    // Extract available semesters and moedim from the blocks to populate the UI dropdowns
    QStringList availableSemesters;
    QStringList availableMoeds;
    
    for (const auto& block : m_allBlocks) {
        QString sem = QString::fromStdString(block.semester);
        QString moed = QString::fromStdString(block.moed);
        
        if (!availableSemesters.contains(sem)) {
            availableSemesters.append(sem);
        }
        if (!availableMoeds.contains(moed)) {
            availableMoeds.append(moed);
        }
    }

    // Expose the available periods to the output manager for UI dropdowns
    m_outputManager.setAvailablePeriods(availableSemesters, availableMoeds);

    // Trigger the algorithm automatically for the first valid block (default view)
    QString firstSem = QString::fromStdString(m_allBlocks[0].semester);
    QString firstMoed = QString::fromStdString(m_allBlocks[0].moed);
    
    // This function will now handle the Thread and Worker creation!
    generateForPeriod(firstSem, firstMoed);
}

void AppController::onSchedulingFinished(const std::vector<std::vector<int>>& solutions) {
    qDebug() << "--- FINAL ALGORITHM OUTPUT ---";
    qDebug() << "Total solutions calculated:" << solutions.size();
    setStatus("Scheduling completed!");


    // Filter the courses to only those that require exams, as the scheduling is based on exam periods
    std::vector<Course> examOnlyCourses;
    for (const auto& course : this->courses_) {
        if (course.getEvaluationMethod() == Evaluation::EXAM) {
            examOnlyCourses.push_back(course);
        }
    }

    // Expose the solutions to the output manager so it can prepare the data for the UI
    m_outputManager.setSchedulingData(solutions, examOnlyCourses, this->examPeriods_);
}

void AppController::onSchedulingFailed(QString message) {
    setError("Scheduling failed: " + message);
}

void AppController::generateForPeriod(const QString& semester, const QString& moed) {
    std::string targetSem = semester.toStdString();
    std::string targetMoed = moed.toStdString();

    SchedulingBlock* selectedBlock = nullptr;
    
    // search for the block that matches the selected semester and moed
    for (auto& block : m_allBlocks) {
        if (block.semester == targetSem && block.moed == targetMoed) {
            selectedBlock = &block;
            break;
        }
    }

    if (!selectedBlock) {
        qDebug() << "No block found for" << semester << moed;
        m_outputManager.setSchedulingData({}, courses_, examPeriods_);
        return;
    }

    setStatus("Generating schedules for " + semester + " " + moed + "...");

    //update the output manager with the current period filter so it can prepare the UI accordingly
    m_outputManager.setPeriodFilter(semester, moed);

    ScheduleGenerator* generator = new ScheduleGenerator(*selectedBlock);
    
    // Thread setup for background execution
    QThread* thread = new QThread();
    SchedulingWorker* worker = new SchedulingWorker(generator, 100000); 
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &SchedulingWorker::run);
    connect(worker, &SchedulingWorker::finished, this, &AppController::onSchedulingFinished);
    connect(worker, &SchedulingWorker::failed, this, &AppController::onSchedulingFailed);
    
    connect(worker, &SchedulingWorker::finished, thread, &QThread::quit);
    connect(worker, &SchedulingWorker::finished, worker, &SchedulingWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    
    thread->start();
}