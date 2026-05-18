cmake_minimum_required(VERSION 3.16)
project(ExamScheduler VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(exam_scheduler
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
    target_compile_options(exam_scheduler PRIVATE /W4)
else()
    target_compile_options(exam_scheduler PRIVATE -Wall -Wextra -Wpedantic)
endif()