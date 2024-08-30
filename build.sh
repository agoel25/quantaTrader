#!/bin/bash

# define the build directory
BUILD_DIR="build"

# delete the existing build directory if it exists
if [ -d "$BUILD_DIR" ]; then
  echo "Deleting existing build directory: $BUILD_DIR"
  rm -rf "$BUILD_DIR"
fi

# create the build directory
echo "Creating build directory: $BUILD_DIR"
mkdir "$BUILD_DIR"

# navigate to the build directory
cd "$BUILD_DIR" || exit

# run CMake
echo "Running CMake..."
cmake ..

# build the project
echo "Building the project..."
make

# run the benchmark
echo "Running the benchmark..."
./benchmark_engine

# define colors for echo statements
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "Build and benchmark SUCCESSFUL"
echo -e "Benchmark executable: ${CYAN}build/benchmark_engine${NC}"
echo -e "Sample executable: ${CYAN}build/engine_sample${NC}"