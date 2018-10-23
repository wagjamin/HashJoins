//
// Created by benjamin on 21.10.18.
//

#include <algorithms/nop_join_mt.h>
#include "algorithms/mt_radix/radix_tasks.h"
#include "algorithms/nop_join.h"
#include "ThreadPool.h"

namespace algorithms{

    // Hash functions used throughout the radix join
    uint64_t task_context::hash1(uint64_t val) {
        return val;
    }
    uint64_t task_context::hash2(uint64_t val) {
        return val;
    }

    task_context::task_context(uint8_t radix_bits, uint8_t radix_passes, double table_size, ThreadPool &pool):
        radix_bits(radix_bits), radix_passes(radix_passes), table_size(table_size), pool(pool){}

    // Main string of execution or a partition task
    std::pair<std::shared_ptr<std::vector<uint64_t>>, std::shared_ptr<std::vector<uint64_t>>>
            partition_task::execute(task_context& context, bool spawn, uint8_t curr_depth, tuple* data_l, tuple* data_r,
                                    uint64_t size_l, uint64_t size_r, tuple* target_l, tuple* target_r) {
        // Create histograms
        auto hist_l = std::make_shared<std::vector<uint64_t>>(static_cast<uint64_t>(1) << context.radix_bits);
        auto hist_r = std::make_shared<std::vector<uint64_t>>(static_cast<uint64_t>(1) << context.radix_bits);
        // Build histogram for the left relation
        for (uint64_t k = 0; k < size_l; ++k) {
            tuple &curr = data_l[k];
            uint64_t hash = context.hash1(std::get<0>(curr));
            // Create bit pattern on which should be filtered
            uint64_t pattern = static_cast<uint64_t>(1) <<
                                                        static_cast<uint64_t>(curr_depth * context.radix_bits);
            // We only care about the specific partition's bits
            uint64_t value = static_cast<uint64_t>(hash & pattern) >>
                                                                   static_cast<uint64_t>((curr_depth - 1) *
                                                                                         context.radix_bits);
            // Increment histogram value
            ++((*hist_l)[value]);
        }
        // Build histogram for the right relation
        for (uint64_t k = 0; k < size_r; ++k) {
            tuple &curr = data_r[k];
            uint64_t hash = context.hash1(std::get<0>(curr));
            // Create bit pattern on which should be filtered
            uint64_t pattern = static_cast<uint64_t>(1) <<
                                                        static_cast<uint64_t>(curr_depth * context.radix_bits);
            // We only care about the specific partition's bits
            uint64_t value = static_cast<uint64_t>(hash & pattern) >>
                                                                   static_cast<uint64_t>((curr_depth - 1) *
                                                                                         context.radix_bits);
            // Increment histogram value
            ++((*hist_r)[value]);
        }
        // Create scatter tasks if spawning is enabled
        if (spawn) {
            // TODO
        }
        // Return histograms
        return {hist_l, hist_r};
    }

    // Perform actual scattering
    void scatter_task::execute(task_context& context, bool spawn, uint8_t curr_depth,
                               std::shared_ptr<std::vector<uint64_t>> sum_l,
                               std::shared_ptr<std::vector<uint64_t>> sum_r, tuple* data_l, tuple* data_r,
                               uint64_t size_l, uint64_t size_r, tuple* target_l, tuple* target_r) {
        // Scatter left relation based on prefix sum
        for(uint64_t k = 0; k < size_l; ++k){
            tuple& curr = data_l[k];
            uint64_t hash = context.hash1(std::get<0>(curr));
            // Create bit pattern on which should be filtered
            uint64_t pattern = static_cast<uint64_t>(1) <<
                    static_cast<uint64_t>(curr_depth * context.radix_bits);
            // We only care about the specific partition's bits
            uint64_t value = static_cast<uint64_t>(hash & pattern) >>
                    static_cast<uint64_t>((curr_depth - 1) * context.radix_bits);
            uint64_t write_to = (*sum_l)[value]++;
            target_l[write_to] = curr;
        }
        // Scatter right relation based on prefix sum
        for(uint64_t k = 0; k < size_r; ++k){
            tuple& curr = data_r[k];
            uint64_t hash = context.hash1(std::get<0>(curr));
            // Create bit pattern on which should be filtered
            uint64_t pattern = static_cast<uint64_t>(1) <<
                                                        static_cast<uint64_t>(curr_depth * context.radix_bits);
            // We only care about the specific partition's bits
            uint64_t value = static_cast<uint64_t>(hash & pattern) >>
                                                                   static_cast<uint64_t>((curr_depth - 1) * context.radix_bits);
            uint64_t write_to = (*sum_r)[value]++;
            target_r[write_to] = curr;
        }
        // Create new partition or probe/build tasks
        if(spawn){
            // TODO
        }
    }

    void join_task::execute(task_context& context, tuple* data_l, tuple* data_r, uint64_t size_l, uint64_t size_r,
                            std::shared_ptr<std::vector<triple>> output){
        // Run a simple no partitioning join on the given data
        nop_join join(data_l, data_r, size_l, size_r, context.table_size, std::move(output));
        join.execute();
    }

} // namespace algorithms