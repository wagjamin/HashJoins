//
// Created by benjamin on 26.09.18.
//

#include "algorithms/radix_join.h"
#include <utility>

namespace algorithms{

    /// Simple linear probing hash table
    struct radix_join::hash_table{

        uint64_t size;
        std::unique_ptr<tuple[]> arr;

        explicit hash_table(uint64_t size): size(size){
            arr = std::make_unique<tuple[]>(size);
        };

    };

    radix_join::radix_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right):
            radix_join(std::move(left), std::move(right), 1.5) {};

    radix_join::radix_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right,
                           double table_size): left(left), right(right), table_size(table_size){};

    inline uint64_t radix_join::hash(uint64_t val) {
        return val;
    }

    void radix_join::partition(tuple* data_s, tuple* data_t,
                               uint64_t* hist, uint64_t count, uint8_t start_bit) {
        uint64_t part_count = 1 << start_bit;
        // Pattern filtering the bits we are looking at
        uint64_t pattern = (1 << (start_bit+part_bits)) - 1;
        // Create histogram of the hash values
        for(tuple& curr: data_s){
            uint64_t index = (hash(std::get<0>(curr)) & pattern) >> start_bit;
            hist[index]++;
        }
        // Build prefix sum
        for(uint64_t part = 1; part < part_count; ++part){
            hist[part] += hist[part - 1];
        }
        // Scatter tuples into destination
        for(tuple& curr: data_s){
            uint64_t index = (hash(std::get<0>(curr)) & pattern) >> start_bit;
            uint64_t write_to = hist[index]++;
            data_t[write_to] = std::move(curr);
        }
    }

    void radix_join::execute() {

    }

    std::shared_ptr<std::vector<triple>> radix_join::get() {
        if(result == nullptr){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }

} // namespace algorithms