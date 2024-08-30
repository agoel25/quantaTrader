# project setup
cmake_minimum_required(VERSION 3.10)
project(quantaTrader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

# setup source files for all executables 
file(GLOB_RECURSE BENCHMARK_SOURCES "src/*.cpp")
list(APPEND BENCHMARK_SOURCES benchmark/benchmark_engine.cpp)
list(APPEND BENCHMARK_SOURCES benchmark/generate_orders.cpp)

file(GLOB_RECURSE SAMPLE_SOURCES "src/*.cpp")
list(APPEND SAMPLE_SOURCES sample/engine_sample.cpp)

# find and include packages and native files
find_package(Boost REQUIRED)
find_package(benchmark REQUIRED)

include_directories(libs)
include_directories(include)
include_directories(include/matching)
include_directories(include/event_handling)
include_directories(include/utils)
include_directories(${Boost_INCLUDE_DIRS})

# define executables with their respective source files
add_executable(benchmark_engine benchmark/benchmark_engine.cpp ${BENCHMARK_SOURCES})
target_link_libraries(benchmark_engine Boost::boost benchmark::benchmark)

add_executable(engine_sample sample/engine_sample.cpp ${SAMPLE_SOURCES})