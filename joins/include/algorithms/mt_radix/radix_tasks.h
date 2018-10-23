//
// Created by benjamin on 21.10.18.
//

#ifndef HASHJOINS_RADIX_TASKS_H
#define HASHJOINS_RADIX_TASKS_H

#include <memory>
#include <vector>
#include "ThreadPool.h"

/**
 * This file contains a variety of tasks used within the radix join which
 * are ultimately added to the thread pool.
 */
namespace algorithms{

    /// Helper struct containing basic information about the executed radix join
    struct task_context{

        task_context(uint8_t radix_bits, uint8_t radix_passes, double table_size, ThreadPool* pool);

        /// The total num
        uint8_t radix_bits;
        uint8_t radix_passes;
        double table_size;
        ThreadPool* pool;

        /// Hash function used throughout the partition phase
        uint64_t hash1(uint64_t val);
        /// Hash function used throughout the build and probe phase
        uint64_t hash2(uint64_t val);

    };

    struct task{
        /// First element is the value on which should be joined, second one the rid
        typedef std::tuple<uint64_t, uint64_t> tuple;
        /// Join result, containing (join_val, rid_left, rid_right)
        typedef std::tuple<uint64_t, uint64_t, uint64_t> triple;
    };

    /// Creates histograms of the hash values within the given partition
    struct partition_task: task{

        /// Perform the actual operation, returns a histogram of left and right partition
        static std::pair<std::shared_ptr<std::vector<uint64_t>>, std::shared_ptr<std::vector<uint64_t>>> execute(
                task_context* context, bool spawn, uint8_t curr_depth, tuple* data_l, tuple* data_r,
                uint64_t size_l, uint64_t size_r, tuple* target_l, tuple* target_r);
    };

    /// Scatters data from a given partition into destination based on histograms
    struct scatter_task: task{

        /// Perform the actual operation, no return value needed
        static bool execute(task_context* context, bool spawn, uint8_t curr_depth, std::shared_ptr<std::vector<uint64_t>> sum_l,
                     std::shared_ptr<std::vector<uint64_t>> sum_r, tuple* data_l, tuple* data_r, uint64_t size_l,
                     uint64_t size_r, tuple* target_l, tuple* target_r);
    };

    /// Performs the actual join on given data
    struct join_task: task{

        static void execute(task_context* context, tuple* data_l, tuple* data_r, uint64_t size_l, uint64_t size_r,
                     std::shared_ptr<std::vector<triple>> output);
    };

} // namespace algorithms

#endif //HASHJOINS_RADIX_TASKS_H
