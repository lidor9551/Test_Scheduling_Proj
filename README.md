# 📘 Exam Scheduling System – Version 1.0

## ✨ Overview

The **Exam Scheduling System** is a file-based Object-Oriented software project designed to generate **all valid exam scheduling options** for selected engineering study programs.

As the number of courses and academic programs grows, manually creating exam schedules becomes difficult and may lead to conflicts between exams. This system helps solve that problem by reading structured input files, identifying all relevant exam-based courses, generating valid scheduling combinations, and exporting the results in a clear human-readable format.

**Version 1.0** focuses on the core scheduling engine:

- Reading academic data from text files
- Filtering relevant courses that require exams
- Generating legal exam dates
- Building all valid exam schedules
- Preventing critical conflicts according to the official requirements
- Writing the results to an output file

The project is implemented as a **C++17 file-based application** and follows **Object-Oriented Programming (OOP)** principles.

---

## 🎯 Project Objectives

The system is intended to:

- Read academic and scheduling data from input files
- Allow the user to choose up to **5 study programs**
- Generate **all possible valid exam systems** for the selected programs
- Prevent critical exam conflicts according to the official project rules
- Output all valid exam systems in a clear and sorted format
- Keep the architecture modular, testable, and extensible for future versions

---

## 🧩 Scope of Version 1.0

Version 1.0 includes:

- Input handling through text files only
- Parsing course data, exam periods, and selected programs
- Filtering courses whose evaluation method is `Exam`
- Generating all legal dates from exam periods
- Excluding blocked dates such as Saturdays, holidays, or unavailable ranges
- Generating all valid scheduling combinations
- Detecting conflicts according to the defined rules
- Writing all valid exam systems to a readable output file

Version 1.0 does **not** include:

- Graphical user interface
- Advanced user preference filtering
- Automatic schedule validation tools
- Optimization or recommendation mechanisms
- Exam room assignment
- Exam hour assignment

These features are planned for future versions of the system.

---

## ⚙️ Functional Requirements

### 📌 Program Selection

The user may select up to **5 study programs** from the predefined list of supported engineering programs.

### 📚 Course Filtering

The system considers only courses whose evaluation type is:

```text
Exam
```

Courses evaluated by `Project` or `Attendance` are ignored by the scheduling engine.

### 🗓️ Schedule Generation

The system generates **all possible valid exam systems** for the selected study programs.

Each generated system contains a date assignment for every relevant exam course.

### 🚫 Conflict Rules

A generated exam schedule is considered valid if it satisfies the following rule:

- Two exams that belong to the **same study program** and the **same academic year** may **not** be scheduled on the same date
- Unless **both courses are elective**

In **Version 1.0**:

- Conflicts are checked **by date only**
- Exam hours are **not** taken into account
- Conflicts between two elective courses are allowed
- Conflicts involving repeated courses may still exist

---

## 📥 Input Files

The system receives **three input files**.

### 1️⃣ `courses.txt`

Contains all available courses in the system, including:

- Course name
- Course number
- Instructor name
- Study program or programs
- Academic year
- Semester
- Requirement type: `Obligatory` / `Elective`
- Evaluation type: `Exam` / `Project` / `Attendance`

### 2️⃣ `exam_periods.txt`

Contains exam scheduling periods, including:

- Semester
- Exam period: `Aleph`, `Bet`, `Gimel`
- Start date
- End date
- Excluded dates or excluded date ranges

Examples of excluded dates:

- Saturdays
- Holidays
- Unavailable faculty dates

### 3️⃣ `selected_programs.txt`

Contains the IDs of the study programs selected by the user.

Example:

```text
83101, 83102, 83108
```

---

## 📤 Output

The output of the system is a list of **all valid exam systems**.

Each generated exam system includes:

- All exams included in that system
- For each exam:
  - Course name
  - Course number
  - Instructor name
  - Exam date
  - Semester
  - Exam period

The output is:

- Clearly formatted
- Human-readable
- Sorted by exam date
- Separated by semester
- Separated by exam period

In Version 1.0, the main required output format is:

```text
generated_schedules.txt
```

PDF and Excel export may be added as future extensions.

---

## 🏫 Supported Study Programs

The following study programs are supported in Version 1.0:

- `83101` – Computer Engineering
- `83102` – Electrical Engineering
- `83104` – Industrial Engineering and Information Systems
- `83107` – Data Engineering
- `83108` – Software Engineering
- `83109` – Materials Engineering
- `83105` – Computer Hardware Track – Computer Engineering
- `83182` – Quantum Engineering Track – Electrical Engineering
- `83103` – Neuroengineering Track – Electrical Engineering
- `83115` – Biomedical Engineering Track – Electrical Engineering

---

## 🏗️ Architecture Overview

The system is designed as a clean OOP pipeline.

```text
Input Files
    ↓
InputParser
    ↓
DataStore / Repository
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
TXT Output
```

### Why this architecture?

The project intentionally avoids using a large multidimensional array as the main scheduling structure.

Instead, the system uses:

```text
Objects + Lists + Maps + Backtracking + Conflict Rules
```

This approach is easier to maintain, easier to test, and more flexible for future versions.

---

## 🧱 Main Components

### 1. `ExamSchedulingController`

Responsible for managing the full pipeline:

- Reading input file paths
- Activating the parsers
- Preparing the data
- Running the scheduling algorithm
- Sending the final schedules to the output module

The controller coordinates the flow but does not contain the core scheduling logic itself.

---

### 2. `InputParser`

Responsible for reading and parsing the three input files:

- `courses.txt`
- `exam_periods.txt`
- `selected_programs.txt`

The parser converts raw text data into structured objects used by the system.

Possible parser methods:

```cpp
parseCourses()
parseExamPeriods()
parseSelectedPrograms()
```

---

### 3. `DataStore / Repository`

Stores all parsed data in memory using object-oriented data structures such as lists, sets, and maps.

This component may hold:

- All courses
- Exam periods
- Selected study programs
- Relevant exam courses
- Legal exam dates
- Generated exam systems

This is **not a real database** in Version 1.0.  
It is an in-memory repository used by the application during runtime.

---

### 4. `DateGenerator`

Responsible for generating all legal exam dates from the exam period definitions.

It removes:

- Excluded single dates
- Excluded date ranges
- Unavailable dates defined in the input file

The generated dates are later used by the scheduling engine.

---

### 5. `Preprocessor`

Responsible for preparing data before scheduling.

Main responsibilities:

- Filter only courses with `Evaluation = Exam`
- Keep only courses that belong to the selected programs
- Generate or receive legal dates per semester and exam period
- Sort courses by scheduling priority

Recommended scheduling priority heuristic:

1. Shared courses first
2. Obligatory courses before elective courses
3. Courses with fewer possible dates first

Shared courses are scheduled earlier as a **heuristic** in order to reduce the search space.  
This does not change the correctness of the algorithm.

---

### 6. `ScheduleGenerator`

The core scheduling engine.

This component uses a **Backtracking algorithm** to generate all valid exam systems.

The algorithm:

1. Selects the next course to schedule
2. Tries assigning it to each legal date
3. Checks whether the partial schedule is valid
4. Continues recursively if valid
5. Prunes the branch immediately if invalid
6. Saves the schedule when all courses are assigned

Backtracking is suitable because the system must generate **all valid schedules**, not just one solution.

---

### 7. `ConflictChecker`

Responsible for validating whether a new exam assignment creates a conflict.

In Version 1.0, the main rule is based on:

```text
programId + academicYear + date
```

A `Map`-based helper structure can be used for efficient conflict detection.

Example:

```cpp
std::map<ConflictKey, std::vector<ExamAssignment>> occupied;
```

Where `ConflictKey` represents:

```text
programId + academicYear + examDate
```

Future conflict rules may be implemented using a common `ConflictRule` interface, allowing new rules to be added without changing the scheduling algorithm.

---

### 8. `OutputWriter`

Responsible for converting generated schedules into a readable output file.

In Version 1.0, the main output format is TXT.

Future extensions may include:

- PDF export
- Excel export
- Filtered output views
- User-preference-based sorting

---

## 🧠 Scheduling Strategy

A suitable scheduling strategy for Version 1.0 is:

1. Read and parse all input files
2. Filter only relevant courses with `Exam` evaluation
3. Keep only courses from selected study programs
4. Generate all legal dates from exam periods
5. Sort courses using priority heuristics
6. Assign dates using Backtracking
7. Check conflicts during assignment
8. Save every fully valid schedule
9. Write all valid schedules to the output file

### Backtracking Pseudocode

```text
backtrack(courseIndex):

    if courseIndex == number of courses:
        save current schedule
        return

    course = courses[courseIndex]

    for each legal date for course.semester:
        assignment = course + date

        if ConflictChecker approves assignment:
            add assignment to current schedule
            mark assignment in occupied map

            backtrack(courseIndex + 1)

            remove assignment from current schedule
            unmark assignment from occupied map
```

---

## 🧩 Core Data Model

### `Course`

Represents a course in the system.

Possible fields:

```cpp
courseName
courseNumber
instructorName
offerings
evaluationType
```

### `CourseOffering`

Represents the connection between a course and a specific study program.

Possible fields:

```cpp
programId
academicYear
semester
requirementType
```

### `ExamPeriod`

Represents a legal exam period.

Possible fields:

```cpp
semester
moed
startDate
endDate
excludedDates
excludedRanges
```

### `ExamAssignment`

Represents a single course assigned to a specific date.

Possible fields:

```cpp
course
examDate
semester
moed
```

### `ExamSystem`

Represents one complete valid exam schedule.

Possible fields:

```cpp
assignments
```

### `ConflictKey`

Represents the key used for fast conflict detection.

Possible fields:

```cpp
programId
academicYear
examDate
```

---

## 🚀 Performance Requirement

According to the project requirements, the generation of the full output should take **no more than 30 seconds**.

For this reason, the scheduling logic should avoid unnecessary repeated checks.

Recommended performance improvements:

- Filter irrelevant courses before scheduling
- Generate legal dates once before Backtracking
- Sort constrained courses first
- Use a `Map` for fast conflict detection
- Prune invalid branches immediately

---

## 🗂️ Recommended Project Structure

```text
Test_Scheduling_Proj/
│
├── data/
│   ├── courses.txt
│   ├── exam_periods.txt
│   └── selected_programs.txt
│
├── src/
│   ├── main.cpp
│   │
│   ├── controller/
│   │   ├── ExamSchedulingController.h
│   │   └── ExamSchedulingController.cpp
│   │
│   ├── model/
│   │   ├── Course.h
│   │   ├── CourseOffering.h
│   │   ├── ExamPeriod.h
│   │   ├── ExamAssignment.h
│   │   ├── ExamSystem.h
│   │   └── ConflictKey.h
│   │
│   ├── parser/
│   │   ├── InputParser.h
│   │   └── InputParser.cpp
│   │
│   ├── repository/
│   │   ├── DataStore.h
│   │   └── DataStore.cpp
│   │
│   ├── dates/
│   │   ├── DateGenerator.h
│   │   └── DateGenerator.cpp
│   │
│   ├── preprocessing/
│   │   ├── Preprocessor.h
│   │   └── Preprocessor.cpp
│   │
│   ├── scheduler/
│   │   ├── ScheduleGenerator.h
│   │   └── ScheduleGenerator.cpp
│   │
│   ├── validation/
│   │   ├── ConflictChecker.h
│   │   ├── ConflictChecker.cpp
│   │   ├── ConflictRule.h
│   │   ├── SameProgramYearDateRule.h
│   │   └── SameProgramYearDateRule.cpp
│   │
│   └── output/
│       ├── OutputWriter.h
│       └── OutputWriter.cpp
│
├── docs/
│   ├── design_document.md
│   ├── test_plan.md
│   └── code_review_summary.md
│
├── tests/
│   ├── parser_tests/
│   ├── date_generator_tests/
│   ├── validation_tests/
│   └── scheduler_tests/
│
├── output/
│   └── generated_schedules.txt
│
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

## ▶️ Build and Run

### Using g++

```bash
g++ -std=c++17 -o exam_scheduler src/main.cpp
./exam_scheduler data/courses.txt data/exam_periods.txt data/selected_programs.txt
```

### Using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
./exam_scheduler ../data/courses.txt ../data/exam_periods.txt ../data/selected_programs.txt
```

> The exact command may change as the project structure evolves.

---

## ✅ Testing Plan

The project should include the following types of tests.

### Unit Tests

- Course parsing
- Exam period parsing
- Selected programs parsing
- Date generation
- Conflict checking
- Output formatting

### Integration Tests

- Full end-to-end execution using example input files
- Verification that all required exam courses are scheduled
- Verification that no invalid schedule is included in the output

### Edge Case Tests

- No selected programs
- More than 5 selected programs
- No exam courses
- All dates blocked
- Invalid input format
- Multiple overlapping program-year combinations
- Two elective courses on the same date
- Obligatory and elective course conflict on the same date

### Performance Tests

- Large input scenarios
- Runtime validation against the 30-second requirement

---

## 🔄 Development Process

The project is developed according to the official requirements using:

- **Git** for version control
- **JIRA** for task and workflow management
- **Agile methodology** for iterative development
- **Code Review** as part of the development and quality assurance process

Recommended workflow:

```text
main
│
├── feature/parser
├── feature/date-generator
├── feature/conflict-checker
├── feature/schedule-generator
└── feature/output-writer
```

Each feature should be implemented in a separate branch and merged after review.

---

## 🔮 Future Versions

According to the requirements, future versions of the system are expected to include:

### Version 2.0

- Advanced interface capabilities
- Improved interaction with users

### Version 3.0

- Automatic exam schedule validation tools
- Sorting and filtering of generated schedules
- Better adaptation to user preferences

### Version 4.0

- Advanced extensions
- Optimization options
- Additional scheduling constraints

---

## 👥 Team

**Current Team Lead:**  
Tom David Dan Harel

**Current Design Lead:**  
Lidor Ben David

**Current QA Lead:**  
Yoav Ben Nun

**Team Members:**  
Tom David Dan Harel, Lidor Ben David, Yoav Ben Nun, Omri Halfon, Samuel Mimoun

---

## 📌 Notes

This repository contains the implementation of **Version 1.0** of the Exam Scheduling System, based on the official software requirements document.

The focus of this version is correctness, modular design, maintainability, and generation of all valid exam scheduling systems according to the required input and conflict rules.
