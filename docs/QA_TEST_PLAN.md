# QA Test Plan - Version 2.0

## 1. Purpose

This document defines the QA strategy for Version 2.0 of the Exam Scheduling System.

The goal is to verify that the system behaves correctly according to the SRS, with focus on input loading, study program selection, calendar editing, schedule generation, overlap constraints, output display, and saving selected generated schedules.

## 2. Scope

The QA process covers:

- Input file loading
- Study program selection
- Exam-period calendar editing
- Scheduling generation
- Overlap and constraint validation
- Output schedule navigation
- Saving selected generated schedules to readable files
- Layout and UI validation
- Final regression testing

## 3. Out of Scope

The following areas are not implemented as part of this initial QA setup task:

- Writing all unit tests
- Writing all integration tests
- Creating all test datasets
- Running full regression
- Defining a parser-compatible export format

These areas should be handled in separate QA subtasks.

## 4. Test Levels

### 4.1 Unit Testing

Unit tests validate isolated classes and logic.

Examples:
- Model classes
- Date logic
- Excluded range logic
- Scheduler helper logic

### 4.2 Integration Testing

Integration tests validate flows that combine several components.

Examples:
- Input parser to preprocessing
- Preprocessing to scheduler
- Scheduler to output writer

### 4.3 Layout / UI Testing

Layout and UI tests validate user-facing screens.

Examples:
- Input screen layout
- Program selection screen
- Calendar screen
- Output schedule screen
- Button placement and behavior
- Long text handling

### 4.4 Manual End-to-End Testing

Manual tests validate complete user flows.

Examples:
- Load files
- Select programs
- Edit calendar
- Generate schedules
- Navigate output schedules
- Save selected schedule

### 4.5 Regression Testing

Regression testing verifies that existing functionality still works after feature changes and merges.

## 5. Test Status Definitions

- `Pass` - Actual result matches the expected result.
- `Fail` - Actual result does not match the expected result.
- `Blocked` - Test cannot be executed because a required feature, dataset, or dependency is missing.
- `Not Executed` - Test was not run in the current test cycle.

## 6. Evidence

QA evidence may include:

- Terminal output
- Screenshots
- Exported files
- Test logs
- Notes in QA reports
- Links to related bugs or PRs

## 7. Bug Handling

If a test fails, a separate Jira bug should be opened.

Each bug should include:

- Clear title
- Steps to reproduce
- Expected result
- Actual result
- Environment
- Evidence
- Severity