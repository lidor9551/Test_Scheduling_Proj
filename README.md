# Exam Scheduling System

> **Version 1.0** — A C++ Object-Oriented exam scheduling system for generating all valid exam timetables for selected engineering study programs.

---

## Table of Contents

- [Project Overview](#project-overview)
- [Version 1.0 Scope](#version-10-scope)
- [Main Features](#main-features)
- [System Workflow](#system-workflow)
- [Project Architecture](#project-architecture)
- [Repository Structure](#repository-structure)
- [Core Components](#core-components)
- [Scheduling Algorithm](#scheduling-algorithm)
- [Conflict Rule](#conflict-rule)
- [Input Files](#input-files)
- [Output File](#output-file)
- [Build Instructions](#build-instructions)
- [Run Instructions](#run-instructions)
- [Command-Line Arguments](#command-line-arguments)
- [Testing and Validation](#testing-and-validation)
- [Performance Considerations](#performance-considerations)
- [Development Stages](#development-stages)
- [Future Extensions](#future-extensions)
- [Troubleshooting](#troubleshooting)
- [Authors](#authors)

---

## Project Overview

The **Exam Scheduling System** is a software engineering project designed to support the process of building exam schedules for engineering study programs.

The system receives structured text files as input, parses the academic data, filters the relevant courses, generates all legal exam dates, and produces all valid exam schedules according to the conflict rules defined for **Version 1.0**.

The project is implemented in **C++17** and follows an **Object-Oriented Programming (OOP)** design. The code is divided into clear layers: parsing, domain model, preprocessing, scheduling, and exporting.

The main goal is to generate valid exam schedules while preventing critical conflicts between exams of the same study program and academic year.

---

## Version 1.0 Scope

Version 1.0 focuses on the core scheduling engine.

### Included in this version

- File-based input and output.
- Reading three required input files:
  - Courses file.
  - Exam periods file.
  - Selected programs file.
- Filtering only courses whose evaluation type is `Exam`.
- Generating legal exam dates from exam-period definitions.
- Removing excluded dates and excluded date ranges.
- Generating all valid exam schedules.
- Checking conflicts by date.
- Writing a readable text output file.
- Supporting up to 5 selected study programs.
- Runtime protection using a configurable time limit.

### Not included in this version

- Graphical user interface.
- Exam hours.
- Room assignment.
- Lecturer availability constraints.
- Automatic ranking of schedules by student preferences.
- PDF or Excel export.
- Advanced filtering of generated schedules.

These features are planned as possible extensions for future versions.

---

## Main Features

- **C++17 implementation** using CMake.
- **Object-Oriented design** with separated responsibilities.
- **Parser layer** for converting text input into domain objects.
- **Preprocessing layer** for filtering and preparing scheduling blocks.
- **Backtracking scheduling engine** with pruning.
- **Efficient conflict checking** using count-based runtime state.
- **Readable output file** containing complete exam schedules.
- **Configurable execution limits** for large solution spaces.

---

## System Workflow

The system works as a pipeline:

```text
Input Files
    ↓
InputParser
    ↓
Domain Objects
    ↓
SchedulingPreprocessor
    ↓
Scheduling Blocks
    ↓
ScheduleGenerator
    ↓
Valid Schedule Solutions
    ↓
OutputWriter
    ↓
Human-Readable Output File
```

The main execution flow is:

1. Read and validate command-line arguments.
2. Parse the input files.
3. Build scheduling blocks.
4. Generate valid schedules for each block.
5. Combine block solutions.
6. Write the final output file.

---

## Project Architecture

The project follows a layered architecture:

```text
src/
├── dates/             Date handling and date utilities
├── model/             Core domain objects
├── parser/            Input file parsing and validation
├── preprocessing/     Data filtering and scheduling block creation
├── scheduler/         Backtracking solver and conflict logic
├── exporter/          Output file writer
└── main.cpp           Program entry point and pipeline orchestration
```

Each layer has a single responsibility and communicates with the next layer through clear data structures.

---

## Repository Structure

```text
.
├── CMakeLists.txt
├── README.md
├── sample_input/
│   ├── courses.txt
│   ├── periods.txt
│   └── selected_programs.txt
└── src/
    ├── main.cpp
    ├── dates/
    │   └── Date.cpp
    ├── model/
    │   ├── Course.cpp
    │   └── ExamPeriod.cpp
    ├── parser/
    │   └── InputParser.cpp
    ├── preprocessing/
    │   └── Preprocessor.cpp
    ├── scheduler/
    │   └── ScheduleGenerator.cpp
    └── exporter/
        └── OutputWriter.cpp
```

---

## Core Components

### 1. `main.cpp`

The entry point of the program.

Responsibilities:

- Parse command-line arguments.
- Activate the parser layer.
- Activate the preprocessing layer.
- Run the scheduling algorithm.
- Send the generated solutions to the output writer.
- Handle parsing errors, runtime errors, and timeout exceptions.

---

### 2. `InputParser`

Responsible for reading and validating the three input files.

Main parsing operations:

- `parseCourses(...)`
- `parseExamPeriods(...)`
- `parseSelectedPrograms(...)`

Validation includes:

- Valid course number format.
- Valid program IDs.
- Valid semester values.
- Valid moed values.
- Valid evaluation type.
- Valid requirement type.
- Maximum of 5 selected programs.

Supported program IDs:

```text
83101, 83102, 83103, 83104, 83105,
83107, 83108, 83109, 83115, 83182
```

---

### 3. Domain Model

The domain model represents the real academic entities used by the system.

| Entity | Purpose |
|---|---|
| `Course` | Represents a course, including name, number, instructor, evaluation type, and program offerings. |
| `CourseOffering` / program membership | Represents the relation between a course and a study program, including year, semester, and requirement type. |
| `ExamPeriod` | Represents an exam period with semester, moed, date range, and excluded dates. |
| `Date` | Represents and compares calendar dates. |
| `SchedulingBlock` | Represents one scheduling unit, usually by semester and moed. |
| `RuntimeCourse` | A filtered course prepared for the scheduling algorithm. |

---

### 4. `SchedulingPreprocessor`

Prepares the parsed data before running the scheduling algorithm.

Responsibilities:

- Remove courses that do not require an exam.
- Keep only courses that belong to selected study programs.
- Match courses to the relevant semester.
- Build scheduling blocks for each exam period.
- Generate group IDs based on `(programId, academicYear)`.
- Attach each runtime course to all relevant program-year groups.

This stage is important because it reduces the amount of data passed to the solver and improves runtime efficiency.

---

### 5. `ScheduleGenerator`

The core scheduling engine.

Responsibilities:

- Generate all valid assignments of courses to dates.
- Use recursive backtracking.
- Check conflicts before assigning a course.
- Prune invalid branches early.
- Stop if the configured runtime limit is exceeded.
- Support a maximum number of solutions per scheduling block.

The solver keeps an efficient internal state:

```text
assignedDate[course]
obligatoryCount[group][date]
electiveCount[group][date]
```

This allows the program to check conflicts quickly without scanning the entire partial schedule every time.

---

### 6. `OutputWriter`

Responsible for writing the generated schedules to a human-readable file.

The writer:

- Combines solutions from all scheduling blocks.
- Prints the total number of possible combined schedules.
- Supports a configurable output limit.
- Sorts exams by semester, moed, date, and course number.
- Prints each complete schedule clearly.

---

## Scheduling Algorithm

The algorithm is based on **Backtracking with pruning**.

At each recursive step:

1. Select the next unscheduled course.
2. Calculate all legal candidate dates for that course.
3. Try assigning the course to each legal date.
4. Check whether the assignment creates a conflict.
5. If valid, continue recursively.
6. If invalid, prune the branch.
7. When all courses are assigned, save the schedule as a valid solution.

Simplified pseudocode:

```text
backtrack():
    if all courses are assigned:
        save current solution
        return

    course = selectNextCourse()
    candidates = orderedCandidateDates(course)

    for each date in candidates:
        if canAssign(course, date):
            assign(course, date)
            backtrack()
            unassign(course, date)
```

### Course selection heuristic

The solver chooses the next course using a heuristic similar to **Minimum Remaining Values (MRV)**:

- Prefer the course with the smallest number of currently valid dates.
- Prefer courses with obligatory requirements before courses that are only elective.
- Prefer courses that belong to more program-year groups.
- Use course number as a deterministic tie-breaker.

This improves performance by handling the most constrained courses first.

### Candidate date ordering

Candidate dates are ordered by current scheduling pressure:

- Dates with fewer existing assignments for the same groups are tried first.
- If pressure is equal, dates are ordered chronologically.

---

## Conflict Rule

In **Version 1.0**, conflicts are checked by **date only**.

A conflict exists when two exams are scheduled on the same date and:

- They belong to the same study program.
- They belong to the same academic year.
- At least one of the courses is obligatory.

A conflict does **not** exist when:

- The exams belong to different programs.
- The exams belong to different academic years.
- The exams are scheduled on different dates.
- Both exams are elective courses.

This means that multiple elective exams may be scheduled on the same date for the same program and year, but an obligatory exam may not overlap with another exam in that same group.

---

## Input Files

The program receives three input files.

### 1. Courses File

Contains all courses in all programs.

Each course record is separated by four dollar signs:

```text
$$$$
```

Example:

```text
$$$$
Physics 1
83102
Prof. O. Some
83101,1,FALL,Obligatory
83102,1,FALL,Obligatory
Exam
```

Expected course record structure:

```text
Course name
Course number
Instructor name
Program ID, Year, Semester, Requirement
Program ID, Year, Semester, Requirement
...
Evaluation
```

Supported values:

| Field | Valid values |
|---|---|
| `Program ID` | 5-digit supported program code |
| `Year` | `1`, `2`, `3`, `4` |
| `Semester` | `FALL`, `SPRI`, `SUMM` |
| `Requirement` | `Obligatory`, `Elective` |
| `Evaluation` | `Exam`, `Project`, `Attendance` |

Only courses with:

```text
Evaluation = Exam
```

are scheduled.

### 2. Exam Periods File

Contains the date ranges in which exams may be scheduled.

Each period record is separated by:

```text
$$$$
```

Example:

```text
$$$$
FALL,Aleph
29-01-2026, 11-03-2026
31-01-2026 Saturday
02-03-2026, 04-03-2026 Purim
```

Expected exam period structure:

```text
Semester,Moed
StartDate, EndDate
ExcludedDate Comment
ExcludedStartDate, ExcludedEndDate Comment
...
```

Supported values:

| Field | Valid values |
|---|---|
| `Semester` | `FALL`, `SPRI`, `SUMM` |
| `Moed` | `Aleph`, `Bet`, `Gimel` |
| `Date format` | `DD-MM-YYYY` |

The parser supports excluded lines both with and without a leading dash:

```text
31-01-2026 Saturday
- 31-01-2026 Saturday
```

### 3. Selected Programs File

Contains the selected study programs for which the system should generate exam schedules.

Example:

```text
83101, 83102, 83108
```

The parser supports comma-separated values and newline-separated values.

Rules:

- At least one program must be selected.
- At most five programs may be selected.
- Every selected program must be a valid program ID.

---

## Output File

The output is a readable text file containing all generated schedules, or a limited number of schedules if `--combined-limit` is used.

Example output structure:

```text
All possible exam schedules
Blocks: 2
Total combined schedules: 48
Written schedules limit: 10

================ Schedule #1 ================

--- FALL | Aleph ---
29-01-2026 | 83102 | Physics 1 | Prof. O. Some
03-02-2026 | 83110 | Calculus 1 | Dr. A. Example

--- SPRI | Aleph ---
15-06-2026 | 83201 | Data Structures | Dr. B. Example
```

If no valid schedules exist, the program writes a clear message to the output file.

---

## Build Instructions

### Prerequisites

- C++17-compatible compiler.
- CMake 3.16 or newer.

Examples:

- Windows: Visual Studio Build Tools / MinGW / WSL.
- Linux: `g++` or `clang++`.
- macOS: Apple Clang or GCC.

### Build using CMake

From the repository root:

```bash
cmake -S . -B build
cmake --build build
```

The executable name is:

```text
exam_scheduler
```

On Windows with a multi-configuration generator, the executable may be created under:

```text
build/Debug/exam_scheduler.exe
```

or:

```text
build/Release/exam_scheduler.exe
```

---

## Run Instructions

Linux/macOS/WSL:

```bash
./build/exam_scheduler \
  --courses sample_input/courses.txt \
  --periods sample_input/periods.txt \
  --selected-programs sample_input/selected_programs.txt \
  --output output.txt
```

Windows PowerShell example:

```powershell
.\build\Debug\exam_scheduler.exe `
  --courses sample_input/courses.txt `
  --periods sample_input/periods.txt `
  --selected-programs sample_input/selected_programs.txt `
  --output output.txt
```

If the executable is generated directly inside `build`, use:

```powershell
.\build\exam_scheduler.exe --courses sample_input/courses.txt --periods sample_input/periods.txt --selected-programs sample_input/selected_programs.txt --output output.txt
```

---

## Command-Line Arguments

Required arguments:

| Argument | Description |
|---|---|
| `--courses <path>` | Path to the courses input file. |
| `--periods <path>` | Path to the exam periods input file. |
| `--selected-programs <path>` | Path to the selected programs file. |
| `--output <path>` | Path to the output file that will be created. |

Optional arguments:

| Argument | Description | Default |
|---|---|---|
| `--per-block-limit N` | Maximum number of solutions to generate for each scheduling block. | No limit |
| `--combined-limit N` | Maximum number of combined schedules to write to the output file. | No limit |
| `--max-runtime-seconds N` | Maximum allowed runtime for each scheduling block. | `30` |

---

## Example Execution

Recommended command for development:

```bash
cmake -S . -B build
cmake --build build
./build/exam_scheduler \
  --courses sample_input/courses.txt \
  --periods sample_input/periods.txt \
  --selected-programs sample_input/selected_programs.txt \
  --output output.txt \
  --per-block-limit 1000 \
  --combined-limit 100 \
  --max-runtime-seconds 30
```

After execution, open:

```text
output.txt
```

---

## Testing and Validation

Recommended validation checks:

### Parser validation

- Empty courses file should fail.
- Invalid course number should fail.
- Unknown program ID should fail.
- Invalid semester should fail.
- Invalid moed should fail.
- More than 5 selected programs should fail.

### Date generation validation

- Legal dates should be generated between start date and end date.
- Excluded single dates should be removed.
- Excluded date ranges should be removed.

### Scheduling validation

- Two obligatory exams in the same program-year group must not be scheduled on the same date.
- An obligatory exam and an elective exam in the same program-year group must not be scheduled on the same date.
- Two elective exams in the same program-year group may be scheduled on the same date.
- Courses from different programs may be scheduled on the same date.
- Courses from different academic years may be scheduled on the same date.

### Output validation

- Output file should be created successfully.
- Schedules should be separated clearly.
- Exams should be printed with date, course number, course name, and instructor name.
- Exams should be sorted by semester, moed, date, and course number.

---

## Performance Considerations

The software requirement for Version 1.0 is to generate the output within approximately **30 seconds**.

To support this requirement, the implementation uses:

- Early filtering of irrelevant courses.
- Separate scheduling blocks per exam period.
- Count-based conflict checking.
- Backtracking with pruning.
- MRV-style course selection.
- Candidate date ordering by scheduling pressure.
- Runtime timeout protection.
- Optional output limits for very large solution spaces.

Because the number of possible schedules can grow exponentially, the optional flags are useful during development and testing:

```bash
--per-block-limit 1000
--combined-limit 100
--max-runtime-seconds 30
```

---

## Development Stages

This section documents the main development stages of the project.

### Stage 1 — Requirements Analysis

The team analyzed the official requirements document and identified the main capabilities required for Version 1.0:

- File-based interface.
- Three input files.
- Generation of all valid exam schedules.
- Conflict prevention by date.
- Human-readable output.
- Runtime target of 30 seconds.
- Object-Oriented design.

### Stage 2 — System Design

The system was designed as a modular OOP pipeline.

Main design decisions:

- Keep parsing separate from scheduling.
- Represent academic entities as domain objects.
- Use preprocessing before solving.
- Split the scheduling problem into blocks.
- Use backtracking because the system must generate all valid solutions, not only one solution.
- Keep output writing separate from algorithm logic.

### Stage 3 — Data Model Implementation

The core model classes were implemented to represent:

- Courses.
- Program offerings.
- Evaluation types.
- Requirement types.
- Exam periods.
- Dates.

This stage created the foundation for clean and maintainable algorithm logic.

### Stage 4 — Input Parsing

The parser was implemented to read the required text files and convert them into domain objects.

Implemented validations include:

- Course number format.
- Program ID validation.
- Year validation.
- Semester validation.
- Evaluation type validation.
- Requirement type validation.
- Exam period date validation.

### Stage 5 — Preprocessing

The preprocessing layer was added to prepare the data for the solver.

This stage includes:

- Filtering selected programs.
- Filtering only exam-based courses.
- Matching courses to exam periods by semester.
- Building runtime scheduling blocks.
- Creating internal group IDs for efficient conflict checks.

### Stage 6 — Scheduling Algorithm

The scheduling algorithm was implemented using recursive backtracking.

Enhancements added in this stage:

- Early conflict detection.
- Fast assignment and unassignment.
- MRV-style course selection.
- Candidate date ordering.
- Runtime timeout exception.
- Per-block solution limit.

### Stage 7 — Output Export

The output writer was implemented to export generated schedules into a readable text file.

The output includes:

- Number of scheduling blocks.
- Total number of combined schedules.
- Optional written schedule limit.
- Full exam list for every generated schedule.
- Clear separation by semester and moed.

### Stage 8 — Testing and Review

Recommended review process:

1. Run the program on simple sample files.
2. Validate parser errors using intentionally invalid input.
3. Validate conflict rules using small controlled examples.
4. Validate output sorting and formatting.
5. Test larger files with runtime limits.
6. Review the code structure and responsibilities.

### Stage 9 — README and Documentation

The README is part of the project documentation package.

It explains:

- What the system does.
- What Version 1.0 includes.
- How the project is structured.
- How the scheduling algorithm works.
- How conflicts are detected.
- How to build and run the program.
- How to validate the implementation.
- What future extensions are planned.

---

## Future Extensions

Possible future improvements:

- Graphical user interface.
- Advanced schedule filtering.
- Ranking schedules by preference.
- Automatic schedule validation tool.
- Exam hour assignment.
- Room assignment.
- Lecturer constraints.
- Student load balancing.
- Export to PDF or Excel.
- External configuration file for program IDs and validation rules.
- Unit tests using a C++ testing framework.

---

## Troubleshooting

### `Parse error`

Usually means one of the input files does not match the required format.

Check:

- Missing fields.
- Invalid course number.
- Invalid program ID.
- Invalid date format.
- Invalid semester or moed.

### `Timeout`

The solver exceeded the configured time limit.

Try:

```bash
--max-runtime-seconds 60
```

or limit the number of generated solutions:

```bash
--per-block-limit 1000 --combined-limit 100
```

### `No valid schedules`

The constraints may be too strict for the available dates.

Check:

- Number of legal dates.
- Excluded dates.
- Number of obligatory courses in the same program and year.

---

## Authors

Developed as part of the Software Engineering Project Workshop.

Project: **Exam Scheduling System**  
Version: **1.0**  
Language: **C++17**  
Build system: **CMake**
