//
// Benjamin Wagner 2018
//

#include "algorithms/mt_radix/radix_join_mt.h"
#include "algorithms/mt_radix/radix_tasks.h"
#include <utility>
#include <future>

namespace algorithms{

    radix_join_mt::radix_join_mt(radix_join_mt::tuple *left, tuple *right, uint64_t size_l, uint64_t size_r,
                                 uint8_t bits_per_pass, uint8_t passes):
                radix_join_mt(left, right, size_l, size_r, 1.5, 4, bits_per_pass, passes)
    {}

    radix_join_mt::radix_join_mt(radix_join_mt::tuple *left, tuple *right, uint64_t size_l, uint64_t size_r,
                                 double table_size, uint8_t threads, uint8_t bits_per_pass, uint8_t passes):
            left(left), right(right), size_l(size_l), size_r(size_r), table_size(table_size), threads(threads),
            built(false), result(threads), bits_per_pass(bits_per_pass), passes(passes) {}

    // Run the actual radix join using the tools from radix_task
    void radix_join_mt::execute() {
        // No results on empty datasets
        if(size_l == 0 || size_r == 0){
            built = true;
            return;
        }
        // Target Arrays used for scattering
        std::unique_ptr<tuple[]> target_l(new tuple[size_l]);
        std::unique_ptr<tuple[]> target_r(new tuple[size_r]);
        typedef std::pair<std::shared_ptr<std::vector<uint64_t>>, std::shared_ptr<std::vector<uint64_t>>> histograms;
        // Thread pool and context needed for further subtasks
        ThreadPool pool(threads);
        task_context context(bits_per_pass, passes, threads, table_size, &pool, result);
        // Will get unlocked once all partition tasks are finished
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
                (hist_l)[k] += (*res[n].first)[k];
                (hist_r)[k] += (*res[n].second)[k];
            }
        }
        // Vectors containing the prefix sum
        std::vector<uint64_t> sum_l(part_count);
        std::vector<uint64_t> sum_r(part_count);
        // Build the actual prefix sum
        uint64_t cl = (hist_l)[0];
        uint64_t cr = (hist_r)[0];
        sum_l[0] = 0;
        sum_r[0] = 0;
        for(uint64_t k = 1; k < part_count; ++k){
            sum_l[k] = cl;
            sum_r[k] = cr;
            cl += hist_l[k];
            cr += hist_r[k];
        }

        /*
         * Now we can create the first group of scatter tasks.
         * For this we have to assign each thread a unique local partition within the target array.
         * Similar to the build process in the creation of the prefix sums we perform deferred additions
         * of the thread's histograms onto the prefix sum.
         * Since we are working on fairly small vectors here all data should still be L1 residing.
         */
        std::vector<std::future<bool>> scatter_vec(threads);
        // Schedule scatter tasks for all except the last thread
        // Create copies of the sum arrays
        auto localsum_l = sum_l;
        auto localsum_r = sum_r;
        for(uint8_t k = 0; k < threads - 1; ++k){
            // Create the thread specific scatter information, have to copy the arrays
            auto local_l = std::make_shared<std::vector<uint64_t>>(localsum_l);
            auto local_r = std::make_shared<std::vector<uint64_t>>(localsum_r);
            // Run scatter task
            scatter_vec[k] = pool.enqueue(scatter_task::execute, &context, false, 1, local_l, local_r,
                    left + (k*range_l), right + (k*range_r), range_l, range_r, target_l.get(), target_r.get());
            // Update the sum vectors with thread's histogram data
            for(uint64_t j = 0; j < part_count; ++j){
                localsum_l[j] += (*(res[k]).first)[j];
                localsum_r[j] += (*(res[k]).second)[j];
            }
        }
        // Schedule the final scatter task, fairly complicated calling logic here
        scatter_vec[threads - 1] = pool.enqueue(scatter_task::execute, &context, false, 1,
                std::make_shared<std::vector<uint64_t>>(localsum_l), std::make_shared<std::vector<uint64_t>>(localsum_r),
                left + ((threads - 1)*range_l), right + ((threads - 1)*range_r), size_l - ((threads - 1)*range_l),
                size_r - ((threads - 1)*range_r), target_l.get(), target_r.get());
        // Join Scatter tasks again, as the first round forms a barrier
        for(uint8_t k = 0; k < threads; ++k){
            scatter_vec[k].get();
        }

        // If we are only running one pass we now have to schedule the final build and probe tasks
        if(passes == 1){
            // Schedule partition tasks
            for(uint64_t k = 0; k < part_count; ++k){
                pool.enqueue(join_task::execute, &context, target_l.get() + sum_l[k], target_r.get() + sum_r[k],
                             hist_l[k], hist_r[k]);
            }
        }
        // We have to perform more partitions and scatters
        else {
            // We schedule the second round of partition tasks, but this time with automatic subtask spawning
            for(uint64_t k = 0; k < part_count; ++k){
                pool.enqueue(partition_task::execute, &context, true, 2, target_l.get() + sum_l[k],
                             target_r.get() + sum_r[k], hist_l[k], hist_r[k], left + sum_l[k], right + sum_r[k]);
            }
        }
        built = true;
        // Wait until all last level join tasks are finished, then we can finish up
        std::unique_lock<std::mutex> lk(context.join_wait);
        context.wait.wait(lk, [&context]{return context.finished;});
        pool.finish();
    }

    std::vector<std::vector<radix_join_mt::triple>>& radix_join_mt::get(){
        if(!built){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }

    void radix_join_mt::set(std::vector<std::vector<radix_join_mt::triple>> &res_vec) {
        // The vector is moved for maximum performance. The vector cannot be used by the caller afterwards.
        result = std::move(res_vec);
        // Set built to false again, since data was not built into the new vector
        built = false;
    }

} // namespace algorithms