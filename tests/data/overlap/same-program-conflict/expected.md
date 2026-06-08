# Same Program Conflict Dataset

## Purpose

This dataset verifies that the scheduler prevents conflicting exams for the same selected study program.

The dataset contains two obligatory exam courses for the same program, but only one available exam day.

## Dataset Description

Selected program:

- `83101`

Courses:

- `83112` - Calculus 1
- `83102` - Physics 1

Exam period:

- Semester: `FALL`
- Moed: `Aleph`
- Date range: `01-02-2026` to `01-02-2026`
- Available exam days: 1

## Conflict Rule

Two exams that belong to the same selected study program should not be scheduled on the same day.

## Expected Result

The scheduler should not generate a valid schedule that places both exams on the same day.

The system should either:

- return no valid schedules, or
- report that no valid schedule exists

The application should not crash.

## Expected Status

No valid schedule expected
