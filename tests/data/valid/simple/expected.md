# Valid Simple Dataset

## Purpose

This dataset represents a minimal valid scheduling scenario.

It is intended to verify that the system can:

- parse a valid courses file
- parse a valid exam-periods file
- read a selected study program
- create a basic scheduling input
- generate at least one valid schedule

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
- Date range: `01-02-2026` to `07-02-2026`
- Excluded day: `07-02-2026`

## Expected Result

The parser should load all input files successfully.

The scheduler should be able to generate at least one valid schedule where:

- all three selected courses are scheduled
- no exam is scheduled on the excluded day
- all exams are scheduled inside the exam-period date range
- the application does not crash

## Expected Status

Pass
