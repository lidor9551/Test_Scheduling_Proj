add_executable(scheduling_session_test
    unit/SchedulingSessionTest.cpp
    ../src/application/SchedulingSession.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/Date.cpp
)

target_include_directories(scheduling_session_test PRIVATE ../src)

target_link_libraries(scheduling_session_test
    PRIVATE
        Qt6::Core
)

add_test(
    NAME SchedulingSessionTest
    COMMAND scheduling_session_test
)

add_executable(program_course_model_test
    unit/ProgramCourseModelTest.cpp
    ../src/presentation/ProgramCourseModel.cpp
    ../src/domain/Course.cpp
    ../src/domain/ScheduleGenerationResult.cpp
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
    ../src/infrastructure/InputParser.cpp
    ../src/scheduling/Preprocessor.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
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
    ../src/infrastructure/InputParser.cpp
    ../src/scheduling/Preprocessor.cpp
    ../src/scheduling/ScheduleGenerator.cpp
    ../src/scheduling/SchedulingWorker.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/ScheduleGenerationResult.cpp
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
    ../src/infrastructure/InputParser.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
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

add_executable(no_solution_integration_test
    integration/NoSolutionIntegrationTest.cpp
    ../src/infrastructure/InputParser.cpp
    ../src/scheduling/Preprocessor.cpp
    ../src/scheduling/ScheduleGenerator.cpp
    ../src/scheduling/SchedulingWorker.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/ScheduleGenerationResult.cpp
)

target_include_directories(no_solution_integration_test PRIVATE ../src)

target_link_libraries(no_solution_integration_test
    PRIVATE
        Qt6::Core
)

add_test(
    NAME NoSolutionIntegrationTest
    COMMAND no_solution_integration_test
)

set_tests_properties(NoSolutionIntegrationTest
    PROPERTIES
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_executable(overlap_conflict_integration_test
    integration/OverlapConflictIntegrationTest.cpp
    ../src/infrastructure/InputParser.cpp
    ../src/scheduling/Preprocessor.cpp
    ../src/scheduling/ScheduleGenerator.cpp
    ../src/scheduling/SchedulingWorker.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/ScheduleGenerationResult.cpp
)

target_include_directories(overlap_conflict_integration_test PRIVATE ../src)

target_link_libraries(overlap_conflict_integration_test
    PRIVATE
        Qt6::Core
)

add_test(
    NAME OverlapConflictIntegrationTest
    COMMAND overlap_conflict_integration_test
)

set_tests_properties(OverlapConflictIntegrationTest
    PROPERTIES
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)