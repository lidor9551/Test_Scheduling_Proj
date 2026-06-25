# Tests — Exam Scheduler

This directory contains the automated and manual QA assets for the Exam Scheduler project.

The test suite covers the core domain model, parser and validation logic, scheduling engine, Phase 3 hard constraints, metrics and sorting, presentation-layer managers, QML smoke loading, integration flows, export behavior, and regression scenarios.

---

## Test Structure

```text
tests/
├── common/
├── data/
├── gui/
├── integration/
├── manual/
├── regression/
├── unit/
├── CMakeLists.txt
└── README.md
```

---

## Test Types

### 1. Unit Tests

Unit tests verify isolated components and business rules.

#### Domain

| Test                           | Purpose                                                                                           |
| ------------------------------ | ------------------------------------------------------------------------------------------------- |
| `CourseModelTest`              | Verifies course creation, course metadata, program membership, requirement and evaluation fields. |
| `DateAndExamPeriodTest`        | Verifies date parsing, validation, arithmetic, comparison, and exam period behavior.              |
| `DateAvailabilityPolicyTest`   | Verifies allowed exam dates, excluded ranges, and Saturday blocking.                              |
| `ScheduleGenerationResultTest` | Verifies schedule assignment storage and preservation of course identity.                         |

#### Infrastructure

| Test                      | Purpose                                                                                                                |
| ------------------------- | ---------------------------------------------------------------------------------------------------------------------- |
| `InputParserEdgeCaseTest` | Verifies parser validation for selected programs, courses, exam periods, exclusions, malformed values, and edge cases. |

#### Application

| Test                    | Purpose                                                                                                                                  |
| ----------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| `SchedulingSessionTest` | Verifies application session state, selected programs, loaded courses, exam periods, append/replace behavior, and generation validation. |

#### Scheduling

| Test                         | Purpose                                                                                                      |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------ |
| `SameGroupConflictRuleTest`  | Verifies the base same-group conflict rule.                                                                  |
| `AdvancedConflictRulesTest`  | Verifies Phase 3 hard constraints 2.1–2.5.                                                                   |
| `MetricsCalculatorTest`      | Verifies Phase 3 metrics calculation.                                                                        |
| `SchedulingServiceAsyncTest` | Verifies asynchronous scheduling service behavior, success flow, empty result flow, and duplicate-run guard. |

#### Presentation

| Test                                    | Purpose                                                                                                                      |
| --------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------- |
| `AppControllerTest`                     | Verifies controller state, file path handling, data loading errors, program selection, settings, and sorting priority state. |
| `CalendarManagerTest`                   | Verifies calendar state, period navigation, day toggling, excluded dates, Saturdays, saving, and shifting period bounds.     |
| `SettingsWorkflowTest`                  | Verifies applying settings and regenerating schedules through the controller workflow.                                       |
| `ProgramCourseModelTest`                | Verifies course filtering and QML-facing course model behavior.                                                              |
| `ScheduleOutputManagerExportSafetyTest` | Verifies export safety checks and invalid export scenarios.                                                                  |
| `ScheduleOutputManagerSortingTest`      | Verifies schedule sorting by Phase 3 metrics and priority order.                                                             |

---

### 2. Integration Tests

Integration tests verify that multiple components work together.

| Test                                | Purpose                                                             |
| ----------------------------------- | ------------------------------------------------------------------- |
| `ParserPreprocessorIntegrationTest` | Verifies parser-to-preprocessor flow using valid input data.        |
| `SchedulerIntegrationTest`          | Verifies parser-to-preprocessor-to-scheduler flow.                  |
| `InvalidInputIntegrationTest`       | Verifies malformed input rejection.                                 |
| `NoSolutionIntegrationTest`         | Verifies no-solution scenarios.                                     |
| `OverlapConflictIntegrationTest`    | Verifies same-program conflict rejection.                           |
| `HardConstraintsIntegrationTest`    | Verifies Phase 3 hard constraints in an integrated scheduling flow. |
| `ExportFlowIntegrationTest`         | Verifies generated schedule export flow.                            |

---

### 3. Regression Tests

Regression tests verify that previously completed flows still work after changes.

| Test                        | Purpose                                                     |
| --------------------------- | ----------------------------------------------------------- |
| `Phase3RegressionSmokeTest` | Verifies a Phase 3 smoke flow: generate, sort, and display. |

---

### 4. GUI / QML Tests

| Test           | Purpose                                                                 |
| -------------- | ----------------------------------------------------------------------- |
| `QmlSmokeTest` | Verifies that the main QML screens load successfully in offscreen mode. |

Covered QML files:

```text
Main.qml
SettingsScreen.qml
CalendarScreen.qml
OutputScreen.qml
PeriodEditorScreen.qml
```

---

### 5. Manual QA

Manual QA scenarios are documented in:

```text
tests/manual/MANUAL_QA_CHECKLIST.md
```

The manual checklist covers:

```text
Data loading
Program selection
Calendar management
Period editor
Hard constraints settings
Schedule generation
Output screen
Sorting priorities
Export
GUI visual regression
Full regression flow
```

---

## Test Data

Test datasets are located under:

```text
tests/data/
```

The data folder includes valid input, invalid input, no-solution scenarios, conflict scenarios, performance-related datasets, and Phase 3 datasets.

Each dataset should include input files and, when relevant, an `expected.md` file describing the expected behavior.

---

## Running Tests Locally

### Windows

From the project root:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Expected result:

```text
100% tests passed
```

---

## Running Specific Test Groups

### Unit Tests

```powershell
ctest --test-dir build -C Debug -L Unit --output-on-failure
```

### Integration Tests

```powershell
ctest --test-dir build -C Debug -L Integration --output-on-failure
```

### Phase 3 Tests

```powershell
ctest --test-dir build -C Debug -L Phase3 --output-on-failure
```

### Regression Tests

```powershell
ctest --test-dir build -C Debug -L Regression --output-on-failure
```

### GUI / QML Tests

```powershell
ctest --test-dir build -C Debug -L QML --output-on-failure
```

Or directly:

```powershell
ctest --test-dir build -C Debug -R QmlSmokeTest --output-on-failure
```

---

## GitHub Actions CI

The project includes a GitHub Actions workflow that runs on pull requests and pushes to `main`.

The CI workflow performs:

```text
Checkout repository
Install Qt
Configure CMake
Build Debug
Run full CTest suite
```

A pull request should not be merged unless the CI workflow passes.

---

## Current Automated Baseline

The current automated baseline includes:

```text
25 automated tests
Unit tests
Integration tests
Regression tests
GUI/QML smoke test
```

Expected result:

```text
25/25 tests passed
```

---

## Adding a New Test

When adding a new test:

1. Place the test in the correct folder:

   * `tests/unit/...`
   * `tests/integration/...`
   * `tests/regression/...`
   * `tests/gui/...`

2. Register it in:

```text
tests/CMakeLists.txt
```

3. Add meaningful CTest labels, for example:

```text
Unit
Integration
Regression
Phase3
Presentation
Scheduling
GUI
QML
Smoke
```

4. Run the relevant subset first.

5. Run the full suite before opening a pull request:

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

---

## QA Rule

Every bug fix should include either:

```text
A new automated regression test
```

or:

```text
An updated manual QA checklist entry
```

This keeps the project stable during future refactors.
