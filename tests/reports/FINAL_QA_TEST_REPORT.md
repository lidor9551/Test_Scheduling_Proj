# Final QA Test Report — Exam Scheduler

## 1. Overview

This report summarizes the final QA validation for the Exam Scheduler project.

The project currently includes automated unit tests, integration tests, regression tests, GUI/QML smoke tests, manual QA documentation, and GitHub Actions CI.

The goal of this report is to document the final test baseline before submission and confirm that the system is stable after the QA automation phase.

---

## 2. Repository Information

| Field                | Value                            |
| -------------------- | -------------------------------- |
| Repository           | `lidor9551/Test_Scheduling_Proj` |
| Main Branch          | `main`                           |
| Build System         | CMake                            |
| Framework            | Qt 6.8.0                         |
| Compiler / Toolchain | MSVC 2022                        |
| Test Runner          | CTest                            |
| CI                   | GitHub Actions                   |

---

## 3. Automated Test Baseline

The final automated baseline includes:

| Category                  | Coverage                                                                                                      |
| ------------------------- | ------------------------------------------------------------------------------------------------------------- |
| Domain Unit Tests         | Course, Date, ExamPeriod, DateAvailabilityPolicy, ScheduleGenerationResult                                    |
| Infrastructure Unit Tests | InputParser edge cases                                                                                        |
| Application Unit Tests    | SchedulingSession                                                                                             |
| Scheduling Unit Tests     | SameGroupConflictRule, AdvancedConflictRules, MetricsCalculator, SchedulingServiceAsync                       |
| Presentation Unit Tests   | AppController, CalendarManager, SettingsWorkflow, ProgramCourseModel, ScheduleOutputManager                   |
| Integration Tests         | Parser/Preprocessor, Scheduler pipeline, invalid input, no solution, conflicts, hard constraints, export flow |
| Regression Tests          | Phase 3 smoke flow                                                                                            |
| GUI/QML Tests             | QML smoke loading for major screens                                                                           |

---

## 4. Automated Test Run

### Command

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

### Result

```text
25/25 tests passed
100% tests passed
0 tests failed
```

### Test Suite Summary

| #  | Test Name                             | Result |
| -- | ------------------------------------- | ------ |
| 1  | CourseModelTest                       | PASS   |
| 2  | DateAndExamPeriodTest                 | PASS   |
| 3  | DateAvailabilityPolicyTest            | PASS   |
| 4  | ScheduleGenerationResultTest          | PASS   |
| 5  | InputParserEdgeCaseTest               | PASS   |
| 6  | SchedulingSessionTest                 | PASS   |
| 7  | SameGroupConflictRuleTest             | PASS   |
| 8  | AdvancedConflictRulesTest             | PASS   |
| 9  | MetricsCalculatorTest                 | PASS   |
| 10 | SchedulingServiceAsyncTest            | PASS   |
| 11 | AppControllerTest                     | PASS   |
| 12 | CalendarManagerTest                   | PASS   |
| 13 | SettingsWorkflowTest                  | PASS   |
| 14 | ProgramCourseModelTest                | PASS   |
| 15 | ScheduleOutputManagerExportSafetyTest | PASS   |
| 16 | ScheduleOutputManagerSortingTest      | PASS   |
| 17 | ParserPreprocessorIntegrationTest     | PASS   |
| 18 | SchedulerIntegrationTest              | PASS   |
| 19 | InvalidInputIntegrationTest           | PASS   |
| 20 | NoSolutionIntegrationTest             | PASS   |
| 21 | OverlapConflictIntegrationTest        | PASS   |
| 22 | HardConstraintsIntegrationTest        | PASS   |
| 23 | ExportFlowIntegrationTest             | PASS   |
| 24 | Phase3RegressionSmokeTest             | PASS   |
| 25 | QmlSmokeTest                          | PASS   |

---

## 5. Label Summary

The automated suite covers the following CTest labels:

| Label            | Purpose                                                  |
| ---------------- | -------------------------------------------------------- |
| `Unit`           | Isolated unit tests                                      |
| `Integration`    | Multi-component integration flows                        |
| `Regression`     | Regression smoke coverage                                |
| `Domain`         | Domain model coverage                                    |
| `Infrastructure` | Parser and input validation coverage                     |
| `Application`    | Session and workflow state coverage                      |
| `Scheduling`     | Scheduling engine and rule coverage                      |
| `Presentation`   | Controller and UI-facing manager coverage                |
| `Phase3`         | Hard constraints, metrics, sorting, and Phase 3 behavior |
| `GUI` / `QML`    | QML smoke loading                                        |
| `Smoke`          | Lightweight high-level smoke coverage                    |

---

## 6. GitHub Actions CI

A GitHub Actions workflow was added to automatically validate the project.

The CI workflow runs on:

```text
pull_request → main
push → main
```

The CI performs:

```text
Checkout repository
Install Qt
Configure CMake
Build Debug
Run full CTest suite
```

Final CI result:

```text
Build Debug + CTest: PASS
```

---

## 7. Manual QA

Manual QA scenarios are documented in:

```text
tests/manual/MANUAL_QA_CHECKLIST.md
```

The checklist covers:

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

Manual QA is intended to complement automated testing by validating user-facing behavior, visual layout, navigation, and export usability.

---

## 8. QA Improvements Completed

The QA phase included the following improvements:

| Area                  | Completed Work                                                             |
| --------------------- | -------------------------------------------------------------------------- |
| Test Structure        | Organized tests by unit, integration, regression, GUI, manual, and reports |
| Common Utilities      | Added shared test macros and helpers                                       |
| Domain Coverage       | Improved Date and ExamPeriod coverage                                      |
| Parser Coverage       | Added parser edge-case tests                                               |
| Presentation Coverage | Added AppController and CalendarManager tests                              |
| Async Coverage        | Added SchedulingService async tests                                        |
| Settings Coverage     | Added settings workflow tests                                              |
| QML Coverage          | Added QML smoke test                                                       |
| CI                    | Added GitHub Actions build and CTest workflow                              |
| Manual QA             | Added manual QA checklist                                                  |
| Documentation         | Updated tests README                                                       |

---

## 9. Known Notes

The current automated baseline is green.

No automated test failures remain in the final local run.

Future refactors should preserve the same baseline and add regression tests for every bug fix.

---

## 10. Final Status

| Field               | Status   |
| ------------------- | -------- |
| Build               | PASS     |
| Automated Tests     | PASS     |
| GUI/QML Smoke Test  | PASS     |
| GitHub Actions CI   | PASS     |
| Manual QA Checklist | Prepared |
| Final QA Status     | PASS     |

---

## 11. Final Conclusion

The Exam Scheduler project has a stable QA baseline.

The project successfully builds, runs the complete automated test suite, loads the QML screens in automated smoke testing, and validates the main scheduling, constraint, sorting, export, and regression flows.

The final automated result is:

```text
25/25 tests passed
```

The project is ready for final review from the QA/testing perspective.
