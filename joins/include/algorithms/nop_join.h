//
// Benjamin Wagner 2018
//

#ifndef HASHJOINS_NOP_JOIN_H
#define HASHJOINS_NOP_JOIN_H

#include "algorithms/hash_helpers.h"
#include <memory>
#include <vector>
#include <tuple>

namespace algorithms{

    /// Single threaded no partitioning join
    class nop_join {

    public:
        /// First element is the value on which should be joined, second one the rid
        typedef std::tuple<uint64_t, uint64_t> tuple;
        /// Join result, containing (join_val, rid_left, rid_right)
        typedef std::tuple<uint64_t, uint64_t, uint64_t> triple;

        /// Basic constructor
        nop_join(tuple* left, tuple* right, uint64_t size_l, uint64_t size_r);
        /// Join constructor with additional parameter
        nop_join(tuple* left, tuple* right,
                 uint64_t size_l, uint64_t size_r, double table_size);
        /// Join constructor  offering maximum flexibility, 'result' is moved into the join object
        nop_join(tuple* left, tuple* right,
                 uint64_t size_l, uint64_t size_r, double table_size, std::vector<triple>& result);

        /// Performs the actual join and writes result
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
        /// Size of the left array
        uint64_t size_l;
        /// Size of the right array
        uint64_t size_r;
        /// table_size*|left| is the size of the hash table being built
        double table_size;
        /// Boolean flag indicating whether build was already called
        bool built;
        /// Result vector
        std::vector<triple> result;

    };

} // namespace algorithms

#endif //HASHJOINS_NOP_JOIN_H
