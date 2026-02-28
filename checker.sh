#!/bin/bash

set -u

if [ $# -lt 1 ]; then
  echo "Usage: $0 <executable> [tests_dir]" >&2
  echo "Example: $0 ./train tests" >&2
  exit 2
fi

EXECUTABLE="$1"
TESTS_DIR="${2:-tests}"
INPUT_DIR="${TESTS_DIR}/Input"
OUTPUT_DIR="${TESTS_DIR}/Output"

if [ ! -x "$EXECUTABLE" ]; then
  echo "Error: executable not found or not executable: $EXECUTABLE" >&2
  exit 2
fi

if [ ! -d "$INPUT_DIR" ]; then
  echo "Error: input directory not found: $INPUT_DIR" >&2
  exit 2
fi

if [ ! -d "$OUTPUT_DIR" ]; then
  echo "Error: output directory not found: $OUTPUT_DIR" >&2
  exit 2
fi

shopt -s nullglob

total=0
passed=0
failed=0

for input_file in "$INPUT_DIR"/*.in; do
  base_name="$(basename "$input_file" .in)"
  expected_file="${OUTPUT_DIR}/${base_name}.out"

  if [ ! -f "$expected_file" ]; then
    echo "[SKIP] Missing expected output for ${base_name}" >&2
    continue
  fi

  total=$((total + 1))

  actual_file="$(mktemp)"
  "$EXECUTABLE" "$input_file" > "$actual_file" 2>/dev/null

  if diff -u "$expected_file" "$actual_file" >/dev/null; then
    echo "[PASS] ${base_name}"
    passed=$((passed + 1))
  else
    echo "[FAIL] ${base_name}"
    diff -u "$expected_file" "$actual_file"
    failed=$((failed + 1))
  fi

  rm -f "$actual_file"
done

echo "Summary: ${passed}/${total} passed, ${failed} failed"

if [ "$failed" -ne 0 ]; then
  exit 1
fi
