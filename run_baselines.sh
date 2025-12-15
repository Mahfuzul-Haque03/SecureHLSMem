#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT_DIR="${ROOT_DIR}/baseline_logs"

mkdir -p "${OUT_DIR}/clang"
mkdir -p "${OUT_DIR}/cppcheck"
mkdir -p "${OUT_DIR}/scanbuild"

# Find all .c files under the root
C_FILES=$(find "${ROOT_DIR}" -maxdepth 3 -type f -name "*.c" ! -name "main_test.c")

echo "Found files:"
echo "${C_FILES}"

# 1) clang -Wall -Wextra
echo "Running clang -Wall -Wextra..."
for f in ${C_FILES}; do
  rel="${f#$ROOT_DIR/}"
  log="${OUT_DIR}/clang/$(echo "${rel}" | tr '/' '_').log"
  # -fsyntax-only: just check, don't produce object files
  clang -Wall -Wextra -fsyntax-only "${f}" > "${log}" 2>&1 || true
done

# 2) cppcheck
echo "Running cppcheck..."
for f in ${C_FILES}; do
  rel="${f#$ROOT_DIR/}"
  log="${OUT_DIR}/cppcheck/$(echo "${rel}" | tr '/' '_').log"
  # Template: ID::file::line::message for easy parsing
  cppcheck --enable=warning,style,performance,portability,information \
           --template='{id}::{file}::{line}::{message}' \
           "${f}" > "${log}" 2>&1 || true
done

# 3) Clang Static Analyzer (scan-build)
echo "Running scan-build..."
for f in ${C_FILES}; do
  rel="${f#$ROOT_DIR/}"
  base_log="${OUT_DIR}/scanbuild/$(echo "${rel}" | tr '/' '_')"

  # scan-build normally wraps a build system, but we can use it per-file:
  # -o sets output directory for HTML; we also capture stderr text.
  scan-build -o "${base_log}_reports" \
    clang -c "${f}" -o /dev/null > "${base_log}.log" 2>&1 || true
done

echo "Baseline analysis finished. Logs in ${OUT_DIR}"
