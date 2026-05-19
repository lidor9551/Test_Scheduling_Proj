# Exam Scheduling System

## Overview

Exam Scheduling System is a file-based software project for generating valid exam schedules for selected engineering study programs.

The system receives structured input files, parses course and exam-period data, filters the relevant exam-based courses, generates legal exam dates, and produces all valid exam schedules according to the required conflict rules.

## Team Roles and Responsibilities

Version 1 responsibilities were divided between the team members as follows:

- **Lidor – Development Lead**  
  Responsible for leading the development process, implementing the core logic, integrating the system components, and managing the main development workflow.
  
- **Yoav – QA Lead**  
  Responsible for defining and performing tests, validating the correctness of the generated schedules, and ensuring that the system follows the requirements.

- **Omri – Design Lead**  
  Responsible for the software design, UML diagrams, system architecture, and maintaining a modular Object-Oriented structure.

Version 1.0 focuses on the core scheduling engine:
- Reading input from files
- Processing courses and exam periods
- Generating valid exam schedules
- Detecting date-based conflicts
- Exporting readable output to a text file

The system is designed using Object-Oriented Programming principles and is built in a modular way to support future extensions.

---

## Project Management

The project is managed using an Agile development approach.

Development is maintained through Git, while tasks and progress are managed using Jira.  
Each team member works through their own development flow, and the main branch represents the stable version of the project.

The project includes supporting documents such as:
- Software requirements document
- Design document
- UML / architecture description
- Testing documentation
- Code review summary

---

## Input Files

The system receives three input files:

```text
courses.txt
exam_periods.txt
selected_programs.txt
```

### courses.txt

Contains all course data, including:
- Course name
- Course number
- Instructor name
- Study programs
- Academic year
- Semester
- Requirement type
- Evaluation type

Only courses with `Evaluation = Exam` are scheduled.

### exam_periods.txt

Contains the exam periods, including:
- Semester
- Exam period / Moed
- Start date
- End date
- Excluded dates or excluded date ranges

### selected_programs.txt

Contains the selected study programs for which exam schedules should be generated.

Example:

```text
83101, 83102, 83108
```

---

## Scheduling Logic

The system works as a pipeline:

```text
Input Files
    ↓
InputParser
    ↓
DataStore
    ↓
DateGenerator
    ↓
Preprocessor
    ↓
ScheduleGenerator
    ↓
ConflictChecker
    ↓
OutputWriter
    ↓
generated_schedules.txt
```

General process:

1. Read and parse the input files.
2. Convert the text data into internal objects.
3. Generate all legal exam dates from the exam periods.
4. Filter only relevant courses with exams.
5. Keep only courses that belong to the selected programs.
6. Sort courses by scheduling priority.
7. Use Backtracking to generate all valid schedules.
8. Check conflicts during the scheduling process.
9. Save every complete valid schedule.
10. Write the result to a readable output file.

---

## Conflict Rule

In Version 1.0, conflicts are checked by date only.

A conflict exists when two exams:
- Belong to the same study program
- Belong to the same academic year
- Are scheduled on the same date
- And at least one of them is not an elective course

If both courses are elective, the conflict is allowed according to the Version 1.0 requirements.

---

## Performance

The system is required to generate the output within 30 seconds.

To improve performance, the implementation uses:
- Preprocessing before scheduling
- Filtering irrelevant courses early
- Generating legal dates once
- Backtracking with pruning
- Fast conflict checking using map-based structures
- Prioritized course ordering

---

## How to Run

### Windows PowerShell

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -Iinclude src\*.cpp -o exam_scheduler.exe
.\exam_scheduler.exe data\courses.txt data\exam_periods.txt data\selected_programs.txt output\generated_schedules.txt
```

### Linux / macOS / WSL

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Iinclude src/*.cpp -o exam_scheduler
./exam_scheduler data/courses.txt data/exam_periods.txt data/selected_programs.txt output/generated_schedules.txt
```

---

## Output

The output file contains all valid generated exam schedules.

Default output file:

```text
output/generated_schedules.txt
```

Each generated schedule includes:
- Course name
- Course number
- Instructor name
- Exam date
- Semester
- Moed

The output is written in a readable text format and sorted by exam dates.

---

## Future Extensions

The system was designed to support future versions, including:
- Graphical user interface
- Advanced schedule filtering
- Automatic schedule validation
- Exam hour assignment
- Room assignment
- Additional conflict rules
- Export to PDF or Excel

---

## Status

Current version: `1.0`

This version implements the core file-based scheduling logic and generates valid exam schedules according to the defined project requirements.
