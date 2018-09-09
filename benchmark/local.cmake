# ---------------------------------------------------------------------------
# Seminar Template
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Files
# ---------------------------------------------------------------------------

set(BENCHMARK_CC
    "${CMAKE_SOURCE_DIR}/benchmark/bm_bubble_sort.cc"
    )

# ---------------------------------------------------------------------------
# Benchmarks
# ---------------------------------------------------------------------------

add_executable(bm_bubble_sort
    "${CMAKE_SOURCE_DIR}/benchmark/bm_bubble_sort.cc"
    )
target_compile_options(bm_bubble_sort PUBLIC -Werror)
target_link_libraries(bm_bubble_sort
    example
    benchmark
    gtest
    Threads::Threads
    )
list(APPEND benchmark_targets bm_bubble_sort)

add_custom_target(benchmarks)
add_dependencies(benchmarks
    ${benchmark_targets})

# ---------------------------------------------------------------------------
# Linting
# ---------------------------------------------------------------------------

add_cpplint_target(lint_benchmark ${BENCHMARK_CC})
list(APPEND lint_targets lint_benchmark)
