#!/bin/bash
set -e
cd "$(dirname "$0")/.."
mkdir -p tests_generated_50_outputs

if [ ! -x ./build/exam_scheduler ]; then
  rm -rf build
  cmake -S . -B build
  cmake --build build
fi

count=0
pass=0
for dir in tests_generated_50/unit/* tests_generated_50/integration/* tests_generated_50/no_solution/* tests_generated_50/limits/*; do
  [ -d "$dir" ] || continue
  name=$(echo "$dir" | tr '/' '_')
  out="tests_generated_50_outputs/${name}.txt"
  echo "Running $dir"
  set +e
  ./build/exam_scheduler     --courses "$dir/courses.txt"     --periods "$dir/exam_periods.txt"     --selected-programs "$dir/selected_programs.txt"     --output "$out"     --combined-limit 1000     --per-block-limit 1000     --max-runtime-seconds 20
  code=$?
  set -e
  count=$((count+1))
  if [ $code -eq 0 ]; then
    pass=$((pass+1))
  else
    echo "FAILED: $dir exit=$code" >> tests_generated_50_outputs/failures.txt
  fi
done

echo "Done. Passed $pass / $count"
echo "Outputs are in tests_generated_50_outputs/"
