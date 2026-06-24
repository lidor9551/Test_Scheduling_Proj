#include "TestFactories.h"
#include "TestMacros.h"

#include <iostream>
#include <string>

int main() {
    Course examCourse = TestFactory::examCourse(
        "Calculus 1",
        "83112",
        "Dr. Erez Scheiner",
        "83101",
        Year::FIRST,
        Semester::FALL,
        Requirement::OBLIGATORY
    );

    TEST_EXPECT_EQ(examCourse.getCourseName(), std::string("Calculus 1"));
    TEST_EXPECT_EQ(examCourse.getCourseNumber(), std::string("83112"));
    TEST_EXPECT_EQ(examCourse.getInstructorName(), std::string("Dr. Erez Scheiner"));
    TEST_EXPECT_EQ(examCourse.getEvaluationMethod(), Evaluation::EXAM);
    TEST_EXPECT_TRUE(examCourse.requiresExam());

    examCourse.addProgram(
        "83102",
        Year::FIRST,
        Semester::FALL,
        Requirement::ELECTIVE
    );

    const auto& programs = examCourse.getPrograms();

    TEST_EXPECT_EQ(programs.size(), static_cast<std::size_t>(2));

    TEST_EXPECT_EQ(programs[0].programID, std::string("83101"));
    TEST_EXPECT_EQ(programs[0].year, Year::FIRST);
    TEST_EXPECT_EQ(programs[0].semester, Semester::FALL);
    TEST_EXPECT_EQ(programs[0].requirement, Requirement::OBLIGATORY);

    TEST_EXPECT_EQ(programs[1].programID, std::string("83102"));
    TEST_EXPECT_EQ(programs[1].year, Year::FIRST);
    TEST_EXPECT_EQ(programs[1].semester, Semester::FALL);
    TEST_EXPECT_EQ(programs[1].requirement, Requirement::ELECTIVE);

    Course projectCourse = TestFactory::projectCourse(
        "Software Project",
        "83533",
        "Dr. Terry Bell"
    );

    TEST_EXPECT_EQ(projectCourse.getEvaluationMethod(), Evaluation::PROJECT);
    TEST_EXPECT_FALSE(projectCourse.requiresExam());

    Course attendanceCourse = TestFactory::attendanceCourse(
        "Seminar Attendance",
        "83999",
        "Dr. Noa Gil"
    );

    TEST_EXPECT_EQ(attendanceCourse.getEvaluationMethod(), Evaluation::ATTENDANCE);
    TEST_EXPECT_FALSE(attendanceCourse.requiresExam());

    std::cout << "CourseModelTest passed." << std::endl;
    return EXIT_SUCCESS;
}