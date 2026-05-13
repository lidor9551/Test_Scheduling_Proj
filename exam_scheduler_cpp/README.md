# Exam Scheduler - C++ OOP

This project implements version 1.0 of the exam scheduling system in C++ with an object-oriented design.

## What it supports

- Reads the 3 required input files:
  - courses file
  - exam periods file
  - selected programs file
- Generates all valid exam schedules for the selected programs
- Filters only courses with `Evaluation = Exam`
- Applies the version 1.0 conflict rule:
  - same date + same program + same year is forbidden
  - except when both exams are elective
- Writes a human-readable output file
- Uses file-based input/output
- Built to be extensible for future versions

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/exam_scheduler \
  --courses sample_input/courses.txt \
  --periods sample_input/periods.txt \
  --selected-programs sample_input/selected_programs.txt \
  --output output.txt
```

Optional flags:

```bash
--per-block-limit 1000
--combined-limit 100
--max-runtime-seconds 30
```

## Project structure

- `include/` - headers
- `src/` - implementation
- `sample_input/` - example files in the required format

## Main design

- `Course`, `ProgramOffering`, `ExamPeriod`, `Date` - domain model
- `CourseFileParser`, `ExamPeriodFileParser`, `SelectedProgramsFileParser` - parsing layer
- `SchedulingPreprocessor` - converts selected data into runtime scheduling blocks
- `ExamScheduler` - backtracking solver with pruning
- `OutputWriter` - human-readable output

## Notes

- The parser accepts exclusion lines both as:
  - `31-01-2026 Saturday`
  - `- 31-01-2026 Saturday`
- Runtime state is kept efficient using:
  - assigned date per course
  - obligatory count per (program, year, date)
  - elective count per (program, year, date)
