//
// Created by benjamin on 06.10.18.
//

#ifndef HASHJOINS_NOP_JOIN_MT_H
#define HASHJOINS_NOP_JOIN_MT_H

#include <memory>
#include <vector>
#include <tuple>

namespace algorithms{

    /// Fully fledged NOP-Join implementation
    class nop_join_mt{

    public:
        /// First element is the value on which should be joined, second one the rid
        typedef std::tuple<uint64_t, uint64_t> tuple;
        /// Join result, containing (join_val, rid_left, rid_right)
        typedef std::tuple<uint64_t, uint64_t, uint64_t> triple;

        /// Basic constructor
        nop_join_mt(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r);
        /// Join constructor with additional parameter
        nop_join_mt(tuple* left, tuple* right,
                uint64_t size_l, uint64_t size_r, double table_size, uint8_t threads);
        /// Join constructor  offering maximum flexibility
        nop_join_mt(tuple* left, tuple* right,
                uint64_t size_l, uint64_t size_r, double table_size, uint8_t threads,
                        std::shared_ptr<std::vector<triple>> result);

        /// Performs the actual join and writes result
        void execute();

        /// Set the result vector into which data should be written
        void set_res(std::shared_ptr<std::vector<triple>> res);

        /// Returns a pointer to the result vector
        std::shared_ptr<std::vector<triple>> get();


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
        /// Result vector
        std::shared_ptr<std::vector<triple>> result;

        struct hash_table;

    };

}; // namespace algorithms

#endif //HASHJOINS_NOP_JOIN_MT_H
