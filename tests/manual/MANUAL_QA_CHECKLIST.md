# Manual QA Checklist — Exam Scheduler

## Purpose

This checklist defines the manual QA scenarios for the Exam Scheduler project after the automated test suite is green.

The checklist complements the automated CTest suite and focuses on user-facing flows that are difficult to fully validate through unit or integration tests, especially GUI behavior, visual feedback, navigation, sorting, and export.

## Test Environment

| Field                       | Value                                                 |
| --------------------------- | ----------------------------------------------------- |
| Operating System            | Windows 10 / Windows 11                               |
| Build Type                  | Debug                                                 |
| Framework                   | Qt 6.8.0                                              |
| Compiler                    | MSVC 2022                                             |
| Test Command                | `ctest --test-dir build -C Debug --output-on-failure` |
| Expected Automated Baseline | All tests passed                                      |

---

## Pre-Test Setup

Before starting manual QA, verify:

| ID     | Check              | Expected Result                 | Status | Notes |
| ------ | ------------------ | ------------------------------- | ------ | ----- |
| PRE-01 | Pull latest `main` | Repository is up to date        | TODO   |       |
| PRE-02 | Build project      | Build completes successfully    | TODO   |       |
| PRE-03 | Run full CTest     | All automated tests pass        | TODO   |       |
| PRE-04 | Launch application | Application opens without crash | TODO   |       |

Commands:

```powershell
git switch main
git pull
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

---

# 1. Data Loading

| ID    | Area  | Scenario                             | Steps                                                                 | Expected Result                                           | Status | Notes |
| ----- | ----- | ------------------------------------ | --------------------------------------------------------------------- | --------------------------------------------------------- | ------ | ----- |
| DL-01 | Input | Load valid courses and periods files | Select valid `courses.txt` and `periods.txt`, click replace/load data | Data loads successfully and success message appears       | TODO   |       |
| DL-02 | Input | Replace existing data                | Load one valid dataset, then load another dataset using replace       | Old data is cleared and new data is shown                 | TODO   |       |
| DL-03 | Input | Append data                          | Load valid data, then append another valid dataset                    | New records are added, duplicates are skipped if relevant | TODO   |       |
| DL-04 | Input | Missing file path                    | Try loading without selecting one or both files                       | Error message appears, no crash                           | TODO   |       |
| DL-05 | Input | Invalid file format                  | Select malformed input file                                           | Error message appears, no crash                           | TODO   |       |

---

# 2. Program Selection

| ID    | Area     | Scenario                  | Steps                                                | Expected Result                                              | Status | Notes |
| ----- | -------- | ------------------------- | ---------------------------------------------------- | ------------------------------------------------------------ | ------ | ----- |
| PS-01 | Programs | Display program list      | Open input screen                                    | List of available academic programs is displayed             | TODO   |       |
| PS-02 | Programs | Select one program        | Click one program checkbox/card                      | Program is selected and counter updates                      | TODO   |       |
| PS-03 | Programs | Deselect selected program | Click the same selected program again                | Program is removed from selected list                        | TODO   |       |
| PS-04 | Programs | Select up to 5 programs   | Select 5 different programs                          | All 5 are selected successfully                              | TODO   |       |
| PS-05 | Programs | Try selecting 6th program | After 5 programs are selected, try selecting another | 6th program is rejected or ignored; selected count remains 5 | TODO   |       |
| PS-06 | Programs | View courses for program  | Open course list/details for a selected program      | Courses displayed match selected program and filters         | TODO   |       |

---

# 3. Calendar Management

| ID    | Area     | Scenario                | Steps                                | Expected Result                                        | Status | Notes |
| ----- | -------- | ----------------------- | ------------------------------------ | ------------------------------------------------------ | ------ | ----- |
| CM-01 | Calendar | Open calendar screen    | Load data and navigate to calendar   | Calendar screen opens and displays exam period         | TODO   |       |
| CM-02 | Calendar | Active day display      | Review regular exam days             | Active days are visually available                     | TODO   |       |
| CM-03 | Calendar | Exclude active day      | Click an active day                  | Day changes to excluded status                         | TODO   |       |
| CM-04 | Calendar | Re-include excluded day | Click an excluded day                | Day returns to active status                           | TODO   |       |
| CM-05 | Calendar | Saturday locked         | Try to toggle a Saturday             | Saturday remains excluded and cannot become active     | TODO   |       |
| CM-06 | Calendar | Save calendar changes   | Toggle one day and click save        | Save confirmation appears and state remains consistent | TODO   |       |
| CM-07 | Calendar | Period tree navigation  | Use sidebar to select another period | Calendar updates to selected period                    | TODO   |       |
| CM-08 | Calendar | KPI cards update        | Toggle days and observe KPI cards    | Total / active / excluded counts update correctly      | TODO   |       |

---

# 4. Period Editor

| ID    | Area          | Scenario                  | Steps                                      | Expected Result                               | Status | Notes |
| ----- | ------------- | ------------------------- | ------------------------------------------ | --------------------------------------------- | ------ | ----- |
| PE-01 | Period Editor | Open period editor        | From calendar screen click edit period     | Period editor opens                           | TODO   |       |
| PE-02 | Period Editor | Valid start/end shift     | Enter valid new start and end dates, apply | Period dates update and calendar rebuilds     | TODO   |       |
| PE-03 | Period Editor | Invalid date format       | Enter invalid date text                    | Error message appears, no crash               | TODO   |       |
| PE-04 | Period Editor | Start date after end date | Enter start date later than end date       | Error banner appears and change is rejected   | TODO   |       |
| PE-05 | Period Editor | Error auto-dismiss        | Trigger invalid date error and wait        | Error message disappears automatically        | TODO   |       |
| PE-06 | Period Editor | Save after edit           | Apply valid shift and save                 | Changes are saved and visible after returning | TODO   |       |

---

# 5. Settings / Hard Constraints

| ID    | Area     | Scenario                | Steps                                  | Expected Result                                                   | Status | Notes |
| ----- | -------- | ----------------------- | -------------------------------------- | ----------------------------------------------------------------- | ------ | ----- |
| HC-01 | Settings | Open settings screen    | Navigate to settings                   | Settings screen opens without visual issues                       | TODO   |       |
| HC-02 | Settings | Display all 5 rules     | Review settings screen                 | Rules 2.1, 2.2, 2.3, 2.4, 2.5 are displayed                       | TODO   |       |
| HC-03 | Settings | Enable and disable rule | Toggle each rule switch                | Switch state changes correctly                                    | TODO   |       |
| HC-04 | Settings | Change `k` value        | Change `k` for each rule               | Value updates and remains within valid bounds                     | TODO   |       |
| HC-05 | Settings | Save hard constraints   | Enable rules and click save            | Settings are saved successfully                                   | TODO   |       |
| HC-06 | Settings | Reopen settings         | Leave and return to settings screen    | Previously saved values are restored                              | TODO   |       |
| HC-07 | Settings | `kMax` display          | Load exam period and open settings     | Maximum allowed `k` reflects exam period length                   | TODO   |       |
| HC-08 | Settings | Out-of-range `k`        | Try entering `k` above allowed maximum | Save is blocked or value is clamped; no invalid setting is stored | TODO   |       |

---

# 6. Schedule Generation

| ID    | Area       | Scenario                           | Steps                                                           | Expected Result                                 | Status | Notes |
| ----- | ---------- | ---------------------------------- | --------------------------------------------------------------- | ----------------------------------------------- | ------ | ----- |
| SG-01 | Generation | Generate with valid data           | Load data, select program, click generate                       | Schedules are generated successfully            | TODO   |       |
| SG-02 | Generation | Generate without selected program  | Load data but do not select program, click generate if possible | Error appears or action is blocked              | TODO   |       |
| SG-03 | Generation | Generate after calendar edit       | Exclude a day, save, generate schedules                         | No exam is scheduled on excluded day            | TODO   |       |
| SG-04 | Generation | Generate with hard constraints off | Disable hard constraints and generate                           | Valid schedules are produced                    | TODO   |       |
| SG-05 | Generation | Generate with hard constraints on  | Enable selected hard constraints and generate                   | Generated schedules respect enabled constraints | TODO   |       |
| SG-06 | Generation | UI responsiveness                  | Start generation and interact with UI                           | UI does not freeze                              | TODO   |       |
| SG-07 | Generation | No solution case                   | Use strict constraints or impossible data                       | System reports no solutions without crash       | TODO   |       |

---

# 7. Output Screen

| ID    | Area   | Scenario                   | Steps                                                    | Expected Result                                       | Status | Notes |
| ----- | ------ | -------------------------- | -------------------------------------------------------- | ----------------------------------------------------- | ------ | ----- |
| OS-01 | Output | Open output screen         | Generate schedules and navigate to output                | Output screen opens and displays schedule             | TODO   |       |
| OS-02 | Output | Schedule counter           | Review schedule counter                                  | Counter shows current schedule and total schedules    | TODO   |       |
| OS-03 | Output | Next schedule              | Click next                                               | Current schedule index increases and calendar updates | TODO   |       |
| OS-04 | Output | Previous schedule          | Click previous                                           | Current schedule index decreases and calendar updates | TODO   |       |
| OS-05 | Output | Boundary navigation        | Click previous on first schedule / next on last schedule | Index does not go out of bounds                       | TODO   |       |
| OS-06 | Output | Semester filter            | Select semester filter                                   | Display updates to selected semester                  | TODO   |       |
| OS-07 | Output | Moed filter                | Select moed filter                                       | Display updates to selected moed                      | TODO   |       |
| OS-08 | Output | Multiple exams on same day | Use dataset/schedule with more than one exam on same day | All exams on that day are visible                     | TODO   |       |
| OS-09 | Output | Metrics display            | Open output after generation                             | Current schedule metrics are displayed correctly      | TODO   |       |

---

# 8. Sorting / Priorities

| ID    | Area    | Scenario                      | Steps                               | Expected Result                                          | Status | Notes |
| ----- | ------- | ----------------------------- | ----------------------------------- | -------------------------------------------------------- | ------ | ----- |
| SO-01 | Sorting | Open sorting/priorities panel | Navigate to output sorting section  | Sorting options are visible                              | TODO   |       |
| SO-02 | Sorting | Change priority order         | Reorder sorting priorities          | New order is accepted                                    | TODO   |       |
| SO-03 | Sorting | Save priorities               | Save new sorting priority order     | Saved order persists                                     | TODO   |       |
| SO-04 | Sorting | Apply sorting                 | Apply sorting after schedules exist | Displayed schedule order changes according to priorities | TODO   |       |
| SO-05 | Sorting | Empty or default priorities   | Use default priority order          | No crash; schedules remain displayable                   | TODO   |       |

---

# 9. Export

| ID    | Area   | Scenario                 | Steps                                          | Expected Result                                                           | Status | Notes |
| ----- | ------ | ------------------------ | ---------------------------------------------- | ------------------------------------------------------------------------- | ------ | ----- |
| EX-01 | Export | Export current schedule  | Generate schedules, choose export path, export | Text file is created                                                      | TODO   |       |
| EX-02 | Export | Export readable content  | Open exported file                             | File includes course number, course name, instructor, date, semester/moed | TODO   |       |
| EX-03 | Export | Export selected schedule | Navigate to a different schedule and export    | Exported file matches currently displayed schedule                        | TODO   |       |
| EX-04 | Export | Invalid path             | Try exporting to invalid/empty path            | Export is rejected, no crash                                              | TODO   |       |
| EX-05 | Export | Export without schedules | Try export before generation if possible       | Export is blocked safely                                                  | TODO   |       |

---

# 10. GUI / Visual Regression

| ID     | Area   | Scenario               | Steps                                   | Expected Result                                         | Status | Notes |
| ------ | ------ | ---------------------- | --------------------------------------- | ------------------------------------------------------- | ------ | ----- |
| GUI-01 | Layout | Main screen layout     | Open app at normal resolution           | No clipped text or broken layout                        | TODO   |       |
| GUI-02 | Layout | Calendar screen layout | Open calendar screen                    | Calendar grid and headers are aligned                   | TODO   |       |
| GUI-03 | Layout | Settings screen layout | Open settings screen                    | Rule cards fit and scroll correctly                     | TODO   |       |
| GUI-04 | Layout | Output screen layout   | Open output screen                      | Calendar, navigation, filters and sorting UI are usable | TODO   |       |
| GUI-05 | Layout | Hebrew text            | Review all main screens                 | Hebrew labels are readable and not reversed/cut         | TODO   |       |
| GUI-06 | Layout | Small window size      | Resize window to minimum supported size | UI remains usable, no important button disappears       | TODO   |       |

---

# 11. Regression Checklist

| ID     | Area       | Scenario                  | Steps                                                                              | Expected Result                                | Status | Notes |
| ------ | ---------- | ------------------------- | ---------------------------------------------------------------------------------- | ---------------------------------------------- | ------ | ----- |
| REG-01 | Regression | Full automated test suite | Run full CTest                                                                     | All tests pass                                 | TODO   |       |
| REG-02 | Regression | Main user flow            | Load data → select program → edit calendar → settings → generate → output → export | Flow completes without crash                   | TODO   |       |
| REG-03 | Regression | Stage 1 compatibility     | Use basic file-based data and generate schedules                                   | Basic scheduling behavior still works          | TODO   |       |
| REG-04 | Regression | Stage 2 compatibility     | Use GUI calendar and output flow                                                   | GUI flow still works                           | TODO   |       |
| REG-05 | Regression | Stage 3 compatibility     | Enable hard constraints and sorting                                                | Phase 3 behavior works without breaking output | TODO   |       |
| REG-06 | Regression | CI status                 | Open latest GitHub Actions run                                                     | CI build and tests pass                        | TODO   |       |

---

# Final QA Sign-Off

| Field                  | Value       |
| ---------------------- | ----------- |
| Tester                 |             |
| Date                   |             |
| Branch                 |             |
| Commit Hash            |             |
| Automated Tests Result |             |
| Manual QA Result       |             |
| Open Bugs              |             |
| Final Status           | PASS / FAIL |

## Final Notes

```text
```
