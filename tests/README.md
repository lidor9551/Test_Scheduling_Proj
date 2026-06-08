# Test Suite

This folder contains the testing assets for the Exam Scheduling System.

## Structure

- `unit/` - automated tests for isolated classes and logic.
- `integration/` - automated tests for flows that combine multiple components.
- `layout/` - UI and layout validation tests or checklists.
- `manual/` - manual QA scenarios for user-facing flows.
- `data/` - controlled input datasets used by tests.
- `reports/` - QA execution reports and regression summaries.

## Test Data Structure

- `data/valid/` - valid input files for successful flows.
- `data/invalid/` - invalid input files for error-handling tests.
- `data/overlap/` - datasets for exam conflict and overlap validation.
- `data/no_solution/` - datasets where no valid schedule should exist.

## Test Types

### Unit Tests

Unit tests validate individual classes and logic in isolation.

Examples:
- Date comparison
- Excluded range logic
- Course model behavior
- Schedule generator constraints

### Integration Tests

Integration tests validate complete internal flows without relying on the GUI.

Examples:
- Parser to preprocessing flow
- Preprocessing to scheduler flow
- Scheduler to output writer flow

### Layout / UI Tests

Layout tests validate that screens remain readable and usable.

Examples:
- Buttons do not overlap
- Long course names do not break the UI
- Calendar layout remains readable
- Output schedule screen displays information clearly

### Manual QA Tests

Manual tests validate complete user flows that are difficult to automate at this stage.

Examples:
- Loading input files through the UI
- Selecting study programs
- Editing calendar days
- Generating schedules
- Navigating generated schedules
- Saving a selected generated schedule

## Test Documentation Rule

Every test should clearly define:

- Purpose
- Preconditions
- Input data
- Steps
- Expected result
- Actual result when executed
- Status: Pass, Fail, Blocked, or Not Executed