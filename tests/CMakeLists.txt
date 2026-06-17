# tests/CMakeLists.txt
#
# Centralized test configuration for Exam Scheduler.

# ---------------------------------------------------------------------------
# Resolve Qt runtime directory for Windows CTest runs
# ---------------------------------------------------------------------------

set(EXAM_SCHEDULER_TEST_ENV "")

if(WIN32)
    get_target_property(QT_CORE_LOCATION Qt6::Core IMPORTED_LOCATION_DEBUG)

    if(NOT QT_CORE_LOCATION)
        get_target_property(QT_CORE_LOCATION Qt6::Core IMPORTED_LOCATION_RELEASE)
    endif()

    if(QT_CORE_LOCATION)
        get_filename_component(QT_RUNTIME_DIR "${QT_CORE_LOCATION}" DIRECTORY)
        set(EXAM_SCHEDULER_TEST_ENV "PATH=${QT_RUNTIME_DIR};$ENV{PATH}")
        message(STATUS "Qt runtime directory for tests: ${QT_RUNTIME_DIR}")
    else()
        message(WARNING "Could not resolve Qt6::Core runtime directory. CTest may fail on Windows if Qt DLLs are not in PATH.")
    endif()
endif()


# ---------------------------------------------------------------------------
# Common helper for all C++ tests
# ---------------------------------------------------------------------------

function(add_exam_scheduler_test TARGET_NAME TEST_NAME TEST_LABEL)
    add_executable(${TARGET_NAME}
        ${ARGN}
    )

    target_include_directories(${TARGET_NAME}
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/../src
            ${CMAKE_CURRENT_LIST_DIR}/common
    )

    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Qt6::Core
    )

    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TARGET_NAME}
    )

    set_tests_properties(${TEST_NAME}
        PROPERTIES
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            LABELS "${TEST_LABEL}"
    )

    if(WIN32 AND EXAM_SCHEDULER_TEST_ENV)
        set_tests_properties(${TEST_NAME}
            PROPERTIES
                ENVIRONMENT "${EXAM_SCHEDULER_TEST_ENV}"
        )
    endif()
endfunction()


# ---------------------------------------------------------------------------
# Unit tests — Domain layer
# ---------------------------------------------------------------------------

add_exam_scheduler_test(course_model_test CourseModelTest "Unit;Domain"
    unit/domain/CourseModelTest.cpp
    ../src/domain/Course.cpp
)

add_exam_scheduler_test(date_and_exam_period_test DateAndExamPeriodTest "Unit;Domain"
    unit/domain/DateAndExamPeriodTest.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/DateAvailabilityPolicy.cpp
)

add_exam_scheduler_test(date_availability_policy_test DateAvailabilityPolicyTest "Unit;Domain"
    unit/domain/DateAvailabilityPolicyTest.cpp
    ../src/domain/DateAvailabilityPolicy.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
)

add_exam_scheduler_test(schedule_generation_result_test ScheduleGenerationResultTest "Unit;Domain"
    unit/domain/ScheduleGenerationResultTest.cpp
    ../src/domain/ScheduleGenerationResult.cpp
    ../src/domain/Course.cpp
    ../src/domain/Date.cpp
)


# ---------------------------------------------------------------------------
# Unit tests — Application layer
# ---------------------------------------------------------------------------

add_exam_scheduler_test(scheduling_session_test SchedulingSessionTest "Unit;Application"
    unit/application/SchedulingSessionTest.cpp
    ../src/application/SchedulingSession.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/DateAvailabilityPolicy.cpp
    ../src/domain/Date.cpp
)


# ---------------------------------------------------------------------------
# Unit tests — Scheduling layer
# ---------------------------------------------------------------------------

add_exam_scheduler_test(same_group_conflict_rule_test SameGroupConflictRuleTest "Unit;Scheduling"
    unit/scheduling/SameGroupConflictRuleTest.cpp
    ../src/scheduling/SameGroupConflictRule.cpp
)


# ---------------------------------------------------------------------------
# Unit tests — Presentation layer
# ---------------------------------------------------------------------------

add_exam_scheduler_test(program_course_model_test ProgramCourseModelTest "Unit;Presentation"
    unit/presentation/ProgramCourseModelTest.cpp
    ../src/presentation/ProgramCourseModel.cpp
    ../src/domain/Course.cpp
    ../src/domain/ScheduleGenerationResult.cpp
)

add_exam_scheduler_test(schedule_output_manager_export_safety_test ScheduleOutputManagerExportSafetyTest "Unit;Presentation"
    unit/presentation/ScheduleOutputManagerExportSafetyTest.cpp
    ../src/presentation/ScheduleOutputManager.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/Date.cpp
    ../src/domain/DateAvailabilityPolicy.cpp
    ../src/domain/ScheduleGenerationResult.cpp
)


# ---------------------------------------------------------------------------
# Integration tests — Parser + Preprocessor
# ---------------------------------------------------------------------------

add_exam_scheduler_test(parser_preprocessor_integration_test ParserPreprocessorIntegrationTest "Integration;Parser;Preprocessor"
    integration/parser_preprocessor/ParserPreprocessorIntegrationTest.cpp
    ../src/infrastructure/InputParser.cpp
    ../src/scheduling/Preprocessor.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/DateAvailabilityPolicy.cpp
)


# ---------------------------------------------------------------------------
# Integration tests — Full scheduling pipeline
# ---------------------------------------------------------------------------

add_exam_scheduler_test(scheduler_integration_test SchedulerIntegrationTest "Integration;Scheduling"
    integration/scheduling_pipeline/SchedulerIntegrationTest.cpp
    ../src/infrastructure/InputParser.cpp
    ../src/scheduling/Preprocessor.cpp
    ../src/scheduling/ScheduleGenerator.cpp
    ../src/scheduling/SchedulingWorker.cpp
    ../src/scheduling/SameGroupConflictRule.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/DateAvailabilityPolicy.cpp
    ../src/domain/ScheduleGenerationResult.cpp
)


# ---------------------------------------------------------------------------
# Integration tests — Invalid input
# ---------------------------------------------------------------------------

add_exam_scheduler_test(invalid_input_integration_test InvalidInputIntegrationTest "Integration;InputValidation"
    integration/input_validation/InvalidInputIntegrationTest.cpp
    ../src/infrastructure/InputParser.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/DateAvailabilityPolicy.cpp
)


# ---------------------------------------------------------------------------
# Integration tests — No solution scenarios
# ---------------------------------------------------------------------------

add_exam_scheduler_test(no_solution_integration_test NoSolutionIntegrationTest "Integration;NoSolution"
    integration/no_solution/NoSolutionIntegrationTest.cpp
    ../src/infrastructure/InputParser.cpp
    ../src/scheduling/Preprocessor.cpp
    ../src/scheduling/ScheduleGenerator.cpp
    ../src/scheduling/SameGroupConflictRule.cpp
    ../src/scheduling/SchedulingWorker.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/DateAvailabilityPolicy.cpp
    ../src/domain/ScheduleGenerationResult.cpp
)


# ---------------------------------------------------------------------------
# Integration tests — Conflict scenarios
# ---------------------------------------------------------------------------

add_exam_scheduler_test(overlap_conflict_integration_test OverlapConflictIntegrationTest "Integration;Conflicts"
    integration/conflicts/OverlapConflictIntegrationTest.cpp
    ../src/infrastructure/InputParser.cpp
    ../src/scheduling/Preprocessor.cpp
    ../src/scheduling/ScheduleGenerator.cpp
    ../src/scheduling/SameGroupConflictRule.cpp
    ../src/scheduling/SchedulingWorker.cpp
    ../src/domain/Date.cpp
    ../src/domain/Course.cpp
    ../src/domain/ExamPeriod.cpp
    ../src/domain/DateAvailabilityPolicy.cpp
    ../src/domain/ScheduleGenerationResult.cpp
)