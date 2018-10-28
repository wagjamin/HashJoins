//
// Created by benjamin on 21.10.18.
//
#include "algorithms/mt_radix/radix_tasks.h"
#include "algorithms/nop_join.h"

namespace algorithms{

    // Hash functions used throughout the radix join
    uint64_t task_context::hash1(uint64_t val) {
        return val;
    }

    task_context::task_context(uint8_t radix_bits, uint8_t radix_passes, uint8_t thread_count, double table_size,
                               ThreadPool *pool, task_context::result_vec results):
        radix_bits(radix_bits), radix_passes(radix_passes), thread_count(thread_count), table_size(table_size),
        pool(pool), free_index(thread_count), output_mutex(), results(std::move(results))
    {
        // Properly fill the free_index vector
        for(uint8_t k = 0; k < thread_count; ++k){
            free_index[k] = k;
        }
    }

    // Main string of execution or a partition task
    std::pair<std::shared_ptr<std::vector<uint64_t>>, std::shared_ptr<std::vector<uint64_t>>>
            partition_task::execute(task_context* context, bool spawn, uint8_t curr_depth, tuple* data_l, tuple* data_r,
                                    uint64_t size_l, uint64_t size_r, tuple* target_l, tuple* target_r) {
        // Create histograms
        auto hist_l = std::make_shared<std::vector<uint64_t>>(static_cast<uint64_t>(1) << context->radix_bits);
        auto hist_r = std::make_shared<std::vector<uint64_t>>(static_cast<uint64_t>(1) << context->radix_bits);
        // Create bit pattern on which should be filtered
        uint64_t pattern = (static_cast<uint64_t>(1) << static_cast<uint64_t>(curr_depth * context->radix_bits)) - 1;
        auto shiftback = static_cast<uint64_t>((curr_depth - 1) * context->radix_bits);
        // Build histogram for the left relation
        for (uint64_t k = 0; k < size_l; ++k) {
            tuple &curr = data_l[k];
            uint64_t hash = context->hash1(std::get<0>(curr));
            // We only care about the specific partition's bits
            uint64_t value = static_cast<uint64_t>(hash & pattern) >> shiftback;
            // Increment histogram value
            ++((*hist_l)[value]);
        }
        // Build histogram for the right relation
        for (uint64_t k = 0; k < size_r; ++k) {
            tuple &curr = data_r[k];
            uint64_t hash = context->hash1(std::get<0>(curr));
            // We only care about the specific partition's bits
            uint64_t value = static_cast<uint64_t>(hash & pattern) >> shiftback;
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
    bool scatter_task::execute(task_context* context, bool spawn, uint8_t curr_depth,
                               std::shared_ptr<std::vector<uint64_t>> sum_l,
                               std::shared_ptr<std::vector<uint64_t>> sum_r, tuple* data_l, tuple* data_r,
                               uint64_t size_l, uint64_t size_r, tuple* target_l, tuple* target_r) {
        // Create bit pattern on which should be filtered
        uint64_t pattern = (static_cast<uint64_t>(1) << static_cast<uint64_t>(curr_depth * context->radix_bits)) - 1;
        auto shiftback = static_cast<uint64_t>((curr_depth - 1) * context->radix_bits);
        // Scatter left relation based on prefix sum
        for(uint64_t k = 0; k < size_l; ++k){
            tuple& curr = data_l[k];
            uint64_t hash = context->hash1(std::get<0>(curr));
            // We only care about the specific partition's bits
            uint64_t value = static_cast<uint64_t>(hash & pattern) >> shiftback;
            uint64_t write_to = (*sum_l)[value]++;
            target_l[write_to] = curr;
        }
        // Scatter right relation based on prefix sum
        for(uint64_t k = 0; k < size_r; ++k){
            tuple& curr = data_r[k];
            uint64_t hash = context->hash1(std::get<0>(curr));
            // Create bit pattern on which should be filtered
            // We only care about the specific partition's bits
            uint64_t value = static_cast<uint64_t>(hash & pattern) >> shiftback;
            uint64_t write_to = (*sum_r)[value]++;
            target_r[write_to] = curr;
        }
        // Create new partition or probe/build tasks
        if(spawn){
            // TODO
        }
        return true;
    }

    void join_task::execute(task_context* context, tuple* data_l, tuple* data_r, uint64_t size_l, uint64_t size_r){
        // No need to bother on empty partitions
        if(size_l == 0 || size_r == 0){
            return;
        }
        // First we obtain a valid output buffer
        context -> output_mutex.lock();
        uint8_t index = context->free_index.back();
        context->free_index.pop_back();
        context -> output_mutex.unlock();
        // Calculate index%threads to obtain index of a currently unused output vector
        auto output = (*context->results)[index];
        // Run a simple no partitioning join on the given data
        nop_join join(data_l, data_r, size_l, size_r, context->table_size, output);
        join.execute();
        // We put the output buffer back into the unused stack
        context -> output_mutex.lock();
        context->free_index.emplace_back(index);
        context -> output_mutex.unlock();
    }

} // namespace algorithms