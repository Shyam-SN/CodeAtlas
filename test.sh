#!/bin/bash
set -e

# Change directory to the script's directory (CodeAtlas project root)
cd "$(dirname "$0")"

echo "=== 1. Configuring CMake (Release Mode) ==="
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

echo "=== 2. Building CodeAtlas ==="
# Detect available CPU cores portably across macOS, Linux, and BSD
if command -v nproc >/dev/null 2>&1; then
    NPROC=$(nproc)
elif command -v sysctl >/dev/null 2>&1; then
    NPROC=$(sysctl -n hw.ncpu 2>/dev/null || echo 4)
else
    NPROC=4
fi

cmake --build build -j"$NPROC"

echo "=== 3. Running Unit Tests ==="
./build/tests/codeatlas_tests

echo "=== 4. Running CLI Example Demo ==="
./build/examples/demo_codeatlas_cli

echo "=== 5. Running Benchmarks ==="
./build/benchmarks/codeatlas_benchmarks

if [ -f "./build/qt_app/codeatlas_qt_app" ]; then
    echo ""
    echo "=== CodeAtlas Qt Desktop Studio is Ready! ==="
    echo "To launch the GUI desktop app & AI Visualizer, run:"
    echo "  ./build/qt_app/codeatlas_qt_app"
fi

echo "=== CodeAtlas Build, Test, and Benchmarking Completed Successfully! ==="
