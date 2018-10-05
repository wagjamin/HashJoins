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
            radix_join(std::move(left), std::move(right), 1.5, 6) {};

    radix_join::radix_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right,
                           double table_size,
                           uint8_t part_bits): left(std::move(left)), right(std::move(right)), table_size(table_size),
                                               result(nullptr), part_bits(part_bits),
                                               part_count(static_cast<uint32_t>(1) << part_bits) {
    };

    inline uint64_t radix_join::hash(uint64_t val) {
        return val;
    }

    void radix_join::partition(tuple* data_s, tuple* data_t,
                               uint64_t* hist, uint64_t count) {
        // Pattern filtering the bits we are looking at
        uint64_t pattern = part_count - 1;
        // Create histogram of the hash values
        for(uint64_t i = 0; i < count; ++i){
            uint64_t index = (hash(std::get<0>(data_s[pattern])) & pattern);
            hist[index]++;
        }
        // Build prefix sum
        for(uint64_t part = 1; part < part_count; ++part){
            hist[part] += hist[part - 1];
        }
        // Scatter tuples into destination
        for(uint64_t i = 0; i < count; ++i){
            uint64_t index = (hash(std::get<0>(data_s[i])) & pattern);
            uint64_t write_to = hist[index]++;
            data_t[write_to] = std::move(data_s[i]);
        }
    }

    void radix_join::execute() {
        // Partition left side
        auto list_l = std::make_shared<std::vector<tuple>>((*left).size());
        auto hist_l = std::make_unique<std::vector<uint64_t>>(part_count);
        partition((*left).data(), (*left).data(), (*hist_l).data(), (*left).size());
        // Partition right side
        auto list_r = std::make_shared<std::vector<tuple>>((*left).size());
        auto hist_r = std::make_unique<std::vector<uint64_t>>(part_count);
        partition((*left).data(), (*left).data(), (*hist_l).data(), (*left).size());
        // Join the separate partitions
        for(uint32_t part = 0; part < part_count; ++part){
            uint64_t start_l, start_r, end_l, end_r;
            if(part == 0){
                start_l = 0;
                start_r = 0;
            }
            // Start of partition is stored in histogram
            else{
                start_l = (*hist_l)[part - 1];
                start_r = (*hist_r)[part - 1];
            }
            // Read partition ends from histogram
            end_l = (*hist_l)[part];
            end_r = (*hist_r)[part];
            hash_table table(static_cast<uint64_t>((end_l - start_l) * table_size));
            
        }
    }

    std::shared_ptr<std::vector<radix_join::triple>> radix_join::get() {
        if(result == nullptr){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }

} // namespace algorithms