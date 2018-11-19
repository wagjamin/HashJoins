//
// Created by benjamin on 01.11.18.
//

#include "generators/incremental_generator.h"
#include "gtest/gtest.h"
#include <unordered_set>

using namespace generators;  // NOLINT

// Assert that the generated values are within range
TEST(IncGenTest, RangeTester) {
    uint64_t max = 100000;
    incremental_generator gen(0, max);
    gen.build();
    auto res = gen.get_vec_copy();
    ASSERT_EQ(gen.get_count(), max + 1);

    for(uint64_t k = 0; k <= max; ++k){
        ASSERT_EQ(std::get<0>(res[k]), k);
        ASSERT_EQ(std::get<1>(res[k]), k);
    }
}