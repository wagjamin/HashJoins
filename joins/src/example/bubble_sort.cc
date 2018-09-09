// ---------------------------------------------------------------------------
// Seminar Template
// ---------------------------------------------------------------------------
#include <algorithm>
#include "example/bubble_sort.h"
// ---------------------------------------------------------------------------
namespace example {
// ---------------------------------------------------------------------------
BubbleSort::BubbleSort(bool ascending)
    : ascending_(ascending) {
}
// ---------------------------------------------------------------------------
void BubbleSort::operator()(std::vector<uint32_t>& vector) const {
    uint64_t limit = vector.size();
    bool swapped = true;

    while ((limit != 0) && swapped) {
        swapped = false;

        for (uint32_t i = 1; i < limit; ++i) {
            if (ascending_ ^ (vector[i - 1] < vector[i])) {
                std::swap(vector[i - 1], vector[i]);

                swapped = true;
            }
        }

        --limit;
    }
}
// ---------------------------------------------------------------------------
}  // namespace joins
// ---------------------------------------------------------------------------
