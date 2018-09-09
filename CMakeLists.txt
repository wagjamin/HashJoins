cmake_minimum_required(VERSION 3.10)
project(HashJoins)

# ---------------------------------------------------------------------------
# Environment
# ---------------------------------------------------------------------------

if (NOT UNIX)
    message(FATAL_ERROR "Unsupported OS")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT NO_ASAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif ()

if (NOT NO_COVERAGE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage -fprofile-arcs -ftest-coverage")
endif()

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

find_package(Threads REQUIRED)

find_program(GCOV_PATH gcov OPTIONAL)
find_program(LCOV_PATH lcov OPTIONAL)
find_program(GENHTML_PATH genhtml OPTIONAL)

include("${CMAKE_SOURCE_DIR}/cmake/coverage.cmake")
include("${CMAKE_SOURCE_DIR}/vendor/cpplint.cmake")
include("${CMAKE_SOURCE_DIR}/vendor/googletest.cmake")
include("${CMAKE_SOURCE_DIR}/vendor/benchmark.cmake")

# ---------------------------------------------------------------------------
# Modules
# ---------------------------------------------------------------------------

enable_testing()

include("${CMAKE_SOURCE_DIR}/benchmark/local.cmake")
include("${CMAKE_SOURCE_DIR}/joins/local.cmake")

add_custom_target(check
    COMMAND CTEST_OUTPUT_ON_FAILURE=1 ${CMAKE_CTEST_COMMAND}
    DEPENDS ${test_drivers})
add_coverage_target(coverage ${test_drivers})

# ---------------------------------------------------------------------------
# Linting
# ---------------------------------------------------------------------------

add_custom_target(lint)
add_dependencies(lint ${lint_targets})

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

message(STATUS "[BENCHMARK] settings")
message(STATUS "    BENCHMARK_INCLUDE_DIR       = ${BENCHMARK_INCLUDE_DIR}")
message(STATUS "    BENCHMARK_LIBRARY_PATH      = ${BENCHMARK_LIBRARY_PATH}")
message(STATUS "[TEST] settings")
message(STATUS "    GTEST_INCLUDE_DIR           = ${GTEST_INCLUDE_DIR}")
message(STATUS "    GTEST_LIBRARY_PATH          = ${GTEST_LIBRARY_PATH}")
message(STATUS "[COVERAGE] settings")
message(STATUS "    GCOV_PATH                   = ${GCOV_PATH}")
message(STATUS "    LCOV_PATH                   = ${LCOV_PATH}")
message(STATUS "    GENHTML_PATH                = ${GENHTML_PATH}")