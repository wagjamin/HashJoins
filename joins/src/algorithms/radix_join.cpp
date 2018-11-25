//
// Benjamin Wagner 2018
//

#include "algorithms/radix_join.h"
#include "algorithms/nop_join.h"
#include <utility>

namespace algorithms{


    radix_join::radix_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r):
            radix_join(left, right, size_l, size_r, 1.5, 6) {};

    radix_join::radix_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r, double table_size,
                           uint8_t part_bits): left(left), right(right), size_l(size_l), size_r(size_r),
                                               table_size(table_size), part_bits(part_bits),
                                               part_count(static_cast<uint32_t>(1) << part_bits), built(false) {};


    void radix_join::partition(tuple* data_s, tuple* data_t, uint64_t* hist, uint64_t count) {
        // Pattern filtering the bits we are looking at
        uint64_t pattern = part_count - 1;
        // Create histogram of the hash values
        for(uint64_t i = 0; i < count; ++i){
            uint64_t index = (helpers::murmur3(std::get<0>(data_s[i])) & pattern);
            hist[index]++;
        }
        // Build prefix sum
        uint64_t sum = hist[0];
        hist[0] = 0;
        for(uint64_t part = 1; part < part_count; ++part){
            uint64_t temp = hist[part];
            hist[part] = sum;
            sum += temp;
        }
        // Scatter tuples into destination
        for(uint64_t i = 0; i < count; ++i){
            uint64_t index = (helpers::murmur3(std::get<0>(data_s[i])) & pattern);
            uint64_t write_to = hist[index]++;
            data_t[write_to] = data_s[i];
        }
    }

    void radix_join::execute() {
        built = true;
        // No results on empty datasets
        if(size_l == 0 || size_r == 0){
            return;
        }
        // Partition left side
        std::unique_ptr<tuple[]> list_l(new tuple[size_l]);
        auto hist_l = std::make_unique<uint64_t[]>(part_count);
        partition(left, list_l.get(), hist_l.get(), size_l);
        // Partition right side
        std::unique_ptr<tuple[]> list_r(new tuple[size_r]);
        auto hist_r = std::make_unique<uint64_t[]>(part_count);
        partition(right, list_r.get(), hist_r.get(), size_r);
        // Join the separate partitions
        for(uint32_t part = 0; part < part_count; ++part){
            uint64_t start_l, start_r, end_l, end_r;
            if(part == 0){
                start_l = 0;
                start_r = 0;
            }
            // Start of partition is stored in histogram
            else{
                start_l = hist_l[part - 1];
                start_r = hist_r[part - 1];
            }
            // Read partition ends from histogram
            end_l = hist_l[part];
            end_r = hist_r[part];
            // Perform standard NOP-Join on partitions
            if(start_l != end_l && start_r != end_r){
                // Pass result vector to the No partitioning join
                nop_join join(list_l.get() + start_l, list_r.get() + start_r, end_l - start_l,
                              end_r - start_r, table_size, result);
                join.execute();
                // Reclaim result vector
                result = std::move(join.get());
            }
        }
    }

    std::vector<radix_join::triple>& radix_join::get() {
        if(!built){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }

    void radix_join::set(std::vector<radix_join::triple> &res_vec) {
        // The vector is moved for maximum performance. The vector cannot be used by the caller afterwards.
        result = std::move(res_vec);
        // Set built to false again, since data was not built into the new vector
        built = false;
    }

} // namespace algorithms