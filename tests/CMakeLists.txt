add_executable(program_course_model_test
    ProgramCourseModelTest.cpp
    ../src/gui/ProgramCourseModel.cpp
    ../src/model/Course.cpp
)

target_include_directories(program_course_model_test PRIVATE ../src)

target_link_libraries(program_course_model_test
    PRIVATE
        Qt6::Core
)

add_test(
    NAME ProgramCourseModelTest
    COMMAND program_course_model_test
)

add_executable(parser_preprocessor_integration_test
    integration/ParserPreprocessorIntegrationTest.cpp
    ../src/parser/InputParser.cpp
    ../src/preprocessing/Preprocessor.cpp
    ../src/dates/Date.cpp
    ../src/model/Course.cpp
    ../src/model/ExamPeriod.cpp
)

target_include_directories(parser_preprocessor_integration_test PRIVATE ../src)

target_link_libraries(parser_preprocessor_integration_test
    PRIVATE
        Qt6::Core
)

add_test(
    NAME ParserPreprocessorIntegrationTest
    COMMAND parser_preprocessor_integration_test
)

set_tests_properties(ParserPreprocessorIntegrationTest
    PROPERTIES
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_executable(scheduler_integration_test
    integration/SchedulerIntegrationTest.cpp
    ../src/parser/InputParser.cpp
    ../src/preprocessing/Preprocessor.cpp
    ../src/scheduler/ScheduleGenerator.cpp
    ../src/scheduler/SchedulingWorker.cpp
    ../src/dates/Date.cpp
    ../src/model/Course.cpp
    ../src/model/ExamPeriod.cpp
)

target_include_directories(scheduler_integration_test PRIVATE ../src)

target_link_libraries(scheduler_integration_test
    PRIVATE
        Qt6::Core
)

add_test(
    NAME SchedulerIntegrationTest
    COMMAND scheduler_integration_test
)

set_tests_properties(SchedulerIntegrationTest
    PROPERTIES
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_executable(invalid_input_integration_test
    integration/InvalidInputIntegrationTest.cpp
    ../src/parser/InputParser.cpp
    ../src/dates/Date.cpp
    ../src/model/Course.cpp
    ../src/model/ExamPeriod.cpp
)

target_include_directories(invalid_input_integration_test PRIVATE ../src)

target_link_libraries(invalid_input_integration_test
    PRIVATE
        Qt6::Core
)

add_test(
    NAME InvalidInputIntegrationTest
    COMMAND invalid_input_integration_test
)

set_tests_properties(InvalidInputIntegrationTest
    PROPERTIES
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)