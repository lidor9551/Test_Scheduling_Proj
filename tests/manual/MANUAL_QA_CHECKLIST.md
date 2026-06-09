# Manual QA Checklist

This checklist defines manual regression tests for the Exam Scheduler application.

Manual QA should be executed before releasing a new version or merging major UI/scheduling changes.

## Environment

Tester name:

Date:

Operating system:

Branch / commit:

Qt version:

Build command used:

Test result:

- [ ] Pass
- [ ] Fail

---

## 1. Application Startup

### QA-MANUAL-001 - Application launches successfully

Steps:

1. Build the project.
2. Run the application executable.
3. Verify that the main window opens.

Expected result:

- Application opens without crashing.
- No fatal QML loading errors are shown.
- Main screen is visible.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

## 2. Input File Loading

### QA-MANUAL-002 - Load valid course and exam-period files

Steps:

1. Open the application.
2. Load a valid `courses.txt` file.
3. Load a valid `periods.txt` or `exam_periods.txt` file.
4. Load a valid `selected_programs.txt` file if supported by the UI.
5. Continue to the next flow.

Expected result:

- Files are accepted.
- No crash occurs.
- User receives a clear success indication or can proceed.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

### QA-MANUAL-003 - Reject invalid input file

Steps:

1. Open the application.
2. Load an invalid courses file, such as `tests/data/invalid/missing-fields/courses.txt`.
3. Try to continue.

Expected result:

- Application does not crash.
- Invalid input is rejected or a clear error message is shown.
- Scheduler is not executed using invalid data.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

## 3. Program Selection

### QA-MANUAL-004 - Select study programs

Steps:

1. Load valid input data.
2. Open the program selection area.
3. Select one or more study programs.
4. Continue to scheduling.

Expected result:

- Selected programs are displayed correctly.
- User can change the selection before generating schedules.
- Only courses relevant to selected programs are used.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

## 4. Schedule Generation

### QA-MANUAL-005 - Generate schedule from valid data

Steps:

1. Load the valid simple dataset.
2. Select the relevant program.
3. Start schedule generation.

Expected result:

- Scheduler completes successfully.
- At least one schedule is generated.
- No exams are scheduled outside the exam period.
- No exams are scheduled on excluded dates.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

### QA-MANUAL-006 - No-solution case

Steps:

1. Load `tests/data/no_solution/impossible-schedule`.
2. Start schedule generation.

Expected result:

- Application does not crash or freeze.
- User receives a clear no-solution message.
- No invalid schedule is displayed as valid.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

## 5. Calendar Output Screen

### QA-MANUAL-007 - Display generated schedules

Steps:

1. Generate schedules from valid input.
2. Navigate to the output/calendar screen.
3. Review the generated schedule.

Expected result:

- Calendar screen opens correctly.
- Exams are shown on the correct dates.
- Course names or identifiers are readable.
- User can understand which schedule is currently displayed.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

### QA-MANUAL-008 - Navigate between generated schedules

Steps:

1. Generate multiple schedules if available.
2. Navigate between generated schedule options.

Expected result:

- User can move between generated schedules.
- Display updates correctly.
- No stale data remains from previous schedules.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

## 6. Calendar Editing and Save Behavior

### QA-MANUAL-009 - Save calendar edits

Steps:

1. Open a generated schedule.
2. Modify the displayed calendar if editing is supported.
3. Click `Save`.
4. Leave and return to the calendar screen.

Expected result:

- Save button preserves calendar changes.
- Save does not trigger file export.
- User receives a clear save confirmation.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

## 7. Export / Save as File

### QA-MANUAL-010 - Export selected generated schedule

Steps:

1. Generate schedules.
2. Choose one generated schedule.
3. Click the export or save-as-file button.
4. Select output location if prompted.

Expected result:

- Only the selected generated schedule is exported.
- Export action does not overwrite calendar-edit save behavior.
- Exported file is readable.
- Exported format matches the format defined by the project requirements.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

## 8. UI and Layout

### QA-MANUAL-011 - Basic layout validation

Steps:

1. Open the application.
2. Navigate through main screens.
3. Resize the window.
4. Check buttons, text fields, and calendar layout.

Expected result:

- No important UI element is cut off.
- Buttons are clickable.
- Text is readable.
- Layout remains usable after resizing.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

## 9. Cross-platform Smoke Test

### QA-MANUAL-012 - macOS smoke test

Steps:

1. Build on macOS.
2. Run automated tests.
3. Launch the application.
4. Execute one valid scheduling flow.

Expected result:

- Build succeeds.
- Automated tests pass.
- Application launches.
- Basic scheduling flow works.

Result:

- [ ] Pass
- [ ] Fail

Notes:

---

### QA-MANUAL-013 - Windows smoke test

Steps:

1. Set Qt path in PowerShell.
2. Build on Windows.
3. Run automated tests.
4. Launch the application.
5. Execute one valid scheduling flow.

Expected result:

- Build succeeds.
- Automated tests pass.
- Application launches.
- Basic scheduling flow works.

PowerShell reminder:

```powershell
$env:PATH = "C:\Qt\6.8.0\msvc2022_64\bin;$env:PATH"
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\msvc2022_64"
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure