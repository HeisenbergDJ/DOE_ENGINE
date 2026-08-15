#!/usr/bin/env bash
# Quick test runner for Linux/macOS: builds the engine and runs every fixture.
set -euo pipefail

cd "$(dirname "$0")/.."

if [[ ! -f build/CMakeCache.txt ]]; then
    cmake -S . -B build -G Ninja
else
    echo "Reusing existing build configuration."
fi
cmake --build build

failed=0
for fixture in delimited_text_experiment multiple_extractors_experiment placeholder_text_experiment; do
    echo
    echo "=== ${fixture} ==="
    if ! ./build/doe_engine "build/tests/fixtures/${fixture}/experiment.json"; then
        failed=1
    fi
done

echo
if [[ ${failed} -eq 1 ]]; then
    echo "Some fixtures failed."
    exit 1
fi
echo "All fixtures passed."
exit 0
