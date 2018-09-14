//
// Created by benjamin on 14.09.18.
//

#ifndef HASHJOINS_NOP_JOIN_H
#define HASHJOINS_NOP_JOIN_H

#include "join.h"

namespace algorithms{

    class nop_join: public join {

    public:
        /// Basic constructor, join will be running on 4 threads by default
        nop_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right);
        /// Join constructor with additional parameters
        nop_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right,
                 uint64_t threads, double table_size);

        /// Performs the actual join and writes result
        void build();

        /// Returns a pointer to the result vector
        std::shared_ptr<std::vector<uint64_t[3]>> get();


    private:
        /// Number of threads that should be used when performing the NOP join
        uint64_t threads;
        /// table_size*|left| is the size of the hash table being built
        double table_size;
        /// Left join partner
        std::shared_ptr<std::vector<tuple>> left;
        /// Right join partner
        std::shared_ptr<std::vector<tuple>> right;
        /// Result vector
        std::shared_ptr<std::vector<uint64_t[3]>> result;

    };

} // namespace algorithms

#endif //HASHJOINS_NOP_JOIN_H
