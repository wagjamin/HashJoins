# ---------------------------------------------------------------------------
# Files
# ---------------------------------------------------------------------------

set(BENCHMARK_CC
    "${CMAKE_SOURCE_DIR}/benchmark/bm_joins_mt.cpp"
    )

# ---------------------------------------------------------------------------
# Benchmarks
# ---------------------------------------------------------------------------

add_executable(bm_joins
    "${CMAKE_SOURCE_DIR}/benchmark/bm_joins_mt.cpp"
    )
target_compile_options(bm_joins PUBLIC -Werror -O3)
target_link_libraries(bm_joins
    joins
    benchmark
    gtest
    Threads::Threads
    )
list(APPEND benchmark_targets bm_joins)

add_custom_target(benchmarks)
add_dependencies(benchmarks
    ${benchmark_targets})

# ---------------------------------------------------------------------------
# Linting
# ---------------------------------------------------------------------------

add_cpplint_target(lint_benchmark ${BENCHMARK_CC})
list(APPEND lint_targets lint_benchmark)
