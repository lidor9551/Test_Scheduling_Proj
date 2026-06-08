# Impossible Schedule Dataset

## Purpose

This dataset verifies that the system handles a no-solution scheduling case safely.

The dataset contains three obligatory exam courses for the same selected study program, but only one available exam day.

## Dataset Description

Selected program:

- `83101`

Courses:

- `83112` - Calculus 1
- `83102` - Physics 1
- `83120` - Intro to Programming

Exam period:

- Semester: `FALL`
- Moed: `Aleph`
- Date range: `01-02-2026` to `01-02-2026`
- Available exam days: 1

## Expected Result

The scheduler should detect that no valid schedule can be generated.

The system should either:

- return an empty list of schedules, or
- show a clear no-solution message

The application should not crash or freeze.

## Expected Status

No valid schedule expected
