#!/bin/bash
set -e
ROOT_DIR="$(cd "$(dirname "$0")/../../.." && pwd)"
cd "$ROOT_DIR"
./build/exam_scheduler \
  --courses tests_generated_50/limits/limit_01/courses.txt \
  --periods tests_generated_50/limits/limit_01/exam_periods.txt \
  --selected-programs tests_generated_50/limits/limit_01/selected_programs.txt \
  --output tests_generated_50/limits/limit_01/output.txt \
  --combined-limit 50 \
  --per-block-limit 50 \
  --max-runtime-seconds 60
