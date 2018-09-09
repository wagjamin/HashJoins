# ---------------------------------------------------------------------------
# Seminar Template
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Files
# ---------------------------------------------------------------------------

set(SRC_LIB_EXAMPLE_H
    "${CMAKE_SOURCE_DIR}/joins/include/example/bubble_sort.h"
        "${CMAKE_SOURCE_DIR}/joins/include/algorithms/join.h"
        "${CMAKE_SOURCE_DIR}/joins/include/algorithms/tuple.h"
        "${CMAKE_SOURCE_DIR}/joins/include/generators/uniform_generator.h"
        "${CMAKE_SOURCE_DIR}/joins/include/generators/zipf_generator.h"
    )

set(SRC_LIB_EXAMPLE_CC
    "${CMAKE_SOURCE_DIR}/joins/src/example/bubble_sort.cc"
    )

set(TEST_LIB_EXAMPLE_CC
    "${CMAKE_SOURCE_DIR}/joins/test/example/bubble_sort_test.cc"
    "${CMAKE_SOURCE_DIR}/joins/test/tester.cc"
    )

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------

add_library(example STATIC ${SRC_LIB_EXAMPLE_CC})

target_compile_options(example PUBLIC -Werror)
target_include_directories(example PUBLIC
    "${CMAKE_SOURCE_DIR}/joins/include")
target_link_libraries(example
    Threads::Threads
    )

# ---------------------------------------------------------------------------
# Test driver
# ---------------------------------------------------------------------------

add_executable(tester_example "${TEST_LIB_EXAMPLE_CC}")
target_link_libraries(tester_example
    example
    gtest
    gmock
    Threads::Threads)

add_test(NAME example
    COMMAND tester_example)
list(APPEND test_drivers tester_example)

# ---------------------------------------------------------------------------
# Linting
# ---------------------------------------------------------------------------

add_cpplint_target(lint_example "${SRC_LIB_EXAMPLE_H};${SRC_LIB_EXAMPLE_CC};${TEST_LIB_EXAMPLE_CC}")
list(APPEND lint_targets lint_example)