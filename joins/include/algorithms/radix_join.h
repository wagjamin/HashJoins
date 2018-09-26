//
// Created by benjamin on 26.09.18.
//

#ifndef HASHJOINS_RADIX_JOIN_H
#define HASHJOINS_RADIX_JOIN_H

#include <memory>
#include <vector>
#include <tuple>

namespace algorithms{

    /// Very simple radix join using no threads at all
    class radix_join {

    public:
        /// First element is the value on which should be joined, second one the rid
        typedef std::tuple<uint64_t, uint64_t> tuple;
        /// Join result, containing (join_val, rid_left, rid_right)
        typedef std::tuple<uint64_t, uint64_t, uint64_t> triple;

        /// Basic constructor
        radix_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right);
        /// Join constructor with additional parameter
        radix_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right,
                 double table_size);

        /// Performs the actual join and writes result
        void execute();

        /// Returns a pointer to the result vector
        std::shared_ptr<std::vector<triple>> get();


    private:
        /// Left join partner
        std::shared_ptr<std::vector<tuple>> left;
        /// Right join partner
        std::shared_ptr<std::vector<tuple>> right;
        /// size overhead in building hash tables
        double table_size;
        /// Partition bits per partition pass
        uint8_t part_bits;
        /// Number of partitioning runs
        uint8_t part_runs;
        /// Result vector
        std::shared_ptr<std::vector<triple>> result;

        inline uint64_t hash(uint64_t val);
        /**
         * Partition parts of the data
         * @param data_s    beginning of the data to be partitioned
         * @param data_t    target array into which the values should be scattered
         * @param hist      pointer to the destination histogram, should be zeroed
         * @param count     number of elements within the current partition
         * @param start_bit bit at which partitioning should start
         */
        void partition(tuple* data_s, tuple* data_t, uint64_t* hist, uint64_t count, uint8_t start_bit);

        struct hash_table;

    };

} // namespace algorithms


#endif //HASHJOINS_RADIX_JOIN_H
