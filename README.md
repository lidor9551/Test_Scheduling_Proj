# Exam Scheduler V34.0

## Overview

Exam Scheduler V4.0 is a desktop application for generating valid exam schedules for selected engineering study programs.

The system allows the user to:

- Load course and exam-period input files.
- Select up to five study programs.
- View courses by program, year, and semester.
- Edit exam periods through an interactive calendar.
- Configure scheduling settings, including hard constraints and sorting priorities.
- Generate valid exam schedules.
- Browse generated schedules.
- Export a selected schedule to a readable text file.

The project is implemented in **C++17** with a **Qt6/QML** graphical interface.

---

## Technologies

- C++17
- Qt 6
- Qt Quick / QML
- Qt Quick Controls 2
- Qt Concurrent
- CMake
- CTest

---

## Project structure:

├── qml/
│   ├── Main.qml
│   ├── CalendarScreen.qml
│   ├── PeriodEditorScreen.qml
│   ├── OutputScreen.qml
│   └── SettingsScreen.qml
│
├── src/
│   ├── main.cpp
│   │
│   ├── domain/
│   │   ├── Course.h / Course.cpp
│   │   ├── ExamPeriod.h / ExamPeriod.cpp
│   │   ├── Date.h / Date.cpp
│   │   ├── DateAvailabilityPolicy.h / DateAvailabilityPolicy.cpp
│   │   └── ScheduleGenerationResult.h / ScheduleGenerationResult.cpp
│   │
│   ├── application/
│   │   └── SchedulingSession.h / SchedulingSession.cpp   (central session state)
│   │
│   ├── scheduling/
│   │   ├── Preprocessor.h / Preprocessor.cpp
│   │   ├── ScheduleGenerator.h / ScheduleGenerator.cpp
│   │   ├── SchedulingWorker.h / SchedulingWorker.cpp
│   │   ├── SchedulingService.h / SchedulingService.cpp
│   │   ├── MetricsCalculator.h / MetricsCalculator.cpp
│   │   ├── SameGroupConflictRule.h / SameGroupConflictRule.cpp
│   │   ├── AdvancedConflictRules.h / AdvancedConflictRules.cpp
│   │   ├── IConflictRule.h
│   │   ├── IReadOnlySchedule.h
│   │   └── SchedulingState.h
│   │
│   ├── presentation/
│   │   ├── AppController.h / AppController.cpp
│   │   ├── CalendarManager.h / CalendarManager.cpp
│   │   ├── ProgramCourseModel.h / ProgramCourseModel.cpp
│   │   ├── ScheduleOutputManager.h / ScheduleOutputManager.cpp
│   │   └── DragAndDropAdapter.h / DragAndDropAdapter.cpp
│   │
│   ├── infrastructure/
│   │   ├── InputParser.h / InputParser.cpp
│   │   └── OutputWriter.h / OutputWriter.cpp
│   │
│   └── utils/
│       └── Utils.h
│
└── tests/
    ├── unit/
    ├── integration/
    ├── gui/
    ├── regression/
    ├── layout/
    ├── manual/
    ├── reports/
    ├── common/
    └── data/

---

## Requirements

Before building the project, install:

- Git
- CMake 3.16 or newer
- C++17-compatible compiler
- Qt 6.x

Required Qt modules:

- Qt Core
- Qt Gui
- Qt Qml
- Qt Quick
- Qt Quick Controls 2
- Qt Concurrent

---

## Clone the Repository

```bash
git clone https://github.com/lidor9551/Test_Scheduling_Proj.git
cd Test_Scheduling_Proj
```

---

# Build and Run with CMake

The project must be built using **CMake**.

The generated executable is:

```text
exam_scheduler
```

On Windows:

```text
exam_scheduler.exe
```

Do not compile the project manually with `g++`, because the project uses Qt, QML, CMake auto-generated files, and post-build QML file copying.

---

## Windows — MinGW / Ninja

Adjust the Qt path according to your installed Qt version.

Example Qt path:

```powershell
C:\Qt\6.8.2\mingw_64
```

### Configure

```powershell
cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\mingw_64"
```

### Build

```powershell
cmake --build build
```

### Run

```powershell
.\build\exam_scheduler.exe
```

### If Qt DLLs are not found

```powershell
$env:Path = "C:\Qt\6.8.2\mingw_64\bin;$env:Path"
.\build\exam_scheduler.exe
```

---

## Windows — MSVC

Adjust the Qt path according to your installed Qt version.

Example Qt path:

```powershell
C:\Qt\6.8.2\msvc2022_64
```

### Configure

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\msvc2022_64"
```

### Build Debug

```powershell
cmake --build build --config Debug
```

### Run Debug

```powershell
.\build\Debug\exam_scheduler.exe
```

### Build Release

```powershell
cmake --build build --config Release
```

### Run Release

```powershell
.\build\Release\exam_scheduler.exe
```

### If Qt DLLs are not found

For Debug:

```powershell
$env:Path = "C:\Qt\6.8.2\msvc2022_64\bin;$env:Path"
.\build\Debug\exam_scheduler.exe
```

For Release:

```powershell
$env:Path = "C:\Qt\6.8.2\msvc2022_64\bin;$env:Path"
.\build\Release\exam_scheduler.exe
```

---

## Linux

Install dependencies:

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build qt6-base-dev qt6-declarative-dev qt6-tools-dev
```

### Configure

```bash
cmake -S . -B build -G Ninja
```

### Build

```bash
cmake --build build
```

### Run

```bash
./build/exam_scheduler
```

---

## macOS

Install dependencies:

```bash
brew install cmake ninja qt
```

### Configure

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
```

### Build

```bash
cmake --build build
```

### Run

```bash
./build/exam_scheduler
```

---

# Running Tests

The project uses **CTest**.

## Configure with Tests Enabled

### Windows MinGW / Ninja

```powershell
cmake -S . -B build -G "Ninja" -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\mingw_64"
```

### Windows MSVC

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\msvc2022_64"
```

### Linux / macOS

```bash
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
```

---

## Build Tests

```bash
cmake --build build
```

For MSVC Debug:

```powershell
cmake --build build --config Debug
```

For MSVC Release:

```powershell
cmake --build build --config Release
```

---

## Run All Tests

### Ninja / MinGW / Linux / macOS

```bash
ctest --test-dir build --output-on-failure
```

### MSVC Debug

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

### MSVC Release

```powershell
ctest --test-dir build -C Release --output-on-failure
```

---

## Run a Specific Test

```bash
ctest --test-dir build -R ProgramCourseModelTest --output-on-failure
```

```bash
ctest --test-dir build -R ParserPreprocessorIntegrationTest --output-on-failure
```

```bash
ctest --test-dir build -R SchedulerIntegrationTest --output-on-failure
```

```bash
ctest --test-dir build -R InvalidInputIntegrationTest --output-on-failure
```

```bash
ctest --test-dir build -R NoSolutionIntegrationTest --output-on-failure
```

```bash
ctest --test-dir build -R OverlapConflictIntegrationTest --output-on-failure
```

For MSVC:

```powershell
ctest --test-dir build -C Debug -R SchedulerIntegrationTest --output-on-failure
```

---

# Application Flow

1. Run the application.
2. Select the courses file.
3. Select the exam-periods file.
4. Load or replace the data.
5. Select up to five study programs.
6. View courses if needed.
7. Open the calendar screen.
8. Edit excluded days or exam-period dates if needed.
9. Generate schedules.
10. Browse generated schedules.
11. Export the selected schedule.

---

# Scheduling Rules

The scheduling engine uses a backtracking algorithm.

The main conflict rules are:

- Obligatory courses from the same student group cannot be scheduled on the same date.
- An obligatory course and an elective course from the same student group cannot be scheduled on the same date.
- Two elective courses from the same student group may be scheduled on the same date.
- Conflict checking is based on dates only, not exam hours.

---

# Troubleshooting

## CMake Cannot Find Qt6

Error example:

```text
Could not find a package configuration file provided by "Qt6"
```

Fix by passing `CMAKE_PREFIX_PATH`:

```powershell
cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\mingw_64"
```

or:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\msvc2022_64"
```

---

## Qt DLL Not Found on Windows

Error example:

```text
Qt6Core.dll was not found
```

Fix:

```powershell
$env:Path = "C:\Qt\6.8.2\mingw_64\bin;$env:Path"
.\build\exam_scheduler.exe
```

or for MSVC:

```powershell
$env:Path = "C:\Qt\6.8.2\msvc2022_64\bin;$env:Path"
.\build\Release\exam_scheduler.exe
```

---

## QML File Not Found

Error example:

```text
FATAL ERROR: Engine root objects is empty
```

Fix:

```bash
cmake --build build
```

Then run again:

```powershell
.\build\exam_scheduler.exe
```

or:

```bash
./build/exam_scheduler
```

---

# Clean Build

## Windows PowerShell

```powershell
Remove-Item -Recurse -Force build
cmake -S . -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:\Qt\6.8.2\mingw_64"
cmake --build build
.\build\exam_scheduler.exe
```

## Linux / macOS

```bash
rm -rf build
cmake -S . -B build -G Ninja
cmake --build build
./build/exam_scheduler
```

---

# Development Workflow

Recommended Git workflow:

```bash
git checkout main
git pull origin main
git checkout -b TES-XX-short-description
```

After changes:

```bash
git status
git add .
git commit -m "TES-XX describe the change"
git push origin TES-XX-short-description
```

Before opening a Pull Request, run:

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

For MSVC:

```powershell
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

---

# Notes

- Use CMake for all builds.
- Do not use direct `g++` compilation.
- Keep scheduling logic in C++.
- Do not add scheduling logic inside QML.
- Run tests before opening or merging a Pull Request.
- Keep the UI responsive during schedule generation.
