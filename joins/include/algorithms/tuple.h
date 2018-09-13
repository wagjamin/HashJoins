//
// Created by benjamin on 09.09.18.
//

#ifndef HASHJOINS_TUPLE_H
#define HASHJOINS_TUPLE_H

namespace algorithms{

    /**
     * Simple Tuple which will be the target of the Hash Join. Represents the value to be hashed
     * as well as the rid of the data element. Such a layout would be typical in a column store.
     */
    struct tuple{

    public:
        uint64_t value;
        uint64_t rid;

        tuple(uint64_t value, uint64_t rid): value(value), rid(rid){};
        tuple(): value(0), rid(0) {};
        /// Member-wise copy and move is fine
        ~tuple() = default;
        tuple(const tuple& t) = default;
        tuple(tuple&& t) = default;
        tuple& operator=(const tuple& t) = default;
        tuple& operator=(tuple&& t) = default;

    };

} // namespace algorithms

#endif //HASHJOINS_TUPLE_H
