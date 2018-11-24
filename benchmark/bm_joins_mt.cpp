//
// Created by benjamin on 01.11.18.
//

#include <chrono>
#include <iostream>
#include "benchmark/benchmark.h"
#include "algorithms/nop_join_mt.h"
#include "algorithms/mt_radix/radix_join_mt.h"
#include "generators/uniform_generator.h"
#include "generators/incremental_generator.h"
#include "generators/zipf_generator.h"

/// The fixed seed used for benchmarking purposes
#define SEED 0

namespace {

    /**
     * Benchmark the NOP join, the input arguments are the following:
     * First:  Size of left Join Side
     * Second: Size of right Join Side
     * Third:  Number of Threads
     * Fourth: 0 if Uniform, 1 if Zipf
     * Optional: (Fifth: Zipf offset (alpha=offset*0.25))
     */
    void BenchmarkNOP(benchmark::State &state) {
        // Get dataset size
        auto data_size_l = static_cast<uint64_t >(state.range(0));
        auto data_size_r = static_cast<uint64_t >(state.range(1));
        // Get thread count
        auto threads = static_cast<uint8_t>(state.range(2));

        std::vector<std::tuple<uint64_t, uint64_t>> build;
        std::vector<std::tuple<uint64_t, uint64_t>> probe;
        // After this block we can forget the generators again
        {
            generators::incremental_generator gen0(1, data_size_l);
            gen0.build();
            build = gen0.get_vec_copy();
            // Uniform Distribution
            if (state.range(3) == 0) {
                generators::uniform_generator gen1(1, data_size_l, data_size_r, SEED);
                gen1.build();
                probe = gen1.get_vec_copy();
            }
            // Zipf Distribution
            else {
                // Zipf Alpha
                double alpha = state.range(4) * 0.25;
                generators::zipf_generator gen1(10000, alpha, data_size_r, SEED);
                gen1.build();
                probe = gen1.get_vec_copy();
            }
        }

        for (auto _ : state) {
            state.PauseTiming();
            // Create copies of the generated arrays for the current task
            std::vector<std::tuple<uint64_t, uint64_t>> build_temp = build;
            std::vector<std::tuple<uint64_t, uint64_t>> probe_temp = probe;
            // Create NOP Join
            algorithms::nop_join_mt join(build_temp.data(), probe_temp.data(), data_size_l, data_size_r, 1.0,
                                         threads);
            state.ResumeTiming();
            // Execute the actual join
            join.execute();
        }

        state.SetItemsProcessed((data_size_r + data_size_l) * state.iterations());
    }

    /**
     * Benchmark the Radix join, the input arguments are the following:
     * First:  Size of left Join Side
     * Second: Size of right Join Side
     * Third:  Number of Threads
     * Fourth: Number of Radix Passes
     * Fifth:  Number of Radix Bits per pass
     * Sixth:  0 if Uniform, 1 if Zipf
     * Optional: (Sixth: Zipf offset (alpha=offset*0.25))
   */
    void BenchmarkRPJ(benchmark::State &state) {
        // Get dataset size
        auto data_size_l = static_cast<uint64_t >(state.range(0));
        auto data_size_r = static_cast<uint64_t >(state.range(1));
        // Get thread count
        auto threads = static_cast<uint8_t>(state.range(2));
        // Get Radix Partition info
        auto runs = static_cast<uint8_t>(state.range(3));
        auto bits = static_cast<uint8_t>(state.range(4));

        std::vector<std::tuple<uint64_t, uint64_t>> build;
        std::vector<std::tuple<uint64_t, uint64_t>> probe;
        // After this block we can forget the generators again
        {
            generators::incremental_generator gen0(1, data_size_l);
            gen0.build();
            build = gen0.get_vec_copy();
            // Uniform Distribution
            if (state.range(5) == 0) {
                generators::uniform_generator gen1(1, data_size_l, data_size_r, SEED);
                gen1.build();
                probe = gen1.get_vec_copy();
            }
            // Zipf Distribution
            else {
                // Zipf Alpha
                double alpha = state.range(6) * 0.25;
                generators::zipf_generator gen1(10000, alpha, data_size_r, SEED);
                gen1.build();
                probe = gen1.get_vec_copy();
            }
        }

        for (auto _ : state) {
            state.PauseTiming();
            // Create copies of the generated arrays for the current task
            std::vector<std::tuple<uint64_t, uint64_t>> build_temp = build;
            std::vector<std::tuple<uint64_t, uint64_t>> probe_temp = probe;
            // Create NOP Join
            algorithms::radix_join_mt join(build_temp.data(), probe_temp.data(), data_size_l, data_size_r, 1.0,
                                           threads, bits, runs);
            state.ResumeTiming();
            // Execute the actual join
            join.execute();
        }

        state.SetItemsProcessed((data_size_r + data_size_l) * state.iterations());
    }


    // Static member containing the threads the uniform benchmarks should be run on
    static std::vector<int64_t> threads{1, 2, 3, 4, 5, 7, 10, 12, 14, 15, 17, 20}; // NOLINT
    // The threads the zipf benchmarks should be run on
    static std::vector<int64_t> zipf_threads{20}; // NOLINT

    // Applying Thread and Size Arguments onto NOP Join
    void NOPArgsUniform(benchmark::internal::Benchmark *b) {
        // Run on Same Sized Input
        int64_t same_size_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(24);
        for (int64_t k: threads) {
            b->Args({same_size_count, same_size_count, k, 0});
        }
        // Run on Smaller Left side
        int64_t left_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(16);
        int64_t right_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(25);
        for (int64_t k: threads) {
            b->Args({left_count, right_count, k, 0});
        }
    }

    // Applying Thread and Size Arguments onto Radix
    void RPJArgsUniform(benchmark::internal::Benchmark *b) {
        // Run on Same Sized Input
        int64_t same_size_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(24);
        for (int64_t k: threads) {
            // Single Pass
            for (int64_t n = 9; n <= 12; ++n) {
                b->Args({same_size_count, same_size_count, k, 1, n, 0});
            }
            // Two Pass
            for (int64_t n = 5; n <= 8; ++n) {
                b->Args({same_size_count, same_size_count, k, 2, n, 0});
            }
            // Three Pass
            for (int64_t n = 5; n <= 6; ++n) {
                b->Args({same_size_count, same_size_count, k, 3, n, 0});
            }
        }
        // Run on Smaller Left side
        int64_t left_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(16);
        int64_t right_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(25);
        for (int64_t k: threads) {
            // Single Pass
            for (int64_t n = 4; n <= 8; ++n) {
                b->Args({left_count, right_count, k, 1, n, 0});
            }
            // Multi Pass
            for (int64_t n = 3; n <= 5; ++n) {
                b->Args({left_count, right_count, k, 2, n, 0});
            }
        }
    }

    // Applying Thread and Size Arguments onto NOP Join with underlying zipf distribution
    void NOPArgsZipf(benchmark::internal::Benchmark *b) {
        // Run on Same Sized Input
        int64_t same_size_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(24);
        for (int64_t k: zipf_threads) {
            for (int64_t zipf = 1; zipf <= 8; ++zipf) {
                b->Args({same_size_count, same_size_count, k, 1, zipf});
            }
        }
        // Run on Smaller Left side
        int64_t left_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(16);
        int64_t right_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(25);
        for (int64_t k: zipf_threads) {
            for (int64_t zipf = 1; zipf <= 8; ++zipf) {
                b->Args({left_count, right_count, k, 1, zipf});
            }
        }
    }

    // Applying Thread and Size Arguments onto Radix with underlying zipf distribution
    void RPJArgsZipf(benchmark::internal::Benchmark *b) {
        // Run on Same Sized Input
        int64_t same_size_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(24);
        for (int64_t k: zipf_threads) {
            for (int64_t zipf = 1; zipf <= 8; ++zipf) {
                // Single Pass
                b->Args({same_size_count, same_size_count, k, 1, 12, 1, zipf});
                // Two Pass
                b->Args({same_size_count, same_size_count, k, 2, 6, 1, zipf});
            }
        }
        // Run on Smaller Left side
        int64_t left_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(16);
        int64_t right_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(25);
        for (int64_t k: zipf_threads) {
            for (int64_t zipf = 1; zipf <= 8; ++zipf) {
                // Single Pass
                b->Args({left_count, right_count, k, 1, 8, 1, zipf});
                // Two Pass
                b->Args({left_count, right_count, k, 2, 4, 1, zipf});
            }
        }
    }

} // namespace

// Using real time since we are in a multithreaded setting
BENCHMARK(BenchmarkNOP)->Apply(NOPArgsUniform)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BenchmarkRPJ)->Apply(RPJArgsUniform)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BenchmarkNOP)->Apply(NOPArgsZipf)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BenchmarkRPJ)->Apply(RPJArgsZipf)->UseRealTime()->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();

