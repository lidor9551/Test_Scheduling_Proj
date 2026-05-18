#!/bin/bash
set -e
ROOT_DIR="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT_DIR"
./build/exam_scheduler \
  --courses tests_generated_50/limits/limit_05/courses.txt \
  --periods tests_generated_50/limits/limit_05/exam_periods.txt \
  --selected-programs tests_generated_50/limits/limit_05/selected_programs.txt \
  --output tests_generated_50/limits/limit_05/output.txt \
  --combined-limit 1000000 \
  --per-block-limit 1000000 \
  --max-runtime-seconds 60
