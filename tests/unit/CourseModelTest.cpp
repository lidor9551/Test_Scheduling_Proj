#include "domain/Course.h"

#include <cstdlib>
#include <iostream>
#include <string>

#define EXPECT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAILED: " << #condition \
                      << " at line " << __LINE__ << std::endl; \
            return EXIT_FAILURE; \
        } \
    } while (false)

#define EXPECT_EQ(actual, expected) \
    do { \
        if (!((actual) == (expected))) { \
            std::cerr << "FAILED: " << #actual << " == " << #expected \
                      << " at line " << __LINE__ << std::endl; \
            return EXIT_FAILURE; \
        } \
    } while (false)

int main() {
    Course examCourse(
        "Calculus 1",
        "83112",
        "Dr. Erez Scheiner",
        Evaluation::EXAM
    );

    EXPECT_EQ(examCourse.getCourseName(), std::string("Calculus 1"));
    EXPECT_EQ(examCourse.getCourseNumber(), std::string("83112"));
    EXPECT_EQ(examCourse.getInstructorName(), std::string("Dr. Erez Scheiner"));
    EXPECT_EQ(examCourse.getEvaluationMethod(), Evaluation::EXAM);
    EXPECT_TRUE(examCourse.requiresExam());

    examCourse.addProgram(
        "83101",
        1,
        Semester::FALL,
        Requirement::OBLIGATORY
    );

    examCourse.addProgram(
        "83102",
        1,
        Semester::FALL,
        Requirement::ELECTIVE
    );

    const auto& programs = examCourse.getPrograms();

    EXPECT_EQ(programs.size(), static_cast<std::size_t>(2));

    EXPECT_EQ(programs[0].programID, std::string("83101"));
    EXPECT_EQ(programs[0].year, 1);
    EXPECT_EQ(programs[0].semester, Semester::FALL);
    EXPECT_EQ(programs[0].requirement, Requirement::OBLIGATORY);

    EXPECT_EQ(programs[1].programID, std::string("83102"));
    EXPECT_EQ(programs[1].year, 1);
    EXPECT_EQ(programs[1].semester, Semester::FALL);
    EXPECT_EQ(programs[1].requirement, Requirement::ELECTIVE);

    Course projectCourse(
        "Software Project",
        "83533",
        "Dr. Terry Bell",
        Evaluation::PROJECT
    );

    EXPECT_EQ(projectCourse.getEvaluationMethod(), Evaluation::PROJECT);
    EXPECT_TRUE(!projectCourse.requiresExam());

    Course attendanceCourse(
        "Seminar Attendance",
        "83999",
        "Dr. Noa Gil",
        Evaluation::ATTENDANCE
    );

    EXPECT_EQ(attendanceCourse.getEvaluationMethod(), Evaluation::ATTENDANCE);
    EXPECT_TRUE(!attendanceCourse.requiresExam());

    std::cout << "CourseModelTest passed." << std::endl;
    return EXIT_SUCCESS;
}