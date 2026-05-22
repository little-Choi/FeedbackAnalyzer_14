#!/usr/bin/env bash
# Regenerate tests/expected/* golden files from current application output.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${1:-build}"
cd "$ROOT"

if [[ ! -x "${BUILD_DIR}/tests/feedback_analyzer_tests" &&
      ! -f "${BUILD_DIR}/tests/feedback_analyzer_tests.exe" ]]; then
  cmake -S . -B "${BUILD_DIR}" -DBUILD_TESTING=ON
  cmake --build "${BUILD_DIR}" --target feedback_analyzer_tests
fi

EXE="${BUILD_DIR}/tests/feedback_analyzer_tests"
[[ -f "${BUILD_DIR}/tests/feedback_analyzer_tests.exe" ]] && EXE="${BUILD_DIR}/tests/feedback_analyzer_tests.exe"

FEEDBACK_ANALYZER_UPDATE_GOLDEN=1 "${EXE}" "[golden]" --reporter compact
echo "Golden files updated under tests/expected/"
