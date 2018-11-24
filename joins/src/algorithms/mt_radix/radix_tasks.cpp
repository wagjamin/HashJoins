//
// Benjamin Wagner 2018
//

#include "algorithms/mt_radix/radix_tasks.h"
#include "algorithms/nop_join.h"

namespace algorithms{

    // Hash functions used throughout the radix join
    uint64_t task_context::hash1(uint64_t val) {
        // Murmur 3 taken from "A Seven-Dimensional Analysis of Hashing Methods and its
        // Implications on Query Processing" by Richter et al
        val ^= val >> 33;
        val *= 0xff51afd7ed558ccd;
        val ^= val >> 33;
        val *= 0xc4ceb9fe1a85ec53;
        val ^= val >> 33;
        return val;
    }

    task_context::task_context(uint8_t radix_bits, uint8_t radix_passes, uint8_t thread_count, double table_size,
                               ThreadPool *pool, task_context::result_vec results):
        radix_bits(radix_bits), radix_passes(radix_passes), thread_count(thread_count), table_size(table_size),
        pool(pool), free_index(thread_count), output_mutex(), results(std::move(results)), finished(false),
        join_count(0), join_exp(static_cast<uint64_t>(1) << static_cast<uint64_t>(radix_bits*radix_passes))
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
            // Create prefix sums used by the scatter task
            uint64_t part_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(context->radix_bits);
            uint64_t cl = (*hist_l)[0];
            uint64_t cr = (*hist_r)[0];
            (*hist_l)[0] = 0;
            (*hist_r)[0] = 0;
            for(uint64_t k = 1; k < part_count; ++k){
                uint64_t temp_l = (*hist_l)[k];
                uint64_t temp_r = (*hist_r)[k];
                (*hist_l)[k] = cl;
                (*hist_r)[k] = cr;
                cl += temp_l;
                cr += temp_r;
            }
            // Schedule scatter task based on prefix sum
            context->pool->enqueue(scatter_task::execute, context, true, curr_depth, hist_l, hist_r, data_l, data_r,
                    size_l, size_r, target_l, target_r);
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
            // Number of partitions created by this scatter task
            uint64_t part_count = static_cast<uint64_t>(1) << static_cast<uint64_t>(context->radix_bits);
            // We need to run more partition passes
            if(curr_depth < context->radix_passes){
                /*
                 * Schedule the next round of partition tasks. This can be done by calculating the respective
                 * sub-arrays from the prefix sums passed to the scatter task.
                 * Since we updated the sums in the previous part now every index in the prefix sum actually refers
                 * to the first index of the next partition
                 */
                for(uint64_t k = 0; k < part_count - 1; ++k){
                    context->pool->enqueue(partition_task::execute, context, true, curr_depth + 1,
                           target_l + (*sum_l)[k], target_r + (*sum_r)[k], (*sum_l)[k + 1] - (*sum_l)[k],
                           (*sum_r)[k + 1] - (*sum_r)[k], data_l + (*sum_l)[k], data_r + (*sum_r)[k]);
                }
                // Fencepost, the first partition was not scheduled in the previous loop
                context->pool->enqueue(partition_task::execute, context, true, curr_depth + 1,
                           target_l, target_r, (*sum_l)[0], (*sum_r)[0], data_l, data_r);
            }
            // We create regular probe passes since we are in the deepest partition level
            else{
                // Nearly same as before when scheduling next round of partition passes, just have to pass less data
                for(uint64_t k = 0; k < part_count - 1; ++k){
                    context->pool->enqueue(join_task::execute, context, target_l + (*sum_l)[k], target_r + (*sum_r)[k],
                                           (*sum_l)[k + 1] - (*sum_l)[k], (*sum_r)[k + 1] - (*sum_r)[k]);
                }
                // Fencepost, the first partition was not scheduled in the previous loop
                context->pool->enqueue(join_task::execute, context, target_l, target_r, (*sum_l)[0], (*sum_r)[0]);
            }
        }
        return true;
    }

    void join_task::execute(task_context* context, tuple* data_l, tuple* data_r, uint64_t size_l, uint64_t size_r){
        // Check if this was the final join task, if so finish up the thread pool
        uint64_t value = ++(context->join_count);
        // Notify the waiting call to execute() in radix_join_mt that we are finished
        if(value == context->join_exp){
            {
                std::lock_guard<std::mutex> lk(context -> join_wait);
                context -> finished = true;
            }
            (context -> wait).notify_one();
        }

        // No need to bother on empty partitions
        if(size_l == 0 || size_r == 0){
            return;
        }
        // First we obtain a valid output buffer
        std::unique_lock<std::mutex> lock(context -> output_mutex, std::defer_lock);
        lock.lock();
        uint8_t index = context->free_index.back();
        context->free_index.pop_back();
        lock.unlock();
        // Calculate index%threads to obtain index of a currently unused output vector
        auto output = (*context->results)[index];
        // Run a simple no partitioning join on the given data
        nop_join join(data_l, data_r, size_l, size_r, context->table_size, output);
        join.execute();
        // We put the output buffer back into the unused stack
        lock.lock();
        context->free_index.emplace_back(index);
        lock.unlock();
    }

} // namespace algorithms