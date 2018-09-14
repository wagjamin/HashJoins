//
// Created by benjamin on 14.09.18.
//

#include "algorithms/nop_join.h"
#include <utility>
#include <mutex>

namespace algorithms{

    struct nop_join::hash_table{
        /// One of the hash table entries
        struct bucket{

            /// Overflow bucket used for chaining
            struct overflow{
                tuple t;
                overflow* next;
            };

            std::mutex lock;
            tuple t1;
            tuple t2;
            overflow* next;

            bucket(): next(nullptr) {}

        };

        std::unique_ptr<bucket[]> arr;

        explicit hash_table(uint64_t size){
            arr = std::make_unique<bucket[]>(size);
        }

    };

    nop_join::nop_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right):
       nop_join(std::move(left), std::move(right), 4, 1.5){}

    nop_join::nop_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right,
                       uint64_t threads, double table_size):
            left(left), right(right), threads(threads), table_size(table_size) {
        // Internally, the left table should always be the one with which we build, i.e. the smaller one
        if((*left).size() >= (*right).size()){
            auto temp = right;
            right = left;
            left = right;
        }
    }

    void nop_join::build() {

        hash_table table = hash_table(static_cast<uint64_t>(1.5 * (*left).size()));
        // Build Phase
        for(tuple& curr: *left){
        }
        // Probe Phase
        for(tuple& curr: *right){
        }
    }

    std::shared_ptr<std::vector<uint64_t[3]>> nop_join::get() {
        if(result == nullptr){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }

} // namespace algorithms