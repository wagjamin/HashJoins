//
// Created by benjamin on 09.09.18.
//

#ifndef HASHJOINS_JOIN_H
#define HASHJOINS_JOIN_H

#include <vector>
#include <memory>

namespace algorithms{

    /**
     * Base Class for all joins.
     */
    class join{

    public:
        join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right) = 0;

        ~join() = 0;


    };



} // namespace algorithms

#endif //HASHJOINS_JOIN_H
