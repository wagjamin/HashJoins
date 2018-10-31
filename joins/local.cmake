#----------------------------------------------------------------------------
# Files
# ---------------------------------------------------------------------------

set(SRC_LIB_EXAMPLE_H
        "${CMAKE_SOURCE_DIR}/joins/include/example/bubble_sort.h"
        "${CMAKE_SOURCE_DIR}/joins/include/algorithms/nop_join.h"
        "${CMAKE_SOURCE_DIR}/joins/include/algorithms/radix_join.h"
        "${CMAKE_SOURCE_DIR}/joins/include/algorithms/nop_join_mt.h"
        "${CMAKE_SOURCE_DIR}/joins/include/algorithms/mt_radix/radix_join_mt.h"
        "${CMAKE_SOURCE_DIR}/joins/include/algorithms/mt_radix/radix_tasks.h"
        "${CMAKE_SOURCE_DIR}/joins/lib/ThreadPool.h"
        "${CMAKE_SOURCE_DIR}/joins/include/generators/uniform_generator.h"
    )

set(SRC_LIB_EXAMPLE_CC
        "${CMAKE_SOURCE_DIR}/joins/src/example/bubble_sort.cc"
        "${CMAKE_SOURCE_DIR}/joins/src/generators/uniform_generator.cpp"
        "${CMAKE_SOURCE_DIR}/joins/src/algorithms/nop_join.cpp"
        "${CMAKE_SOURCE_DIR}/joins/src/algorithms/radix_join.cpp"
        "${CMAKE_SOURCE_DIR}/joins/src/algorithms/nop_join_mt.cpp"
        "${CMAKE_SOURCE_DIR}/joins/src/algorithms/mt_radix/radix_join_mt.cpp"
        "${CMAKE_SOURCE_DIR}/joins/src/algorithms/mt_radix/radix_tasks.cpp"
    )

set(TEST_LIB_EXAMPLE_CC
        "${CMAKE_SOURCE_DIR}/joins/test/tester.cc"
        "${CMAKE_SOURCE_DIR}/joins/test/example/bubble_sort_test.cc"
        "${CMAKE_SOURCE_DIR}/joins/test/generators/uniform_generator_test.cpp"
        "${CMAKE_SOURCE_DIR}/joins/test/algorithms/nop_join_test.cpp"
        "${CMAKE_SOURCE_DIR}/joins/test/algorithms/nop_join_mt_test.cpp"
        "${CMAKE_SOURCE_DIR}/joins/test/algorithms/radix_join_test.cpp"
        "${CMAKE_SOURCE_DIR}/joins/test/algorithms/radix_join_mt_test.cpp"
    )

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------

add_library(example STATIC ${SRC_LIB_EXAMPLE_CC})

target_compile_options(example PUBLIC -Werror)
target_include_directories(example PUBLIC
    "${CMAKE_SOURCE_DIR}/joins/include"
    "${CMAKE_SOURCE_DIR}/joins/lib")
target_link_libraries(example
    Threads::Threads
    )

# ---------------------------------------------------------------------------
# Test driver
# ---------------------------------------------------------------------------

add_executable(join_tester "${TEST_LIB_EXAMPLE_CC}")
target_link_libraries(join_tester
    example
    gtest
    gmock
    Threads::Threads)

add_test(NAME JoinTest
    COMMAND join_tester)
list(APPEND test_drivers join_tester)

# ---------------------------------------------------------------------------
# Linting
# ---------------------------------------------------------------------------

add_cpplint_target(lint_example "${SRC_LIB_EXAMPLE_H};${SRC_LIB_EXAMPLE_CC};${TEST_LIB_EXAMPLE_CC}")
list(APPEND lint_targets lint_example)