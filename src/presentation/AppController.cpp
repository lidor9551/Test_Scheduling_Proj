#include "presentation/AppController.h"
#include "infrastructure/InputParser.h"
#include "scheduling/ScheduleGenerator.h" 
#include "scheduling/SchedulingWorker.h"
#include "scheduling/Preprocessor.h"
#include <QFileInfo>
#include <QUrl>
#include <QDir>
#include <string>
#include <utility>
#include <QThread>
#include <QVariant>


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
    return session_.courseCount();
}

int AppController::examPeriodCount() const {
    return session_.examPeriodCount();
}

bool AppController::hasData() const {
    return session_.hasData();
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

        session_.replaceData(std::move(loadedCourses), std::move(loadedPeriods));

        programCourseModel_.setCourses(session_.courses());
        m_outputManager.setCourses(session_.courses());
        m_outputManager.setProgramsMap(getInternalProgramsMap());

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

        SchedulingSession::AppendResult result =
            session_.appendData(loadedCourses, loadedPeriods);

        programCourseModel_.setCourses(session_.courses());
        m_outputManager.setCourses(session_.courses());
        m_outputManager.setProgramsMap(getInternalProgramsMap());

        emit dataChanged();

        setStatus(
            QString("Append completed. Added %1 courses, skipped %2 duplicate courses. Added %3 periods, skipped %4 duplicate periods.")
                .arg(result.addedCourses)
                .arg(result.skippedDuplicateCourses)
                .arg(result.addedExamPeriods)
                .arg(result.skippedDuplicateExamPeriods)
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

    for (const Course& c : session_.courses()) {
        list.append(QVariant::fromValue(c));
    }

    return list;
}

QVariantList AppController::getExamPeriodsVariant() const {
    QVariantList list;

    for (const ExamPeriod& p : session_.examPeriods()) {
        list.append(QVariant::fromValue(p));
    }

    return list;
}

QStringList AppController::availablePrograms() const {
    if (m_availablePrograms.isEmpty()) {
        return QStringList{"83101", "83102", "83103", "83104", "83105", "83107", "83108", "83109", "83115", "83182"};
    }
    return m_availablePrograms;
}

QStringList AppController::selectedPrograms() const {
    QStringList result;

    for (const std::string& programId : session_.selectedPrograms()) {
        result.append(QString::fromStdString(programId));
    }

    return result;
}

void AppController::toggleProgram(const QString& programId) {
    const std::string id = programId.toStdString();

    bool changed = false;

    if (session_.isProgramSelected(id)) {
        changed = session_.deselectProgram(id);
    } else {
        changed = session_.selectProgram(id);

        if (!changed) {
            qDebug() << "Cannot select more than"
                     << static_cast<int>(SchedulingSession::MaxSelectedPrograms)
                     << "programs.";
        }
    }

    if (changed) {
        emit selectedProgramsChanged();
    }
}

QVariantList AppController::getCoursesForProgram(const QString& programId, int year, int semester) {
    QVariantList courseList;
    std::string pid = programId.toStdString();

    for (const auto& course : session_.courses()) {
        const auto& programs = course.getPrograms();
        
        for (const auto& p : programs) {
            // basic filtering by program ID
            bool match = (p.programID == pid);
            
            // if year or semester are specified (not -1), also filter by them
            if (year != -1) match &= (yearToInt(p.year) == year);
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
    qDebug() << "Selected programs count:" << session_.selectedProgramCount();
        setStatus("Generating schedules...");

    // override original output with new instance to reset previous state
    if (m_calendarManager && !m_calendarManager->getPeriods().empty()) {
        m_calendarManager->saveChanges();
        session_.replaceExamPeriods(m_calendarManager->getPeriods());
        qDebug() << "[SYNC] Successfully synced updated periods from CalendarManager.";
    } else {
        qDebug() << "[SYNC] CalendarManager is empty or not ready. Using original parsed periods.";
    }

    // Keep a reference to the full course list for preprocessing
    const std::vector<Course>& allCourses = session_.courses();
    const std::vector<ExamPeriod>& examPeriods = session_.examPeriods();

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
    for (const std::string& prog : session_.selectedPrograms()) {
        selectedProgs.push_back(prog);
    }

    // Build all blocks and save them to the class member (m_allBlocks)
    SchedulingPreprocessor preprocessor(allCourses, examPeriods, selectedProgs);
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
    for (const auto& course : session_.courses()) {
        if (course.getEvaluationMethod() == Evaluation::EXAM) {
            examOnlyCourses.push_back(course);
        }
    }

    // Expose the solutions to the output manager so it can prepare the data for the UI
    m_outputManager.setSchedulingData(solutions, examOnlyCourses, session_.examPeriods());
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
        m_outputManager.setSchedulingData({}, session_.courses(), session_.examPeriods());
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

// C++ function to create the internal map of program IDs to names
QMap<QString, QString> AppController::getInternalProgramsMap() const {
    QMap<QString, QString> map;
    map["83101"] = QStringLiteral("הנדסת מחשבים");
    map["83102"] = QStringLiteral("הנדסת חשמל");
    map["83104"] = QStringLiteral("הנדסת תעשיה ומערכות מידע");
    map["83107"] = QStringLiteral("הנדסת נתונים");
    map["83108"] = QStringLiteral("הנדסת תוכנה");
    map["83109"] = QStringLiteral("הנדסת חומרים");
    map["83105"] = QStringLiteral("הנדסת מחשבים – מגמת חומרת מחשבים");
    map["83182"] = QStringLiteral("הנדסת חשמל – מגמת הנדסה קוונטית");
    map["83103"] = QStringLiteral("הנדסת חשמל – מגמת נוירו הנדסה");
    map["83115"] = QStringLiteral("הנדסת חשמל – מגמת הנדסה ביו-רפואית");
    return map;
}

// QML function to expose the program names map for display purposes (calls the C++ function and converts it to QVariantMap)
QVariantMap AppController::getProgramNamesMap() const {
    QVariantMap vMap;
    QMap<QString, QString> cppMap = getInternalProgramsMap();
    
    for (auto it = cppMap.constBegin(); it != cppMap.constEnd(); ++it) {
        vMap.insert(it.key(), it.value());
    }
    return vMap;
}


