//
// Created by benjamin on 13.09.18.
//

#include "generators/uniform_generator.h"
#include "gtest/gtest.h"
#include <unordered_set>

using namespace generators;  // NOLINT

// Assert that the generated values are within range
TEST(UniGenTest, RangeTester) {

    // Run the generator with several different ranges
    for(uint64_t i = 0; i < 3; ++i){
        uint64_t min = 0 + i;
        uint64_t max = static_cast<uint64_t>(1) << (i + 12);
        uint64_t count = static_cast<uint64_t>(1) << (14 + i);
        uniform_generator gen(min, max, count);
        gen.build();
        auto vec = gen.get_vec_copy();
        for(uint64_t k = 0; k < count; k++){
            ASSERT_GE(std::get<0>(vec[k]), min);
            ASSERT_LE(std::get<0>(vec[k]), max);
        }
    }
}

// Assert that the distribution is reasonably uniform
TEST(UniGenTest, DistTester) {
    uint64_t min = 1;
    auto max = static_cast<uint64_t>(1) << static_cast<uint64_t>(10);
    auto count = static_cast<uint64_t>(1) << static_cast<uint64_t>(25);
    uniform_generator gen(min, max, count);
    gen.build();
    // Create value histogram
    auto freq = std::make_unique<std::vector<uint64_t>>(max);
    auto vec = gen.get_vec_copy();
    for(uint64_t k = 0; k < gen.get_count(); ++k){
        (*freq)[std::get<0>((vec)[k]) - 1]++;
    }
    // Ensure that histogram values fluctuate around mean
    auto expected = static_cast<uint64_t>(count/max);
    for(auto& item: *freq){
        ASSERT_LE(static_cast<uint64_t>(0.95*expected), item);
        ASSERT_GE(static_cast<uint64_t>(1.05*expected), item);
    }
}

// Assert that the given record ids are unique
TEST(UniGenTest, RIDTester){
    uniform_generator gen(0, 10, static_cast<uint64_t>(1) << static_cast<uint64_t>(22));
    gen.build();
    std::unordered_set<uint64_t> my_set = std::unordered_set<uint64_t>();
    auto vec = gen.get_vec_copy();
    for(uint64_t k = 0; k < gen.get_count(); ++k){
        ASSERT_EQ(my_set.find(std::get<1>((vec)[k])), my_set.end());
        my_set.insert(std::get<1>((vec)[k]));
    }
}


