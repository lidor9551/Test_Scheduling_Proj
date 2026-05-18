# tests_generated_50

Generated QA suite for the exam scheduling project.

Structure:
- `unit/` — 15 focused tests.
- `integration/` — 20 larger tests with 5 programs, 4 years, overlapping courses.
- `no_solution/` — 10 intentionally impossible cases.
- `limits/` — 5 heavy tests meant to run with output/runtime limits.

Each test folder contains:
- `courses.txt`
- `exam_periods.txt`
- `selected_programs.txt`

Copy this folder into the project root: `~/Desktop/Test_Scheduling_Proj/`.
Then run `./tests_generated_50/run_all_limited.sh` from the project root.
