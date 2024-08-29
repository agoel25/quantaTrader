cmake_minimum_required(VERSION 3.10)
project(quantaTrader LANGUAGES CXX)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
list(APPEND SOURCES benchmark/benchmark_engine.cpp)
list(APPEND SOURCES benchmark/generate_orders.cpp)

find_package(Boost REQUIRED)
find_package(benchmark REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(libs)
include_directories(include)
include_directories(include/matching)
include_directories(include/event_handling)
include_directories(include/utils)
include_directories(${Boost_INCLUDE_DIRS})

enable_testing()

# add_executable(main src/main.cpp src/matching/order.cpp)
# add_executable(main ${SOURCES})
add_executable(benchmark_engine benchmark/benchmark_engine.cpp ${SOURCES})

target_link_libraries(benchmark_engine Boost::boost benchmark::benchmark)