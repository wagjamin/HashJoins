//
// Created by benjamin on 21.10.18.
//

#ifndef HASHJOINS_RADIX_TASKS_H
#define HASHJOINS_RADIX_TASKS_H

#include <memory>
#include <vector>

/**
 * This file contains a variety of tasks used within the radix join which
 * are ultimately added to the thread pool.
 */
namespace algorithms{

    /// Helper struct containing basic information about the executed radix join
    struct task_context{

        /// The total num
        uint8_t radix_bits;
        uint8_t radix_passes;
        double table_size;
        // TODO add thread pool

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

        task(task_context& context);

        /// Context of the task within the overall join
        task_context& context;
    };

    /// Creates histograms of the hash values within the given partition
    struct partition_task: task{

        partition_task(task_context& context, bool spawn, uint8_t curr_depth, tuple* data_l, tuple* data_r,
                uint64_t size_l, uint64_t size_r, tuple* target_l, tuple* target_r);

        /// Boolean flag indicating whether the partition task should spawn its own scatter tasks
        bool spawn;
        /// Current depth within the radix join
        uint8_t curr_depth;
        /// Pointer to the data which has to be partitioned
        tuple *data_l, *data_r;
        /// Size of the data which has to be partitioned
        uint64_t size_l, size_r;
        /// Pointer to destination for potentially spawned scatter tasks
        tuple *target_l, *target_r;

        /// Perform the actual operation, returns a histogram of left and right partition
        std::pair<std::shared_ptr<std::vector<uint64_t>>, std::shared_ptr<std::vector<uint64_t>>> operator()();
    };

    /// Scatters data from a given partition into destination based on histograms
    struct scatter_task: task{

        scatter_task(task_context& context, bool spawn, uint8_t curr_depth, std::shared_ptr<std::vector<uint64_t>> sum_l,
                     std::shared_ptr<std::vector<uint64_t>> sum_r, tuple* data_l, tuple* data_r, uint64_t size_l,
                     uint64_t size_r, tuple* target_l, tuple* target_r);

        /// Boolean flag indicating whether the partition task should spawn its own scatter tasks
        bool spawn;
        /// Current depth within the radix join
        uint8_t curr_depth;
        /// Pointer to the prefix-summed histograms of a partition phase
        std::shared_ptr<std::vector<uint64_t>> sum_l, sum_r;
        /// Pointer to the data which has to be partitioned
        tuple *data_l, *data_r;
        /// Size of the data which has to be partitioned
        uint64_t size_l, size_r;
        /// Pointer to destination for potentially spawned scatter tasks
        tuple *target_l, *target_r;

        /// Perform the actual operation, no return value needed
        void operator()();
    };

    /// Performs the actual join on given data
    struct join_task: task{

        join_task(task_context& context, tuple* data_l, tuple* data_r, uint64_t size_l, uint64_t size_r,
                  std::shared_ptr<std::vector<triple>> output);

        /// Pointer to the data which has to be joined
        tuple *data_l, *data_r;
        /// Size of the data which has to be joined
        uint64_t size_l, size_r;
        /// Output vector
        std::shared_ptr<std::vector<triple>> output;

        void operator()();
    };

} // namespace algorithms

#endif //HASHJOINS_RADIX_TASKS_H
