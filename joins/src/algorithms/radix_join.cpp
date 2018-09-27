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
                           double table_size): left(std::move(left)), right(std::move(right)), table_size(table_size), result(nullptr) {};

    inline uint64_t radix_join::hash(uint64_t val) {
        return val;
    }

    void radix_join::partition(tuple* data_s, tuple* data_t,
                               uint64_t* hist, uint64_t count, uint8_t start_bit) {
        uint64_t part_count = 1 << start_bit;
        // Pattern filtering the bits we are looking at
        uint64_t pattern = (1 << (start_bit+part_bits)) - 1;
        // Create histogram of the hash values
        for(uint64_t i = 0; i < count; ++i){
            uint64_t index = (hash(std::get<0>(data_s[pattern])) & pattern) >> start_bit;
            hist[index]++;
        }
        // Build prefix sum
        for(uint64_t part = 1; part < part_count; ++part){
            hist[part] += hist[part - 1];
        }
        // Scatter tuples into destination
        for(uint64_t i = 0; i < count; ++i){
            uint64_t index = (hash(std::get<0>(data_s[i])) & pattern) >> start_bit;
            uint64_t write_to = hist[index]++;
            data_t[write_to] = std::move(data_s[i]);
        }
    }

    void radix_join::execute() {
        // Create temporary maps and histograms
        std::shared_ptr<std::vector<tuple>> list_1{left};
        auto list_2 = std::make_shared<std::vector<tuple>>((*left).size());
        auto hist_1 = std::make_unique<uint64_t[]>(1 << (part_runs * part_bits));
        auto hist_2 = std::make_unique<uint64_t[]>(1 << (part_runs * part_bits));
        (hist_1).get()[0] = (*left).size();
        // TODO Finish Partition phase
        for(uint8_t run = 1; run <= part_runs; ++run){
            for(uint64_t part = 0; part < (run*part_bits); ++part){

            }
            // Shift histograms and maps
            std::swap(list_1, list_2);
            std::swap(hist_1, hist_2);
        }
        // TODO perform proper build phase
    }

    std::shared_ptr<std::vector<radix_join::triple>> radix_join::get() {
        if(result == nullptr){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }

} // namespace algorithms