//
// Benjamin Wagner 2018
//

#include "algorithms/nop_join.h"
#include <utility>

namespace algorithms{

    nop_join::nop_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r):
            nop_join(left, right, size_l, size_r, 1.5){}

    nop_join::nop_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r, double table_size):
            left(left), right(right), size_l(size_l), size_r(size_r),
            table_size(table_size), built(false), result(){}

    nop_join::nop_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r,
                       double table_size, std::vector<triple>& result):
                                left(left), right(right), size_l(size_l), size_r(size_r),
                                table_size(table_size), built(false), result(std::move(result)){}

    void nop_join::execute() {
        // No results on empty datasets
        if(size_l == 0 || size_r == 0){
            built = true;
            return;
        }
        auto new_size = static_cast<uint64_t>(1.5 * size_l);
        helpers::hash_table table = helpers::hash_table(new_size);
        // Build Phase
        for(uint64_t k = 0; k < size_l; ++k){
            tuple& curr = left[k];
            uint64_t index = helpers::murmur3(std::get<0>(curr)) % new_size;
            helpers::hash_table::bucket& bucket = table.arr[index];
            switch(bucket.count){
                case 0:
                    bucket.t1 = curr;
                    break;
                case 1:
                    bucket.t2 = curr;
                    break;
                case 2:
                    bucket.next = std::make_unique<helpers::overflow>(curr);
                    break;
                default:
                    helpers::overflow* ptr = bucket.next.get();
                    // Follow pointer indirection
                    for(uint64_t i = 0; i < static_cast<uint64_t>(bucket.count - 3); i++){
                        ptr = ptr->next.get();
                    }
                    // Create new bucket containing tuple
                    ptr->next = std::make_unique<helpers::overflow>(curr);
            }
            ++bucket.count;
        }
        // Probe Phase
        built = true;
        for(uint64_t k = 0; k < size_r; ++k){
            tuple& curr = right[k];
            uint64_t index = helpers::murmur3(std::get<0>(curr)) % new_size;
            helpers::hash_table::bucket& bucket = table.arr[index];
            // Follow overflow buckets
            if(bucket.count > 2){
                helpers::overflow* curr_over = bucket.next.get();
                for(uint64_t i = 0; i < static_cast<uint64_t>(bucket.count - 2); ++i){
                    if(std::get<0>(curr_over->t) == std::get<0>(curr)){
                        result.emplace_back(std::get<0>(curr_over->t), std::get<1>(curr_over->t), std::get<1>(curr));
                    }
                    curr_over = curr_over->next.get();
                }
            }
            // Look at second tuple
            if(bucket.count > 1 && std::get<0>(bucket.t2) == std::get<0>(curr)){
                result.emplace_back(std::get<0>(bucket.t2), std::get<1>(bucket.t2), std::get<1>(curr));
            }
            // Look at first tuple
            if(bucket.count > 0 && std::get<0>(bucket.t1) == std::get<0>(curr)){
                result.emplace_back(std::get<0>(bucket.t1), std::get<1>(bucket.t1), std::get<1>(curr));
            }
        }
    }

    std::vector<nop_join::triple>& nop_join::get() {
        if(!built){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }

    void nop_join::set(std::vector<nop_join::triple> &res_vec) {
        // The vector is moved for maximum performance. The vector cannot be used by the caller afterwards.
        result = std::move(res_vec);
        // Set built to false again, since data was not built into the new vector
        built = false;
    }

} // namespace algorithms