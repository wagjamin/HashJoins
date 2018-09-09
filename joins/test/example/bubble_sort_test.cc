// ---------------------------------------------------------------------------
// Seminar Template
// ---------------------------------------------------------------------------
#include <numeric>
#include <random>
#include "example/bubble_sort.h"
#include "gtest/gtest.h"
// ---------------------------------------------------------------------------
using namespace example;  // NOLINT
// ---------------------------------------------------------------------------
TEST(BubbleSortTest, SortsVectorAscending) {
    std::vector<uint32_t> vector;
    vector.resize(64);

    std::iota(vector.begin(), vector.end(), 0);
    std::shuffle(vector.begin(), vector.end(), std::mt19937(42));

    BubbleSort sort(true);
    sort(vector);

    EXPECT_EQ(vector.size(), 64);

    for (uint32_t i = 0; i < vector.size(); ++i) {
        EXPECT_EQ(vector[i], i);
    }
}
// ---------------------------------------------------------------------------
TEST(BubbleSortTest, SortsVectorDescending) {
    std::vector<uint32_t> vector;
    vector.resize(64);

    std::iota(vector.begin(), vector.end(), 0);
    std::shuffle(vector.begin(), vector.end(), std::mt19937(42));

    BubbleSort sort(false);
    sort(vector);

    EXPECT_EQ(vector.size(), 64);

    for (uint32_t i = 0; i < vector.size(); ++i) {
        EXPECT_EQ(vector[i], 63 - i);
    }
}
// ---------------------------------------------------------------------------
