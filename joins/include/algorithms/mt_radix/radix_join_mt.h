//
// Benjamin Wagner 2018
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

        /// Basic constructor
        radix_join_mt(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r,
                      uint8_t bits_per_pass, uint8_t passes);
        /// Join constructor with additional parameter
        radix_join_mt(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r, double table_size,
                      uint8_t threads, uint8_t bits_per_pass, uint8_t passes);

        /// Performs the actual join and writes result
        void execute();

        /// Returns a reference to the result vectors
        std::vector<std::vector<triple>>& get();

        /// Pass a result vector to the join, will be moved and may not be used further by the caller
        void set(std::vector<std::vector<triple>>& res_vec);

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
        std::vector<std::vector<triple>> result;
        /// Number of radix bits on which data should be partitioned every pass
        uint8_t bits_per_pass;
        /// Number of partitioning passes that should be performed
        uint8_t passes;
    };

}; // namespace algorithms

#endif //HASHJOINS_RADIX_JOIN_MT_H
