//
// Created by benjamin on 20.09.18.
//

#include "generators/uniform_generator.h"
#include "algorithms/nop_join.h"
#include "gtest/gtest.h"

using namespace generators;  // NOLINT
using namespace algorithms; // NOLINT

// Ensure proper creation of object and no return before execution
TEST(NopTest, CreationTester) {

    uniform_generator uni(0, 10000, 1000);
    uni.build();
    auto left = uni.get();
    uni.build();
    auto right = uni.get();
    nop_join join(left, right, 1.5);
    ASSERT_ANY_THROW(join.get());
}

// No result should be returned
TEST(NopTest, NoResTester) {
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

    nop_join join(left, right, 1.5);
    join.execute();
    ASSERT_EQ((*join.get()).size(), 0);
}

// This is a simple cross product
TEST(NopTest, CrossTester1) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get();
    uni = uniform_generator(1,1,1);
    uni.build();
    auto right = uni.get();
    nop_join join(left, right, 1.5);
    join.execute();
    ASSERT_EQ((*join.get()).size(), count);
}


// This is a more complicated cross product
TEST(NopTest, CrossTester2) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get();
    uni.build();
    auto right = uni.get();
    nop_join join(left, right, 1.5);
    join.execute();
    ASSERT_EQ((*join.get()).size(), count*count);
}


