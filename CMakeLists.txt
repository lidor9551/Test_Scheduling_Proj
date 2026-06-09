cmake_minimum_required(VERSION 3.16)

project(ExamScheduler VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(COMMAND qt_policy)
    qt_policy(SET QTP0001 NEW)
endif()

# CHANGEMENT: Ajout de QuickControls2 à la liste des composants requis
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Concurrent)

qt_add_executable(exam_scheduler
    src/main.cpp
    src/gui/AppController.cpp
    src/gui/ProgramCourseModel.cpp
    src/gui/CalendarManager.cpp
    src/dates/Date.cpp
    src/model/Course.cpp
    src/model/ExamPeriod.cpp
    src/parser/InputParser.cpp
    src/preprocessing/Preprocessor.cpp
    src/scheduler/ScheduleGenerator.cpp
    src/scheduler/SchedulingWorker.cpp
    src/exporter/OutputWriter.cpp
)

target_include_directories(exam_scheduler PRIVATE src)

if (MSVC)
    target_compile_options(exam_scheduler PRIVATE /W4 /Zc:__cplusplus)
else()
    target_compile_options(exam_scheduler PRIVATE -Wall -Wextra -Wpedantic)
endif()

# CHANGEMENT: Ajout de Qt6::QuickControls2 aux bibliothèques liées
target_link_libraries(exam_scheduler
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Concurrent
)

set(QML_FILES
    Main.qml
    CalendarScreen.qml
    PeriodEditorScreen.qml
)

install(FILES ${QML_FILES} DESTINATION .)

foreach(QML_FILE ${QML_FILES})
    add_custom_command(TARGET exam_scheduler POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/${QML_FILE}"
            "$<TARGET_FILE_DIR:exam_scheduler>/${QML_FILE}"
    )
endforeach()

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()