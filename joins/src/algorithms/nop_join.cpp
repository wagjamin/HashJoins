//
// Benjamin Wagner 2018
//

#include "algorithms/nop_join.h"
#include <utility>

namespace algorithms{

    /// Simple chained hash table used within the nop join
    struct nop_join::hash_table{
        /// One of the hash table entries
        struct bucket{

            /// Overflow bucket used for chaining
            struct overflow{
                tuple t;
                std::unique_ptr<overflow> next;

                overflow(tuple t): t(t){}
            };

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

    nop_join::nop_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r):
            nop_join(left, right, size_l, size_r, 1.5){}

    nop_join::nop_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r, double table_size):
            nop_join(left, right, size_l, size_r, table_size, std::make_shared<std::vector<triple>>()){}

    nop_join::nop_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r,
                       double table_size, std::shared_ptr<std::vector<triple>> result):
                                left(left), right(right), size_l(size_l), size_r(size_r),
                                table_size(table_size), built(false), result(std::move(result)){}

    void nop_join::execute() {
        // No results on empty datasets
        if(size_l == 0 || size_r == 0){
            built = true;
            return;
        }
        auto new_size = static_cast<uint64_t>(1.5 * size_l);
        hash_table table = hash_table(new_size);
        // Build Phase
        for(uint64_t k = 0; k < size_l; ++k){
            tuple& curr = left[k];
            uint64_t index = hash(std::get<0>(curr)) % new_size;
            hash_table::bucket& bucket = table.arr[index];
            switch(bucket.count){
                case 0:
                    bucket.t1 = curr;
                    break;
                case 1:
                    bucket.t2 = curr;
                    break;
                case 2:
                    bucket.next = std::make_unique<hash_table::bucket::overflow>(curr);
                    break;
                default:
                    hash_table::bucket::overflow* ptr = bucket.next.get();
                    // Follow pointer indirection
                    for(uint64_t i = 0; i < static_cast<uint64_t>(bucket.count - 3); i++){
                        ptr = ptr->next.get();
                    }
                    // Create new bucket containing tuple
                    ptr->next = std::make_unique<hash_table::bucket::overflow>(curr);
            }
            ++bucket.count;
        }
        // Probe Phase
        built = true;
        for(uint64_t k = 0; k < size_r; ++k){
            tuple& curr = right[k];
            uint64_t index = hash(std::get<0>(curr)) % new_size;
            hash_table::bucket& bucket = table.arr[index];
            // Follow overflow buckets
            if(bucket.count > 2){
                hash_table::bucket::overflow* curr_over = bucket.next.get();
                for(uint64_t i = 0; i < static_cast<uint64_t>(bucket.count - 2); ++i){
                    if(std::get<0>(curr_over->t) == std::get<0>(curr)){
                        result->push_back({std::get<0>(curr_over->t), std::get<1>(curr_over->t), std::get<1>(curr)});
                    }
                    curr_over = curr_over->next.get();
                }
            }
            // Look at second tuple
            if(bucket.count > 1 && std::get<0>(bucket.t2) == std::get<0>(curr)){
                result->push_back({std::get<0>(bucket.t2), std::get<1>(bucket.t2), std::get<1>(curr)});
            }
            // Look at first tuple
            if(bucket.count > 0 && std::get<0>(bucket.t1) == std::get<0>(curr)){
                result->push_back({std::get<0>(bucket.t1), std::get<1>(bucket.t1), std::get<1>(curr)});
            }
        }
    }

    void nop_join::set_res(std::shared_ptr<std::vector<algorithms::nop_join::triple>> res) {
        result = std::move(res);
    }

    std::shared_ptr<std::vector<nop_join::triple>> nop_join::get() {
        if(!built){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }

    uint64_t nop_join::hash(uint64_t val) {
        // Murmur 3 taken from "A Seven-Dimensional Analysis of Hashing Methods and its
        // Implications on Query Processing" by Richter et al
        val ^= val >> 33;
        val *= 0xff51afd7ed558ccd;
        val ^= val >> 33;
        val *= 0xc4ceb9fe1a85ec53;
        val ^= val >> 33;
        return val;
    }

} // namespace algorithms