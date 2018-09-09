//
// Created by benjamin on 09.09.18.
//

#ifndef HASHJOINS_JOIN_H
#define HASHJOINS_JOIN_H

#include<vector>

namespace algorithms{

    /**
     * Base Class for all joins.
     */
    class Join{

    public:
        Join(Tuple* left, Tuple* right, size_t size_left, size_t size_right);
        Join(std::vector<uint64_t> left, std::vector<uint64_t> right);

        ~Join();


    };



} // namespace algorithms

#endif //HASHJOINS_JOIN_H
