#include "scheduling/SchedulingService.h"
#include "TestMacros.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QMetaType>
#include <QObject>
#include <QTimer>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

Course makeExamCourse(
    const std::string& name,
    const std::string& number
) {
    Course course(name, number, "Dr. Async", Evaluation::EXAM);
    course.addProgram(
        "83101",
        Year::FIRST,
        Semester::FALL,
        Requirement::OBLIGATORY
    );

    return course;
}

SchedulingBlock makeSingleCourseBlock(const Course& course) {
    RuntimeCourse runtimeCourse;
    runtimeCourse.id = 0;
    runtimeCourse.course = &course;
    runtimeCourse.memberships = {
        CourseMembership{0, Requirement::OBLIGATORY}
    };

    SchedulingBlock block;
    block.semester = "FALL";
    block.moed = "Aleph";
    block.allowedDates = {
        Date(1, 1, 2026)
    };
    block.runtimeCourses = {
        runtimeCourse
    };

    return block;
}

SchedulingBlock makeNoSolutionBlock(const Course& course) {
    RuntimeCourse runtimeCourse;
    runtimeCourse.id = 0;
    runtimeCourse.course = &course;
    runtimeCourse.memberships = {
        CourseMembership{0, Requirement::OBLIGATORY}
    };

    SchedulingBlock block;
    block.semester = "FALL";
    block.moed = "Aleph";
    block.allowedDates = {};
    block.runtimeCourses = {
        runtimeCourse
    };

    return block;
}

SchedulingBlock makeLargeSearchBlock(const std::vector<Course>& courses) {
    SchedulingBlock block;
    block.semester = "FALL";
    block.moed = "Aleph";

    for (int day = 1; day <= static_cast<int>(courses.size()); ++day) {
        block.allowedDates.push_back(Date(day, 1, 2026));
    }

    for (std::size_t index = 0; index < courses.size(); ++index) {
        RuntimeCourse runtimeCourse;
        runtimeCourse.id = static_cast<int>(index);
        runtimeCourse.course = &courses[index];
        runtimeCourse.memberships = {
            CourseMembership{0, Requirement::OBLIGATORY}
        };

        block.runtimeCourses.push_back(runtimeCourse);
    }

    return block;
}

ScheduleSettings defaultSettings() {
    return ScheduleSettings{};
}

struct AsyncResult {
    bool finished = false;
    bool failed = false;
    bool timedOut = false;

    std::vector<ScheduleGenerationResult> solutions;
    QString failureMessage;
};

void waitForSingleAsyncRun(
    SchedulingService& service,
    const SchedulingBlock& block,
    const ScheduleSettings& settings,
    int limitPerBlock,
    AsyncResult& result
) {
    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    QObject::connect(
        &service,
        &SchedulingService::generationFinished,
        &loop,
        [&](const std::vector<ScheduleGenerationResult>& solutions) {
            result.finished = true;
            result.solutions = solutions;
            loop.quit();
        }
    );

    QObject::connect(
        &service,
        &SchedulingService::generationFailed,
        &loop,
        [&](QString message) {
            result.failed = true;
            result.failureMessage = message;
            loop.quit();
        }
    );

    QObject::connect(
        &timeoutTimer,
        &QTimer::timeout,
        &loop,
        [&]() {
            result.timedOut = true;
            loop.quit();
        }
    );

    service.startAsyncGeneration(block, settings, limitPerBlock);

    timeoutTimer.start(3000);

    if (!result.finished && !result.failed) {
        loop.exec();
    }

    timeoutTimer.stop();

    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}

void testAsyncGenerationEmitsFinishedForSimpleBlock() {
    Course course = makeExamCourse("Algorithms", "89101");

    SchedulingBlock block = makeSingleCourseBlock(course);
    ScheduleSettings settings = defaultSettings();

    SchedulingService service;
    AsyncResult result;

    waitForSingleAsyncRun(service, block, settings, 1, result);

    TEST_EXPECT_FALSE(result.timedOut);
    TEST_EXPECT_TRUE(result.finished);
    TEST_EXPECT_FALSE(result.failed);

    TEST_EXPECT_EQ(result.solutions.size(), static_cast<std::size_t>(1));

    const ScheduleGenerationResult& solution = result.solutions[0];
    const std::vector<ExamAssignment>& assignments = solution.getAssignments();

    TEST_EXPECT_EQ(assignments.size(), static_cast<std::size_t>(1));
    TEST_EXPECT_TRUE(assignments[0].course != nullptr);
    TEST_EXPECT_EQ(assignments[0].course->getCourseNumber(), std::string("89101"));
    TEST_EXPECT_EQ(assignments[0].examDate, Date(1, 1, 2026));
    TEST_EXPECT_TRUE(assignments[0].isObligatory);
}

void testAsyncGenerationCanReturnEmptySolutionListWhenNoDatesExist() {
    Course course = makeExamCourse("Algorithms", "89101");

    SchedulingBlock block = makeNoSolutionBlock(course);
    ScheduleSettings settings = defaultSettings();

    SchedulingService service;
    AsyncResult result;

    waitForSingleAsyncRun(service, block, settings, 1, result);

    TEST_EXPECT_FALSE(result.timedOut);
    TEST_EXPECT_TRUE(result.finished);
    TEST_EXPECT_FALSE(result.failed);

    TEST_EXPECT_TRUE(result.solutions.empty());
}

void testConcurrentStartEmitsAlreadyRunningFailure() {
    Course course = makeExamCourse("Algorithms", "89101");

    SchedulingBlock block = makeSingleCourseBlock(course);
    ScheduleSettings settings = defaultSettings();

    SchedulingService service;

    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    int finishedCount = 0;
    int failedCount = 0;
    QString failureMessage;
    bool timedOut = false;

    QObject::connect(
        &service,
        &SchedulingService::generationFinished,
        &loop,
        [&](const std::vector<ScheduleGenerationResult>& solutions) {
            (void)solutions;
            ++finishedCount;
            loop.quit();
        }
    );

    QObject::connect(
        &service,
        &SchedulingService::generationFailed,
        &loop,
        [&](QString message) {
            ++failedCount;
            failureMessage = message;
        }
    );

    QObject::connect(
        &timeoutTimer,
        &QTimer::timeout,
        &loop,
        [&]() {
            timedOut = true;
            loop.quit();
        }
    );

    service.startAsyncGeneration(block, settings, 1);

    // This call happens immediately, before the first async run can reset
    // the internal running flag on the main event loop.
    service.startAsyncGeneration(block, settings, 1);

    timeoutTimer.start(3000);

    if (finishedCount == 0) {
        loop.exec();
    }

    timeoutTimer.stop();

    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    TEST_EXPECT_FALSE(timedOut);

    TEST_EXPECT_EQ(failedCount, 1);
    TEST_EXPECT_TRUE(failureMessage.contains("already running"));

    TEST_EXPECT_EQ(finishedCount, 1);
}

void testCancellationEmitsFailureForActiveRun() {
    std::vector<Course> courses;
    courses.reserve(10);

    for (int index = 0; index < 10; ++index) {
        courses.push_back(makeExamCourse(
            "Cancellation Course " + std::to_string(index),
            "89" + std::to_string(200 + index)
        ));
    }

    SchedulingBlock block = makeLargeSearchBlock(courses);
    ScheduleSettings settings = defaultSettings();

    SchedulingService service;

    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    bool finished = false;
    bool failed = false;
    bool timedOut = false;
    QString failureMessage;

    QObject::connect(
        &service,
        &SchedulingService::generationFinished,
        &loop,
        [&](const std::vector<ScheduleGenerationResult>& solutions) {
            (void)solutions;
            finished = true;
            loop.quit();
        }
    );

    QObject::connect(
        &service,
        &SchedulingService::generationFailed,
        &loop,
        [&](QString message) {
            failed = true;
            failureMessage = message;
            loop.quit();
        }
    );

    QObject::connect(
        &timeoutTimer,
        &QTimer::timeout,
        &loop,
        [&]() {
            timedOut = true;
            loop.quit();
        }
    );

    service.startAsyncGeneration(block, settings, -1);
    QTimer::singleShot(0, &service, &SchedulingService::cancelActiveGeneration);

    timeoutTimer.start(3000);

    if (!finished && !failed) {
        loop.exec();
    }

    timeoutTimer.stop();

    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    TEST_EXPECT_FALSE(timedOut);
    TEST_EXPECT_FALSE(finished);
    TEST_EXPECT_TRUE(failed);
    TEST_EXPECT_TRUE(failureMessage.contains("cancelled"));
}

void testDestructorShutsDownActiveRunWithoutCrash() {
    std::vector<Course> courses;
    courses.reserve(10);

    for (int index = 0; index < 10; ++index) {
        courses.push_back(makeExamCourse(
            "Destructor Course " + std::to_string(index),
            "89" + std::to_string(300 + index)
        ));
    }

    SchedulingBlock block = makeLargeSearchBlock(courses);
    ScheduleSettings settings = defaultSettings();

    {
        SchedulingService service;
        service.startAsyncGeneration(block, settings, -1);
        service.cancelActiveGeneration();
    }

    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    TEST_EXPECT_TRUE(true);
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    qRegisterMetaType<std::vector<ScheduleGenerationResult>>(
        "std::vector<ScheduleGenerationResult>"
    );

    testAsyncGenerationEmitsFinishedForSimpleBlock();
    testAsyncGenerationCanReturnEmptySolutionListWhenNoDatesExist();
    testConcurrentStartEmitsAlreadyRunningFailure();
    testCancellationEmitsFailureForActiveRun();
    testDestructorShutsDownActiveRunWithoutCrash();

    std::cout << "SchedulingServiceAsyncTest passed." << std::endl;
    return EXIT_SUCCESS;
}
