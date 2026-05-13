cmake_minimum_required(VERSION 3.16)
project(ExamScheduler VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(exam_scheduler
    src/main.cpp
    src/Date.cpp
    src/Domain.cpp
    src/Parser.cpp
    src/Preprocessor.cpp
    src/Scheduler.cpp
    src/OutputWriter.cpp
)

target_include_directories(exam_scheduler PRIVATE include)

if (MSVC)
    target_compile_options(exam_scheduler PRIVATE /W4)
else()
    target_compile_options(exam_scheduler PRIVATE -Wall -Wextra -Wpedantic)
endif()
