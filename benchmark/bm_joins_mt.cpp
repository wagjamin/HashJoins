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

namespace {

    void BenchmarkNOPSameSize(benchmark::State &state) {
        // Using Dataset Size of about 16.8 Million
        uint64_t dataset_size = static_cast<uint64_t>(1) << static_cast<uint64_t>(24);
        auto threads = static_cast<uint8_t>(state.range(0));

        std::unique_ptr<std::vector<std::tuple<uint64_t, uint64_t>>> build;
        std::unique_ptr<std::vector<std::tuple<uint64_t, uint64_t>>> probe;
        // After this block we can forget the generators again
        {
            generators::uniform_generator gen0(0, dataset_size - 1, dataset_size);
            generators::uniform_generator gen1(0, dataset_size - 1, dataset_size);
            gen0.build();
            gen1.build();
            build = gen0.get_vec_copy();
            probe = gen1.get_vec_copy();
        }

        for (auto _ : state) {
            state.PauseTiming();
            // Create copies of the generated arrays for the current task
            std::vector<std::tuple<uint64_t, uint64_t>> build_temp = *build;
            std::vector<std::tuple<uint64_t, uint64_t>> probe_temp = *probe;
            // Create NOP Join
            algorithms::nop_join_mt join(build_temp.data(), probe_temp.data(), dataset_size, dataset_size, 1.5,
                                         threads);
            state.ResumeTiming();
            // Execute the actual join
            join.execute();
        }

        state.SetItemsProcessed(dataset_size * state.iterations() * 2);
    }

    void BenchmarkRPJSPSameSize(benchmark::State &state) {
        // Using Dataset Size of about 16.8 Million
        uint64_t dataset_size = (static_cast<uint64_t>(1) << static_cast<uint64_t>(24));
        auto threads = static_cast<uint8_t>(state.range(0));

        std::unique_ptr<std::vector<std::tuple<uint64_t, uint64_t>>> build;
        std::unique_ptr<std::vector<std::tuple<uint64_t, uint64_t>>> probe;
        // After this block we can forget the generators again
        {
            generators::uniform_generator gen0(0, dataset_size - 1, dataset_size);
            generators::uniform_generator gen1(0, dataset_size - 1, dataset_size);
            gen0.build();
            gen1.build();
            build = gen0.get_vec_copy();
            probe = gen1.get_vec_copy();
        }

        for (auto _ : state) {
            state.PauseTiming();
            // Create copies of the generated arrays for the current task
            std::vector<std::tuple<uint64_t, uint64_t>> build_temp = *build;
            std::vector<std::tuple<uint64_t, uint64_t>> probe_temp = *probe;
            // Create NOP Join
            algorithms::radix_join_mt join(build_temp.data(), probe_temp.data(), dataset_size, dataset_size, 1.5,
                                           threads, static_cast<uint8_t>(state.range(1)), 1);
            state.ResumeTiming();
            // Execute the actual join
            join.execute();
        }

        state.SetItemsProcessed(dataset_size * state.iterations() * 2);
    }

}

// Using real time since we are in a multithreaded setting
BENCHMARK(BenchmarkNOPSameSize)->Arg(1)->Arg(2)->Arg(3)->Arg(4)->UseRealTime()->Unit(benchmark::kMillisecond);
BENCHMARK(BenchmarkRPJSPSameSize)
        ->Args({1, 8})->Args({1, 9})->Args({1, 10})->Args({1, 11})
        ->Args({2, 8})->Args({2, 9})->Args({2, 10})->Args({2, 11})
        ->Args({3, 8})->Args({3, 9})->Args({3, 10})->Args({3, 11})
        ->Args({4, 8})->Args({4, 9})->Args({4, 10})->Args({4, 11})
        ->UseRealTime()->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();

