# Design Notes

## Goal
Version 1.0 exam scheduler in C++ with OOP, file-based input/output, and room for extension.

## Main classes
- `Date` - parsing, formatting, ordering, next-day traversal
- `Course`, `ProgramOffering`, `ExamPeriod`, `ExcludedRange` - domain model
- `CourseFileParser`, `ExamPeriodFileParser`, `SelectedProgramsFileParser` - input parsing and validation
- `SchedulingPreprocessor` - builds scheduling blocks by semester and moed
- `ExamScheduler` - backtracking solver with pruning and timeout
- `OutputWriter` - writes full human-readable schedules

## Extensibility ideas
- Add sorting/filtering preferences in `OutputWriter` or a dedicated ranking component
- Add validation/reporting layer for version 2.0
- Add GUI/web layer without changing the core scheduling engine
- Add more conflict rules by extending the scheduler state and `canAssign`
