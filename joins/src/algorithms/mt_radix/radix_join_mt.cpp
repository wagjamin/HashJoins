//
// Created by benjamin on 21.10.18.
//

#include "algorithms/mt_radix/radix_join_mt.h"
#include "algorithms/mt_radix/radix_tasks.h"
#include "ThreadPool.h"
#include <utility>
#include <mutex>
#include <thread>
#include <future>

namespace algorithms{

    radix_join_mt::radix_join_mt(algorithms::radix_join_mt::tuple *left, algorithms::radix_join_mt::tuple *right,
                                 uint64_t size_l, uint64_t size_r, uint8_t bits_per_pass, uint8_t passes):
                radix_join_mt(left, right, size_l, size_r, 1.5, 4, bits_per_pass, passes)
    {}

    radix_join_mt::radix_join_mt(algorithms::radix_join_mt::tuple *left, algorithms::radix_join_mt::tuple *right,
                                 uint64_t size_l, uint64_t size_r, double table_size, uint8_t threads,
                                 uint8_t bits_per_pass, uint8_t passes):
                radix_join_mt(left, right, size_l, size_r, table_size, threads,
                              std::make_shared<std::vector<std::vector<triple>>>(threads), bits_per_pass, passes)
    {}

    radix_join_mt::radix_join_mt(algorithms::radix_join_mt::tuple *left, algorithms::radix_join_mt::tuple *right,
                                 uint64_t size_l, uint64_t size_r, double table_size, uint8_t threads,
                                 std::shared_ptr<std::vector<std::vector<algorithms::radix_join_mt::triple>>> result,
                                 uint8_t bits_per_pass, uint8_t passes):
            left(left), right(right), size_l(size_l), size_r(size_r), table_size(table_size), threads(threads),
            result(std::move(result)), bits_per_pass(bits_per_pass), passes(passes)
    {}

    // Run the actual radix join using the tools from radix_task
    void radix_join_mt::execute() {
        // Target Arrays used for scattering
        std::unique_ptr<tuple[]> target_l(new tuple[size_l]);
        std::unique_ptr<tuple[]> target_r(new tuple[size_r]);
        typedef std::pair<std::shared_ptr<std::vector<uint64_t>>, std::shared_ptr<std::vector<uint64_t>>> histograms;
        // Thread pool and context needed for further subtasks
        ThreadPool pool(threads);
        task_context context(bits_per_pass, passes, table_size, &pool);
        // Create histograms for partition tasks
        std::vector<std::future<histograms>> vec(threads);
        // Schedule the first round of partition tasks
        uint64_t range_l = size_l / threads;
        uint64_t range_r = size_r / threads;
        for(uint8_t k = 0; k < threads - 1; ++k){
            vec[k] = pool.enqueue(partition_task::execute, &context, false, 1, left + (k*range_l), right + (k*range_r),
                    range_l, range_r, nullptr, nullptr);
        }
        vec[threads-1] = pool.enqueue(partition_task::execute, &context, false, 1, left + ((threads - 1)*range_l),
                right + ((threads - 1)*range_r), size_l - ((threads - 1)*range_l), size_r - ((threads - 1)*range_r),
                                      nullptr, nullptr);
        std::vector<histograms> res(threads);
        // Get all histograms of the first partition runs
        for(uint8_t k = 0; k < threads; ++k){
            res[k] = vec[k].get();
        }

        // The following code builds history and prefix sums
        uint64_t part_count = (static_cast<uint64_t>(1) << static_cast<uint64_t>(bits_per_pass));
        // Global histogram
        std::vector<uint64_t> hist_l(part_count);
        std::vector<uint64_t> hist_r(part_count);
        // Build sum over the partition arrays creating global histogram
        for(uint64_t n = 0; n < threads; ++n){
            for(uint64_t k = 0; k < part_count; ++k){
                (hist_l)[k] += (*res[k].first)[k];
                (hist_r)[k] += (*res[k].second)[k];
            }
        }
        // Vectors containing the prefix sum
        std::vector<uint64_t> sum_l(part_count);
        std::vector<uint64_t> sum_r(part_count);
        // Build the actual prefix sum
        uint64_t cl = (sum_l)[0];
        uint64_t cr = (sum_l)[0];
        sum_l[0] = 0;
        sum_r[0] = 0;
        for(uint64_t k = 1; k < part_count; ++k){
            uint64_t temp_l = hist_l[k];
            uint64_t temp_r = hist_r[k];
            sum_l[k] = cl;
            sum_r[k] = cr;
            cl += temp_l;
            cr += temp_r;
        }

        /*
         * Now we can create the first group of scatter tasks.
         * For this we have to assign each thread a unique local partition within the target array.
         * Similar to the build process in the creation of the prefix sums we perform deferred additions
         * of the thread's histograms onto the prefix sum.
         * The already existing sum arrays will be updated continuously.
         * Since we are working on fairly small vectors here all data should still be L1 residing.
         */
        std::vector<std::future<bool>> scatter_vec(threads);
        // Schedule scatter tasks for all except the last thread
        for(uint8_t k = 0; k < threads - 1; ++k){
            // Create the thread specific scatter information
            auto local_l = std::make_shared<std::vector<uint64_t>>(sum_l);
            auto local_r = std::make_shared<std::vector<uint64_t>>(sum_r);
            // Run scatter task
            scatter_vec[k] = pool.enqueue(scatter_task::execute, &context, false, 1, local_l, local_r,
                    left + (k*range_l), right + (k*range_r), range_l, range_r, target_l.get(), target_r.get());
            // Update the sum vectors with thread's histogram data
            for(uint8_t j = 0; j < part_count; ++j){
                sum_l[j] += (*(res[k]).first)[j];
                sum_r[j] += (*(res[k]).second)[j];
            }
        }
        // Schedule the final scatter task, fairly complicated calling logic here
        scatter_vec[threads - 1] = pool.enqueue(scatter_task::execute, &context, false, 1,
                std::make_shared<std::vector<uint64_t>>(sum_l), std::make_shared<std::vector<uint64_t>>(sum_r),
                left + ((threads - 1)*range_l), right + ((threads - 1)*range_r), size_l - ((threads - 1)*range_l),
                size_r - ((threads - 1)*range_r), target_l.get(), target_r.get());
        // Join Scatter tasks again, as the first round forms a barrier
        for(uint8_t k = 0; k < threads; ++k){
            scatter_vec[k].get();
        }

        /*
         * TODO
         * Schedule Second round of spawning partition tasks
         * OR: schedule first and final round of join tasks
         */
    }

    std::shared_ptr<std::vector<std::vector<radix_join_mt::triple>>> radix_join_mt::get() {
        if(!built){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }
} // namespace algorithms