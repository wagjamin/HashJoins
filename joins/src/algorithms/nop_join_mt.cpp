//
// Created by benjamin on 06.10.18.
//

#include "algorithms/nop_join_mt.h"
#include <utility>
#include <mutex>
#include <thread>

namespace algorithms{

    /// Simple chained hash table used within the nop join
    struct nop_join_mt::hash_table{
        /// One of the hash table entries
        struct bucket{

            /// Overflow bucket used for chaining
            struct overflow{
                tuple t;
                std::unique_ptr<overflow> next;

                overflow(tuple t): t(t){}
            };

            // TODO Lightweight CMP SWP lock in the future
            std::mutex lock;
            uint32_t count;
            tuple t1;
            tuple t2;
            std::unique_ptr<overflow> next;

            /// Default constructor
            bucket(): count(0), next(nullptr) {}

        };

        std::unique_ptr<bucket[]> arr;
        uint64_t size;

        explicit hash_table(uint64_t size): size(size){
            arr = std::make_unique<bucket[]>(size);
        }

    };

    nop_join_mt::nop_join_mt(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r):
            nop_join_mt(left, right, size_l, size_r, 1.5, 4){}

    nop_join_mt::nop_join_mt(tuple* left, tuple* right, uint64_t size_l,
                             uint64_t size_r, double table_size, uint8_t threads):
            nop_join_mt(left, right, size_l, size_r, table_size, threads,
                        std::make_shared<std::vector<std::vector<triple>>>()){}

    nop_join_mt::nop_join_mt(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r,
                       double table_size, uint8_t threads, std::shared_ptr<std::vector<std::vector<triple>>> result):
            left(left), right(right), size_l(size_l), size_r(size_r),
            table_size(table_size), threads(threads), built(false), result(std::move(result)){}

    void nop_join_mt::execute() {
        built = true;
        hash_table table(static_cast<uint64_t>(table_size * size_l));
        // Build Phase:
        // Chunk size per thread
        uint64_t offset = size_l/threads;
        std::thread arr[threads];
        // Start all threads except for the final one
        for(uint8_t curr_t = 0; curr_t < threads - 1; ++curr_t){
            arr[curr_t] = std::thread(&nop_join_mt::build, this, curr_t * offset, (curr_t + 1) * offset, &table);
        }
        // Start final thread
        arr[threads - 1] = std::thread(&nop_join_mt::build, this,(threads - 1) * offset, size_l, &table);
        // Join threads again, afterwards build phase is done
        for(uint8_t curr_t = 0; curr_t < threads; ++curr_t){
            arr[curr_t].join();
        }
        // Probe Phase:
        offset = size_r/threads;
        std::thread arr2[threads];
        for(uint8_t curr_t = 0; curr_t < threads - 1; ++curr_t){
            result->push_back(std::vector<triple>());
            arr2[curr_t] = std::thread(&nop_join_mt::probe, this, curr_t * offset,
                    (curr_t + 1) * offset, &table, &(*result)[curr_t]);
        }
        // Start final thread
        result->push_back(std::vector<triple>());
        arr[threads - 1] = std::thread(&nop_join_mt::probe, this,(threads - 1) * offset, size_l,
                &table, &(*result)[threads - 1]);
        // Join threads again, afterwards probe phase is done as well
        for(uint8_t curr_t = 0; curr_t < threads; ++curr_t){
            arr[curr_t].join();
        }
    }

    void nop_join_mt::build(uint64_t start, uint64_t end, hash_table* table) {
        for(uint64_t k = start; k < end; ++k) {
            tuple &curr = left[k];
            uint64_t index = std::get<0>(curr) % table->size;
            hash_table::bucket &bucket = table->arr[index];
            // Critical Section
            bucket.lock.lock();
            // Follow overflow buckets
            switch (bucket.count) {
                case 0:
                    bucket.t1 = curr;
                    break;
                case 1:
                    bucket.t2 = curr;
                    break;
                case 2:
                    bucket.next = std::make_unique<hash_table::bucket::overflow>(curr);
                    break;
                default:
                    hash_table::bucket::overflow *ptr = bucket.next.get();
                    // Follow pointer indirection
                    for (uint64_t i = 0; i < static_cast<uint64_t>(bucket.count - 3); i++) {
                        ptr = ptr->next.get();
                    }
                    // Create new bucket containing tuple
                    ptr->next = std::make_unique<hash_table::bucket::overflow>(curr);
            }
            ++bucket.count;
            bucket.lock.unlock();
        }
    }

    void nop_join_mt::probe(uint64_t start, uint64_t end, hash_table* table, std::vector<triple>* result) {
        for(uint64_t k = start; k < end; ++k){
            tuple& curr = right[k];
            uint64_t index = std::get<0>(curr) % table->size;
            hash_table::bucket& bucket = table->arr[index];
            // Follow overflow buckets
            if(bucket.count > 2){
                hash_table::bucket::overflow* curr_over = bucket.next.get();
                for(uint64_t i = 0; i < static_cast<uint64_t>(bucket.count - 2); ++i){
                    if(std::get<0>(curr_over->t) == std::get<0>(curr)){
                        result->emplace_back(std::get<0>(curr_over->t), std::get<1>(curr_over->t), std::get<1>(curr));
                    }
                    curr_over = curr_over->next.get();
                }
            }
            // Look at second tuple
            if(bucket.count > 1 && std::get<0>(bucket.t2) == std::get<0>(curr)){
                result->emplace_back(std::get<0>(bucket.t2), std::get<1>(bucket.t2), std::get<1>(curr));
            }
            // Look at first tuple
            if(bucket.count > 0 && std::get<0>(bucket.t1) == std::get<0>(curr)){
                result->emplace_back(std::get<0>(bucket.t1), std::get<1>(bucket.t1), std::get<1>(curr));
            }
        }
    }

    inline uint64_t nop_join_mt::hash(uint64_t val) {
        return val;
    }

    std::shared_ptr<std::vector<std::vector<nop_join_mt::triple>>> nop_join_mt::get(){
        if(!built){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }

} // namespace algorithms
