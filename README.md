# Exam Scheduler V4.0

## Overview

Exam Scheduler is a desktop application for generating valid exam schedules for engineering study programs.

The system allows users to load course data and exam-period data, select relevant study programs, configure scheduling rules, generate valid exam schedules, review ranked schedule alternatives, manually inspect the calendar, and export a selected schedule to a readable text file.

The project is implemented in C++17 with a Qt 6 / QML graphical interface.

<img width="1920" height="1032" alt="image" src="https://github.com/user-attachments/assets/a54e68b8-a584-4037-8c36-f07c5c42b0e6" />


## Main Features

- Load course and exam-period input files.
- Select up to five study programs.
- View courses by program, year, and semester.
- Edit and inspect exam periods through an interactive calendar.
- Configure hard scheduling constraints and sorting priorities.
- Generate valid exam schedules asynchronously without blocking the UI.
- Browse generated schedules by period and moed.
- Sort schedules according to calculated quality metrics.
- Validate manual schedule changes against hard constraints.
- Export a selected schedule to a text file.

## Version 4.0 Highlights

Version 4.0 focuses on improving scheduling quality, runtime behavior, and internal architecture.

Key improvements include:

- Added configurable hard constraints for schedule generation.
- Added schedule metrics calculation and sorting support.
- Improved schedule output browsing and export behavior.
- Added validation for manual schedule changes.
- Refactored scheduling thread lifecycle management:
  - `SchedulingService` owns the active scheduling job lifecycle.
  - `SchedulingWorker` runs the scheduling operation on a background thread.
  - `ScheduleGenerator` remains independent from Qt thread management.
  - Cleanup, cancellation, shutdown, and failure paths are handled explicitly.
- Optimized `MetricsCalculator` runtime:
  - Prepared block data is calculated once per scheduling block.
  - Repeated runtime-course lookups were replaced with indexed lookups.
  - String-based date keys were replaced with integer date keys.
  - Repeated sorting and unnecessary container copies were reduced.
- Added and extended automated tests for scheduling, metrics, sorting, export, async lifecycle behavior, and regression coverage.

  <img width="1920" height="1032" alt="image" src="https://github.com/user-attachments/assets/2664b1f0-caa0-4b0f-af7d-deef7e8dd0bf" />


## Technologies

- C++17
- Qt 6
- Qt Quick / QML
- Qt Quick Controls 2
- Qt Concurrent
- CMake
- CTest

## Requirements

Before building the project, install:

- Git
- CMake 3.16 or newer
- A C++17-compatible compiler
- Qt 6.x

Required Qt modules:

- Qt Core
- Qt Gui
- Qt Qml
- Qt Quick
- Qt Quick Controls 2
- Qt Concurrent

## Build and Run

The project must be built with CMake. Do not compile it manually with `g++`, because the application depends on Qt, QML files, CMake auto-generated files, and post-build QML copying.

### Windows - Visual Studio / MSVC

Adjust the Qt path according to your local Qt installation.

```
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\msvc2022_64"
cmake --build build --config Debug
.\build\Debug\exam_scheduler.exe
```

### For a Release build:
```
cmake --build build --config Release
.\build\Release\exam_scheduler.exe
```

### If Qt DLLs are not found:
```
$env:Path = "C:\Qt\6.8.2\msvc2022_64\bin;$env:Path"
.\build\Debug\exam_scheduler.exe
```
### Windows - MinGW / Ninja
```
cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\mingw_64"
cmake --build build
.\build\exam_scheduler.exe
```
### Linux
```
sudo apt install build-essential cmake ninja-build qt6-base-dev qt6-declarative-dev qt6-tools-dev

cmake -S . -B build -G Ninja
cmake --build build
./build/exam_scheduler
```

### macOS
```
brew install qt cmake ninja

cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
cmake --build build
./build/exam_scheduler
```
## Running Tests
Configure the project with testing enabled:
```
cmake -S . -B build -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\msvc2022_64"
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```
Useful focused test commands:
```
ctest --test-dir build -C Debug -R MetricsCalculatorTest --output-on-failure
ctest --test-dir build -C Debug -R MetricsSortingIntegrationTest --output-on-failure
ctest --test-dir build -C Debug -R SchedulingServiceAsyncTest --output-on-failure
ctest --test-dir build -C Debug -R SchedulerIntegrationTest --output-on-failure
```
## Architecture Summary
The application is organized into clear layers:
* `domain` - core data models such as courses, dates, exam periods, and schedule results.
* `infrastructure` - input parsing and output writing.
* `scheduling` - preprocessing, constraint rules, schedule generation, metrics calculation, and async scheduling service.
* `presentation` - Qt/QML-facing controllers and managers.
* `qml` - user interface screens.
* `tests` - unit, integration, regression, GUI smoke, and performance-oriented tests.
Scheduling is performed in the background so the UI remains responsive. The scheduling algorithm itself is kept separate from thread management, which makes the code easier to test, maintain, and extend.

## Agile Workflow
The project was developed using an Agile-style workflow. Work was divided into focused stories, each targeting a specific feature, refactor, bug fix, or test improvement.
For each story, the team followed an incremental process:
* Define the scope and expected behavior.
* Implement the smallest safe change.
* Add or update automated tests.
* Run focused validation.
* Review the code and adjust the implementation when needed.
* Keep larger or riskier improvements outside the scope unless explicitly approved.
This approach helped reduce risk, keep the project stable before delivery, and make each change easier to review.
דs
