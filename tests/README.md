# Tests

This directory contains the automated and manual QA assets for the Exam Scheduler project.

## Test Types

### Unit Tests

Unit tests verify isolated core logic.

Current unit/model-related tests:

- `ProgramCourseModelTest`

### Integration Tests

Integration tests verify that multiple system components work together.

Current integration tests:

- `ParserPreprocessorIntegrationTest`
  - Verifies parser-to-preprocessor flow.
  - Uses `tests/data/valid/simple`.

- `SchedulerIntegrationTest`
  - Verifies parser-to-preprocessor-to-scheduler flow.
  - Uses `tests/data/valid/simple`.

- `InvalidInputIntegrationTest`
  - Verifies malformed input is rejected.
  - Uses `tests/data/invalid/missing-fields`.

- `NoSolutionIntegrationTest`
  - Verifies the scheduler returns no solutions when scheduling is impossible.
  - Uses `tests/data/no_solution/impossible-schedule`.

- `OverlapConflictIntegrationTest`
  - Verifies same-program obligatory exam conflicts are rejected.
  - Uses `tests/data/overlap/same-program-conflict`.

## Test Data

Test datasets are located under:

```text
tests/data/
Available datasets:
tests/data/valid/simple
tests/data/invalid/missing-fields
tests/data/no_solution/impossible-schedule
tests/data/overlap/same-program-conflict
Each dataset contains input files and an expected.md file describing the expected behavior.

Running Tests on macOS / Linux

From the project root:
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

Running Tests on Windows

Before running tests, make sure Qt DLLs are available in the current PowerShell session:
$env:PATH = "C:\Qt\6.8.0\msvc2022_64\bin;$env:PATH"

Then run:
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\msvc2022_64"
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure