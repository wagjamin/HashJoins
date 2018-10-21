//
// Created by benjamin on 21.10.18.
//

#include "algorithms/radix_join_mt.h"
#include <utility>
#include <mutex>
#include <thread>

namespace algorithms{

    radix_join_mt::radix_join_mt(algorithms::radix_join_mt::tuple *left, algorithms::radix_join_mt::tuple *right,
                                 uint64_t size_l, uint64_t size_r, uint8_t bits_per_pass, uint8_t passes):
                radix_join_mt(left, right, size_l, size_r, 1.5, 4, bits_per_pass, passes)
    {}

    radix_join_mt::radix_join_mt(algorithms::radix_join_mt::tuple *left, algorithms::radix_join_mt::tuple *right,
                                 uint64_t size_l, uint64_t size_r, double table_size, uint8_t threads,
                                 uint8_t bits_per_pass, uint8_t passes):
                radix_join_mt(left, right, size_l, size_r, table_size, threads,
                              std::make_shared<std::vector<std::vector<triple>>>(threads), bits_per_pass, passes)
    {}

    radix_join_mt::radix_join_mt(algorithms::radix_join_mt::tuple *left, algorithms::radix_join_mt::tuple *right,
                                 uint64_t size_l, uint64_t size_r, double table_size, uint8_t threads,
                                 std::shared_ptr<std::vector<std::vector<algorithms::radix_join_mt::triple>>> result,
                                 uint8_t bits_per_pass, uint8_t passes):
            left(left), right(right), size_l(size_l), size_r(size_r), table_size(table_size), threads(threads),
            result(std::move(result)), bits_per_pass(bits_per_pass), passes(passes)
    {}

    std::shared_ptr<std::vector<std::vector<radix_join_mt::triple>>> radix_join_mt::get() {
        if(!built){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }
} // namespace algorithms