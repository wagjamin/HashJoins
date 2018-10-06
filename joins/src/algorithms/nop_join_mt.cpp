//
// Created by benjamin on 06.10.18.
//

#include "algorithms/nop_join_mt.h"
#include <utility>
#include <mutex>

namespace algorithms{

    /// Simple chained hash table used within the nop join
    struct nop_join_mt::hash_table{
        /// One of the hash table entries
        struct bucket{

            /// Overflow bucket used for chaining
            struct overflow{
                tuple t;
                std::unique_ptr<overflow> next;

                overflow(tuple t): t(t){}
            };

            // TODO Lightweight CMP SWP lock in the future
            std::mutex lock;
            uint32_t count;
            tuple t1;
            tuple t2;
            std::unique_ptr<overflow> next;

            /// Default constructor
            bucket(): count(0), next(nullptr) {}

        };

        std::unique_ptr<bucket[]> arr;

        explicit hash_table(uint64_t size){
            arr = std::make_unique<bucket[]>(size);
        }

    };

    nop_join_mt::nop_join_mt(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r):
            nop_join_mt(left, right, size_l, size_r, 1.5, 4){}

    nop_join_mt::nop_join_mt(tuple* left, tuple* right, uint64_t size_l,
                             uint64_t size_r, double table_size, uint8_t threads):
            nop_join_mt(left, right, size_l, size_r, table_size, threads, std::make_shared<std::vector<triple>>()){}

    nop_join_mt::nop_join_mt(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r,
                       double table_size, uint8_t threads, std::shared_ptr<std::vector<triple>> result):
            left(left), right(right), size_l(size_l), size_r(size_r),
            table_size(table_size), threads(threads), built(false), result(std::move(result)){}

} // namespace algorithms
