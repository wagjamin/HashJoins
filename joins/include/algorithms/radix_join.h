//
// Benjamin Wagner 2018
//

#ifndef HASHJOINS_RADIX_JOIN_H
#define HASHJOINS_RADIX_JOIN_H

#include <memory>
#include <vector>
#include <tuple>

namespace algorithms {

    /// Single threaded single partition radix join
    class radix_join {
    public:
        /// First element is the value on which should be joined, second one the rid
        typedef std::tuple<uint64_t, uint64_t> tuple;
        /// Join result, containing (join_val, rid_left, rid_right)
        typedef std::tuple<uint64_t, uint64_t, uint64_t> triple;

        /// Basic constructor
        radix_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r);
        /// Join constructor with additional parameter
        radix_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r,
                 double table_size, uint8_t part_bits);

        /// Performs the actual join and writes the result
        void execute();

        /// Returns a reference to the result vector
        std::vector<triple>& get();

        /// Pass a result vector to the join, will be moved and may not be used further by the caller
        void set(std::vector<triple>& res_vec);

    private:
        /// Left join partner
        tuple* left;
        /// Right join partner
        tuple* right;
        /// Size of the left parameter
        uint64_t size_l;
        /// Size of the right parameter
        uint64_t size_r;
        /// size overhead in building hash tables
        double table_size;
        /// Partition bits per partition pass
        uint8_t part_bits;
        /// Total number of partitions being created
        uint32_t part_count;
        /// Boolean flag indicating whether build was already called
        bool built;
        /// Result vector
        std::vector<triple> result;

        /**
         * Partition parts of the data
         * @param data_s    beginning of the data to be partitioned
         * @param data_t    target array into which the values should be scattered
         * @param hist      pointer to the destination histogram, should be zeroed
         * @param count     number of elements within the current partition
         */
        void partition(tuple* data_s, tuple* data_t, uint64_t* hist, uint64_t count);
    };

}  // namespace algorithms


#endif  // HASHJOINS_RADIX_JOIN_H
