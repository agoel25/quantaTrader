cmake_minimum_required(VERSION 3.10)
project(quantaTrader LANGUAGES CXX)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(libs)
include_directories(include)
include_directories(include/matching)
include_directories(include/event_handling)
include_directories(include/utils)
include_directories(${Boost_INCLUDE_DIRS})

enable_testing()

add_executable(main src/main.cpp src/matching/order.cpp)
target_link_libraries(main Boost::boost)