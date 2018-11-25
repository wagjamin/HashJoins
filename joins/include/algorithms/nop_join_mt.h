//
// Benjamin Wagner 2018
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

        /// Performs the actual join and writes result
        void execute();

        /// Set the result vector into which data should be written
        void set_res(std::shared_ptr<std::vector<triple>> res);

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
        /// Result vector into which the final triples get written
        std::vector<std::vector<triple>> result;

        /// Private minimal hash table implementation
        struct hash_table;

        /**
         * Builds a fraction of the left table, used in multi threaded setting
         * @param start   start index of the section of the left table that should be built (inclusive)
         * @param end     end index of the section of the left table that should be built (exclusive)
         */
        void build(uint64_t start, uint64_t end, hash_table* table);

        /**
         * Probes with a fraction of the right table, used in multi threaded setting
         * @param start   start index of the section of the right table that should be probed with (inclusive)
         * @param end     end index of the section of the right table that should be probed with (exclusive)
         * @param table   pointer to the hash table used for probing
         * @param t_num   thread index needed for output coordination
         */
        void probe(uint64_t start, uint64_t end, hash_table* table, uint8_t t_num);

        /// Hash function used for build process
        uint64_t hash(uint64_t val);


    };

}; // namespace algorithms

#endif //HASHJOINS_NOP_JOIN_MT_H
