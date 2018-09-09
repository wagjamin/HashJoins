// ---------------------------------------------------------------------------
// Seminar Template
// ---------------------------------------------------------------------------
#include <chrono>
#include <numeric>
#include <random>
#include "benchmark/benchmark.h"
#include "example/bubble_sort.h"
// ---------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------
void BenchmarkBubbleSort(benchmark::State& state) {
    uint64_t size = state.range(0);

    std::mt19937 engine(42);
    std::vector<uint32_t> vector;
    vector.resize(size);

    std::iota(vector.begin(), vector.end(), 0);

    example::BubbleSort sort;

    for (auto _ : state) {
        std::shuffle(vector.begin(), vector.end(), engine);

        auto start = std::chrono::high_resolution_clock::now();

        sort(vector);
        benchmark::ClobberMemory();

        auto end = std::chrono::high_resolution_clock::now();

        state.SetIterationTime(std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count());
    }

    state.SetItemsProcessed(size * state.iterations());
    state.SetBytesProcessed(size * state.iterations() * sizeof(uint32_t));
}
// ---------------------------------------------------------------------------
void BenchmarkStdSort(benchmark::State& state) {
    uint64_t size = state.range(0);

    std::mt19937 engine(42);
    std::vector<uint32_t> vector;
    vector.resize(size);

    std::iota(vector.begin(), vector.end(), 0);

    example::BubbleSort sort;

    for (auto _ : state) {
        std::shuffle(vector.begin(), vector.end(), engine);

        auto start = std::chrono::high_resolution_clock::now();

        std::sort(vector.begin(), vector.end());
        benchmark::ClobberMemory();

        auto end = std::chrono::high_resolution_clock::now();

        state.SetIterationTime(std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count());
    }

    state.SetItemsProcessed(size * state.iterations());
    state.SetBytesProcessed(size * state.iterations() * sizeof(uint32_t));
}
// ---------------------------------------------------------------------------
}  // namespace
// ---------------------------------------------------------------------------
BENCHMARK(BenchmarkBubbleSort)->RangeMultiplier(2)->Range(1u << 10u, 1u << 15u)->UseManualTime();
BENCHMARK(BenchmarkStdSort)->RangeMultiplier(2)->Range(1u << 10u, 1u << 15u)->UseManualTime();
// ---------------------------------------------------------------------------
BENCHMARK_MAIN();
// ---------------------------------------------------------------------------
