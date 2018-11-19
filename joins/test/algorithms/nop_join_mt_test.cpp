//
// Created by benjamin on 16.10.18.
//

#include "generators/uniform_generator.h"
#include "algorithms/nop_join_mt.h"
#include "gtest/gtest.h"
#include <iostream>
#include <chrono>

// Number of threads the program should be run on
#define thread_count 4

using namespace generators;  // NOLINT
using namespace algorithms; // NOLINT

/*
 * Helper function. Takes the vector of output vectors and calculates the total length
 * of the output.
 */
uint64_t get_size(std::vector<std::vector<nop_join_mt::triple>>* output){
    uint64_t size = 0;
    for(auto& vec: (*output)){
        size += vec.size();
    }
    return size;
}

// Ensure proper creation of object and no return before execution
TEST(NopTestMT, CreationTesterST) {
    uniform_generator uni(0, 10000, 1000);
    uni.build();
    auto left = uni.get_vec_copy();
    uni.build();
    auto right = uni.get_vec_copy();
    nop_join_mt join(left.data(), right.data(), 1000, 1000, 1.5, 1);
    ASSERT_ANY_THROW(join.get());
}

// No result should be returned
TEST(NopTestMT, NoResTesterST) {
    uint64_t min = 0;
    uint64_t max = 10000;
    uint64_t count = 1000;
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    min = 20000;
    max = 30000;
    gen = uniform_generator(min, max, count);
    gen.build();
    auto right = gen.get_vec_copy();
    nop_join_mt join(left.data(), right.data(), count, count, 1.5, 1);
    join.execute();
    ASSERT_EQ(get_size(join.get().get()), 0);
}

// This is a simple cross product
TEST(NopTestMT, CrossTester1ST) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni = uniform_generator(1,1,1);
    uni.build();
    auto right = uni.get_vec_copy();
    nop_join_mt join(left.data(), right.data(), count, 1, 1.5, 1);
    join.execute();
    ASSERT_EQ(get_size(join.get().get()), count);
}


// This is a more complicated cross product
TEST(NopTestMT, CrossTester2ST) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni.build();
    auto right = uni.get_vec_copy();
    nop_join_mt join(left.data(), right.data(), count, count, 1.5, 1);
    join.execute();
    ASSERT_EQ(get_size(join.get().get()), count*count);
}

// Statistical test, usually should not fail
TEST(NopTestMT, StatisticalTesterST){
    uint64_t count = static_cast<uint64_t>(1) << static_cast<uint64_t>(17);
    uint64_t min = 1;
    uint64_t max = static_cast<uint64_t>(1) << static_cast<uint64_t>(12);
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    gen.build();
    auto right = gen.get_vec_copy();
    nop_join_mt join(left.data(), right.data(), count, count, 1.5, 1);
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    join.execute();
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "MT NOP Join Time Single Threaded: " << duration << " milliseconds.\n";
    // Expected overall amount of join partners
    auto expected = static_cast<uint64_t>(max * (static_cast<double>(count)/max) * static_cast<double>((count))/max);
    ASSERT_LE(0.95 * expected, get_size(join.get().get()));
    ASSERT_GE(1.05 * expected, get_size(join.get().get()));
}

// Ensure proper creation of object and no return before execution
TEST(NopTestMT, CreationTesterMT) {
    uniform_generator uni(0, 10000, 1000);
    uni.build();
    auto left = uni.get_vec_copy();
    uni.build();
    auto right = uni.get_vec_copy();
    nop_join_mt join(left.data(), right.data(), 1000, 1000, 1.5, thread_count);
    ASSERT_ANY_THROW(join.get());
}

// No result should be returned
TEST(NopTestMT, NoResTesterMT) {
    uint64_t min = 0;
    uint64_t max = 10000;
    uint64_t count = 1000;
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    min = 20000;
    max = 30000;
    gen = uniform_generator(min, max, count);
    gen.build();
    auto right = gen.get_vec_copy();
    nop_join_mt join(left.data(), right.data(), count, count, 1.5, thread_count);
    join.execute();
    ASSERT_EQ(get_size(join.get().get()), 0);
}

// This is a simple cross product
TEST(NopTestMT, CrossTester1MT) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni = uniform_generator(1,1,1);
    uni.build();
    auto right = uni.get_vec_copy();
    nop_join_mt join(left.data(), right.data(), count, 1, 1.5, thread_count);
    join.execute();
    ASSERT_EQ(get_size(join.get().get()), count);
}


// This is a more complicated cross product
TEST(NopTestMT, CrossTester2MT) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni.build();
    auto right = uni.get_vec_copy();
    nop_join_mt join(left.data(), right.data(), count, count, 1.5, thread_count);
    join.execute();
    ASSERT_EQ(get_size(join.get().get()), count*count);
}

// Statistical test, usually should not fail
TEST(NopTestMT, StatisticalTesterMT){
    uint64_t count = static_cast<uint64_t>(1) << static_cast<uint64_t>(17);
    uint64_t min = 1;
    uint64_t max = static_cast<uint64_t>(1) << static_cast<uint64_t>(12);
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    gen.build();
    auto right = gen.get_vec_copy();
    nop_join_mt join(left.data(), right.data(), count, count, 1.5, thread_count);
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    join.execute();
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "MT NOP Join Time Multi Threaded: " << duration << " milliseconds.\n";
    // Expected overall amount of join partners
    auto expected = static_cast<uint64_t>(max * (static_cast<double>(count)/max) * static_cast<double>((count))/max);
    ASSERT_LE(0.95 * expected, get_size(join.get().get()));
    ASSERT_GE(1.05 * expected, get_size(join.get().get()));
}