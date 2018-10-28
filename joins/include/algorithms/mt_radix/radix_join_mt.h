//
// Created by benjamin on 21.10.18.
//

#ifndef HASHJOINS_RADIX_JOIN_MT_H
#define HASHJOINS_RADIX_JOIN_MT_H

#include <vector>
#include <tuple>
#include <memory>

namespace algorithms{

    /// Fully fledged multi-threaded radix join
    class radix_join_mt{

    public:
        /// First element is the value on which should be joined, second one the rid
        typedef std::tuple<uint64_t, uint64_t> tuple;
        /// Join result, containing (join_val, rid_left, rid_right)
        typedef std::tuple<uint64_t, uint64_t, uint64_t> triple;
        /***
        * We sadly need this unwieldy construction of types to reuse our original
        * nop_join implementation. Might change that in the future
        */
        typedef std::shared_ptr<std::vector<std::shared_ptr<std::vector<triple>>>> result_vec;

        /// Basic constructor
        radix_join_mt(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r,
                      uint8_t bits_per_pass, uint8_t passes);
        /// Join constructor with additional parameter
        radix_join_mt(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r, double table_size,
                      uint8_t threads, uint8_t bits_per_pass, uint8_t passes);

        /// Performs the actual join and writes result
        void execute();

        /// Set the result vector into which data should be written
        void set_res(result_vec res);

        /// Returns a pointer to the result vectors.
        result_vec get();

    private:
        /// Left join partner
        tuple* left;
        /// Right join partner
        tuple* right;
        /// Size of the left array
        uint64_t size_l;
        /// Size of the right array
        uint64_t size_r;
        /// table_size*|left| is the size of the hash table being built
        double table_size;
        /// number of threads on which the algorithm should be run
        uint8_t threads;
        /// Boolean flag indicating whether build was already called
        bool built;
        /// Pointer to the result vectors created by different partitions
        result_vec result;
        /// Number of radix bits on which data should be partitioned every pass
        uint8_t bits_per_pass;
        /// Number of partitioning passes that should be performed
        uint8_t passes;
    };

}; // namespace algorithms

#endif //HASHJOINS_RADIX_JOIN_MT_H
