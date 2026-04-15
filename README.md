# Exam Scheduling System – Version 1.0

## Overview
This project implements **Version 1.0** of an exam scheduling system for the Faculty of Engineering.  
The goal of the system is to generate **all possible valid exam schedules** for selected study programs, while preventing **critical scheduling conflicts** between exams according to the project requirements.

The system is designed as a **file-based application** in this version, with input taken from structured text files and output written to a readable text file.  
The software is developed using **Object-Oriented Programming (OOP)** principles and is designed to support future extensions in later versions. :contentReference[oaicite:1]{index=1}

---

## Project Goals
The system is required to:

- Read course data, exam periods, and selected study programs from input files.
- Identify all courses that are evaluated by **Exam**.
- Generate **all valid exam scheduling options** for the selected programs.
- Prevent critical conflicts between exams according to the rules defined in the requirements.
- Output all generated exam systems in a clear, readable, and sorted format.
- Maintain a design that is modular and extensible for future versions. :contentReference[oaicite:2]{index=2}

---

## Version 1.0 Scope
Version 1.0 focuses on the core scheduling engine and includes:

- Reading input data from files.
- Creating all valid exam schedule combinations.
- Conflict validation based on dates only.
- Producing human-readable output files.

Version 1.0 does **not** include:

- Advanced user interface
- User preference filtering
- Automatic validation tools
- Advanced sorting and recommendation capabilities

These features are planned for future versions. :contentReference[oaicite:3]{index=3}

---

## Functional Requirements
The system allows the user to select up to **5 study programs** from a predefined list of engineering programs.  
Based on these selected programs, the system generates all possible exam schedules for relevant courses whose evaluation type is `Exam`. :contentReference[oaicite:4]{index=4}

A generated exam schedule is considered valid if it does not contain **critical conflicts**, meaning:

- Two exams from the **same program** and the **same year** may not be assigned to the same date,
- unless **both courses are elective**.

In Version 1.0, conflicts are checked **by date only**, without considering exam hours.  
Additionally, the requirements explicitly state that in this version there may still be conflicts for students repeating courses, and conflicts between two elective courses in the same program are allowed. :contentReference[oaicite:5]{index=5}

---

## Input Files
The system receives **three input files**:

1. **Courses data file**  
   Contains information about all courses, including:
   - course name
   - course number
   - instructor name
   - study programs in which the course is taught
   - year
   - semester
   - requirement type (`Obligatory` / `Elective`)
   - evaluation type (`Exam` / `Project` / `Attendance`)

2. **Exam periods file**  
   Contains:
   - semester
   - exam period (Moed)
   - start and end dates
   - excluded dates or excluded date ranges (e.g. holidays, Saturdays)

3. **Selected programs file**  
   Contains the selected study program IDs.  
   The user may select up to **5 programs**. :contentReference[oaicite:6]{index=6}

---

## Output
The system outputs a list of **all possible valid exam systems**.

Each generated system includes:

- all exams included in that system
- for each exam:
  - exam date
  - instructor name

The output is:

- clearly formatted
- sorted by exam date
- separated by semester
- separated by exam periods (e.g. Moed Aleph / Moed Bet) :contentReference[oaicite:7]{index=7}

---

## Suggested Architecture
The project is built using an object-oriented design.  
A possible architecture includes the following core components:

- `Course`
- `CourseOffering` / `ProgramCourseInfo`
- `ExamPeriod`
- `ExamAssignment`
- `ExamSystem`
- `InputParser`
- `DateGenerator`
- `ConflictChecker`
- `ScheduleGenerator`
- `OutputWriter`

This structure helps separate responsibilities between:

- data modeling
- file parsing
- scheduling logic
- conflict validation
- output generation

Such separation is important in order to support the future versions described in the requirements. :contentReference[oaicite:8]{index=8}

---

## Scheduling Approach
The scheduling engine is responsible for generating **all valid combinations** of exam assignments.

A typical solution for this version is based on:

- generating all legal dates from each exam period
- filtering relevant courses
- checking conflicts during assignment
- using a search/backtracking approach to build all valid schedules

Since the system must generate **all possible valid outputs**, correctness and efficient pruning of invalid options are both important.  
The project requirements also specify that the complete output generation must not exceed **30 seconds**. :contentReference[oaicite:9]{index=9}

---

## Development Requirements
According to the project requirements:

- The software must be implemented in **C++** or **Python**
- The design must follow **OOP**
- Development should be managed using **Git**
- Task management should be done using **JIRA**
- The project should follow an **Agile** development methodology
- The project documentation must include design, testing, implementation, and code review materials. :contentReference[oaicite:10]{index=10}

---

## Project Structure
Example project structure:

```text
project-root/
│
├── data/
│   ├── courses.txt
│   ├── exam_periods.txt
│   └── selected_programs.txt
│
├── src/
│   ├── model/
│   ├── parser/
│   ├── scheduler/
│   ├── validation/
│   └── output/
│
├── tests/
│   ├── unit/
│   └── integration/
│
├── docs/
│   ├── design_document.md
│   ├── test_plan.md
│   └── code_review_summary.md
│
├── output/
│   └── generated_schedules.txt
│
└── README.md
