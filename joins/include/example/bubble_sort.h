// ---------------------------------------------------------------------------
// Seminar Template
// ---------------------------------------------------------------------------
#ifndef EXAMPLE_INCLUDE_EXAMPLE_BUBBLE_SORT_H_
#define EXAMPLE_INCLUDE_EXAMPLE_BUBBLE_SORT_H_
// ---------------------------------------------------------------------------
#include <cstdint>
#include <vector>
// ---------------------------------------------------------------------------
namespace example {
// ---------------------------------------------------------------------------
class BubbleSort {
 public:
    explicit BubbleSort(bool ascending = true);

    void operator()(std::vector<uint32_t>& vector) const;

 private:
    bool ascending_;
};
// ---------------------------------------------------------------------------
}  // namespace joins
// ---------------------------------------------------------------------------
#endif  // EXAMPLE_INCLUDE_EXAMPLE_BUBBLE_SORT_H_
// ---------------------------------------------------------------------------
