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
    struct Tuple{

    public:
        uint64_t value;
        uint64_t rid;

        Tuple(uint64_t value, uint64_t rid): value(value), rid(rid){};

    };

} // namespace algorithms

#endif //HASHJOINS_TUPLE_H
