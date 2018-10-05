//
// Created by benjamin on 05.10.18.
//

#include "generators/uniform_generator.h"
#include "algorithms/radix_join.h"
#include "gtest/gtest.h"

using namespace generators;  // NOLINT
using namespace algorithms; // NOLINT


// Ensure proper creation of object and no return before execution
TEST(RadTest, CreationTester) {

    uniform_generator uni(0, 10000, 1000);
    uni.build();
    auto left = uni.get();
    uni.build();
    auto right = uni.get();
    radix_join join(left.get(), right.get(), 1000, 1000, 1.5, 6);
    ASSERT_ANY_THROW(join.get());
}

// No result should be returned
TEST(RadTest, NoResTester) {
    uint64_t min = 0;
    uint64_t max = 10000;
    uint64_t count = 1000;
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get();
    min = 20000;
    max = 30000;
    gen = uniform_generator(min, max, count);
    gen.build();
    auto right = gen.get();
    radix_join join(left.get(), right.get(), count, count, 1.5, 6);
    join.execute();
    ASSERT_EQ((*join.get()).size(), 0);
}

// This is a simple cross product
TEST(RadTest, CrossTester1) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get();
    uni = uniform_generator(1,1,1);
    uni.build();
    auto right = uni.get();
    radix_join join(left.get(), right.get(), count, 1, 1.5, 6);
    join.execute();
    ASSERT_EQ((*join.get()).size(), count);
}


// This is a more complicated cross product
TEST(RadTest, CrossTester2) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get();
    uni.build();
    auto right = uni.get();
    radix_join join(left.get(), right.get(), count, count, 1.5, 6);
    join.execute();
    ASSERT_EQ((*join.get()).size(), count*count);
}

// Statistical test, usually should not fail
TEST(RadTest, StatisticalTester){
    uint64_t count = static_cast<uint64_t>(1) << static_cast<uint64_t>(16);
    uint64_t min = 1;
    uint64_t max = static_cast<uint64_t>(1) << static_cast<uint64_t>(10);
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get();
    gen.build();
    auto right = gen.get();
    radix_join join(left.get(), right.get(), count, count, 1.5, 6);
    join.execute();
    // Expected overall amount of join partners
    uint64_t expected = max * (count/max) * (count/max);
    ASSERT_LE(0.95 * expected, (*join.get()).size());
    ASSERT_GE(1.05 * expected, (*join.get()).size());
}
