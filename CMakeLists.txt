cmake_minimum_required(VERSION 3.16)

# Define the project name, version, and programming language.
project(ExamScheduler VERSION 4.0 LANGUAGES CXX)

# Use C++17 for the entire project.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Qt automatic meta-object compilation.
# This is required for QObject classes that use Q_OBJECT.
set(CMAKE_AUTOMOC ON)

# Enable automatic Qt resource compilation if resource files are added.
set(CMAKE_AUTORCC ON)

# Use the newer Qt resource policy when available.
if(COMMAND qt_policy)
    qt_policy(SET QTP0001 NEW)
endif()

# Locate the Qt6 modules required by the application.
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Concurrent)

# Define the main application executable and all source files compiled into it.
qt_add_executable(
    
    exam_scheduler
    src/main.cpp

    # Presentation layer: C++ objects exposed to QML.
    src/presentation/AppController.cpp
    src/presentation/ProgramCourseModel.cpp
    src/presentation/CalendarManager.cpp
    src/presentation/ScheduleOutputManager.cpp
    src/presentation/ScheduleOutputManager.h
    src/presentation/DragAndDropAdapter.cpp

    # Domain layer: core business objects and date logic.
    src/domain/Date.cpp
    src/domain/Course.cpp
    src/domain/ExamPeriod.cpp
    src/domain/ScheduleGenerationResult.cpp
    src/domain/DateAvailabilityPolicy.cpp
    

    # Application layer: central scheduling session state.
    src/application/SchedulingSession.cpp
    src/application/MoveValidationService.cpp

    # Infrastructure layer: file parsing and output writing.
    src/infrastructure/InputParser.cpp
    src/infrastructure/OutputWriter.cpp

    # Scheduling layer: preprocessing, solver, worker, service, and conflict rules.
    src/scheduling/Preprocessor.cpp
    src/scheduling/ScheduleGenerator.cpp
    src/scheduling/SchedulingWorker.cpp
    src/scheduling/SchedulingService.cpp
    src/scheduling/SameGroupConflictRule.cpp
    src/scheduling/AdvancedConflictRules.cpp
    src/scheduling/MetricsCalculator.cpp
)

# Allow source files to include headers relative to the src directory.
target_include_directories(exam_scheduler PRIVATE src)

# Configure compiler warning levels depending on the compiler.
if (MSVC)
    target_compile_options(exam_scheduler PRIVATE /W4 /Zc:__cplusplus)
else()
    target_compile_options(exam_scheduler PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Link the application against the required Qt libraries.
target_link_libraries(exam_scheduler
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Concurrent
)

# List all QML files that must be available next to the built executable.
set(QML_FILES
    qml/Main.qml
    qml/CalendarScreen.qml
    qml/OutputScreen.qml
    qml/PeriodEditorScreen.qml
    qml/SettingsScreen.qml
    qml/InputScreen.qml

    # Shared, reusable QML components (must be copied next to the executable so
    # the file-based implicit-directory import resolves them at runtime).
    qml/AppTheme.qml
    qml/AppButton.qml
    qml/CalendarGrid.qml
)

# Install QML files with the application.
install(FILES ${QML_FILES} DESTINATION .)

# Copy QML files to the build output directory after each build.
# This allows the executable to load QML files from applicationDirPath()/qml.
foreach(QML_FILE ${QML_FILES})
    get_filename_component(QML_DIR "${QML_FILE}" DIRECTORY)

    add_custom_command(TARGET exam_scheduler POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:exam_scheduler>/${QML_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/${QML_FILE}"
            "$<TARGET_FILE_DIR:exam_scheduler>/${QML_FILE}"
    )
endforeach()

# Enable CTest support for the project.
include(CTest)

# Add the tests subdirectory only when testing is enabled.
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
