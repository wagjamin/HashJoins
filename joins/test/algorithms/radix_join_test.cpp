//
// Benjamin Wagner 2018
//

#include "generators/uniform_generator.h"
#include "algorithms/radix_join.h"
#include "gtest/gtest.h"
#include <iostream>
#include <chrono>

using namespace generators;  // NOLINT
using namespace algorithms; // NOLINT


// Ensure proper creation of object and no return before execution
TEST(RadTest, CreationTester) {

    uniform_generator uni(0, 10000, 1000);
    uni.build();
    auto left = uni.get_vec_copy();
    uni.build();
    auto right = uni.get_vec_copy();
    radix_join join(left.data(), right.data(), 1000, 1000, 1.5, 6);
    ASSERT_ANY_THROW(join.get());
}

// No result should be returned
TEST(RadTest, NoResTester) {
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
    radix_join join(left.data(), right.data(), count, count, 1.5, 6);
    join.execute();
    ASSERT_EQ((*join.get()).size(), 0);
}

// This is a simple cross product
TEST(RadTest, CrossTester1) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni = uniform_generator(1,1,1);
    uni.build();
    auto right = uni.get_vec_copy();
    radix_join join(left.data(), right.data(), count, 1, 1.5, 6);
    join.execute();
    ASSERT_EQ((*join.get()).size(), count);
}


// This is a more complicated cross product
TEST(RadTest, CrossTester2) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni.build();
    auto right = uni.get_vec_copy();
    radix_join join(left.data(), right.data(), count, count, 1.5, 6);
    join.execute();
    ASSERT_EQ((*join.get()).size(), count*count);
}

// Statistical test, usually should not fail
TEST(RadTest, StatisticalTester){
    uint64_t count = static_cast<uint64_t>(1) << static_cast<uint64_t>(17);
    uint64_t min = 1;
    uint64_t max = static_cast<uint64_t>(1) << static_cast<uint64_t>(12);
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    gen.build();
    auto right = gen.get_vec_copy();
    radix_join join(left.data(), right.data(), count, count, 1.5, 7);
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    join.execute();
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "ST Radix Join Time: " << duration << " milliseconds.\n";
    // Expected overall amount of join partners
    auto expected = static_cast<uint64_t>(max * (static_cast<double>(count)/max) * static_cast<double>((count))/max);
    ASSERT_LE(0.95 * expected, (*join.get()).size());
    ASSERT_GE(1.05 * expected, (*join.get()).size());
}
