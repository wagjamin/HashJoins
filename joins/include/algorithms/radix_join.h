//
// Created by benjamin on 26.09.18.
//

#ifndef HASHJOINS_RADIX_JOIN_H
#define HASHJOINS_RADIX_JOIN_H

#include <memory>
#include <vector>
#include <tuple>

namespace algorithms{

    /// Single threaded single partition radix join
    class radix_join {

    public:
        /// First element is the value on which should be joined, second one the rid
        typedef std::tuple<uint64_t, uint64_t> tuple;
        /// Join result, containing (join_val, rid_left, rid_right)
        typedef std::tuple<uint64_t, uint64_t, uint64_t> triple;

        /// Basic constructor
        radix_join(std::shared_ptr<tuple[]> left, std::shared_ptr<tuple[]> right, uint64_t size_l, uint64_t size_r);
        /// Join constructor with additional parameter
        radix_join(std::shared_ptr<tuple[]> left, std::shared_ptr<tuple[]> right, uint64_t size_l, uint64_t size_r,
                 double table_size, uint8_t part_bits);

        /// Performs the actual join and writes result
        void execute();

        /// Returns a pointer to the result vector
        std::shared_ptr<std::vector<triple>> get();


    private:
        /// Left join partner
        std::shared_ptr<tuple[]> left;
        /// Right join partner
        std::shared_ptr<tuple[]> right;
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
        /// Result vector
        std::shared_ptr<std::vector<triple>> result;

        /// Hash function used for radix partitioning
        inline uint64_t hash1(uint64_t val);
        /// Hash function used for building
        inline uint64_t hash2(uint64_t val);

        /**
         * Partition parts of the data
         * @param data_s    beginning of the data to be partitioned
         * @param data_t    target array into which the values should be scattered
         * @param hist      pointer to the destination histogram, should be zeroed
         * @param count     number of elements within the current partition
         */
        void partition(tuple* data_s, tuple* data_t, uint64_t* hist, uint64_t count);

        struct hash_table;

    };

} // namespace algorithms


#endif //HASHJOINS_RADIX_JOIN_H
