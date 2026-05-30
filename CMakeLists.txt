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

set(CMAKE_PREFIX_PATH "C:/Users/user/OneDrive/Desktop/6.7.2/msvc2019_64")

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_add_executable(exam_scheduler
    src/main.cpp
    src/dates/Date.cpp
    src/model/Course.cpp
    src/model/ExamPeriod.cpp
    src/parser/InputParser.cpp
    src/preprocessing/Preprocessor.cpp
    src/scheduler/ScheduleGenerator.cpp
    src/exporter/OutputWriter.cpp
)

target_include_directories(exam_scheduler PRIVATE src)

if (MSVC)
    target_compile_options(exam_scheduler PRIVATE /W4 /Zc:__cplusplus)
else()
    target_compile_options(exam_scheduler PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(exam_scheduler PRIVATE Qt6::Quick)

install(FILES Main.qml DESTINATION .)

add_custom_command(TARGET exam_scheduler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Main.qml"
        "$<TARGET_FILE_DIR:exam_scheduler>/Main.qml"
)