//
// Benjamin Wagner 2018
//

#ifndef HASHJOINS_HASH_HELPERS_H
#define HASHJOINS_HASH_HELPERS_H

#include <memory>
#include <mutex>
#include <tuple>

namespace helpers {

    /// Murmur 3 hash function for 64 bit uint keys, inline to keep all in header file
    inline uint64_t murmur3(uint64_t val) {
        // Murmur 3 taken from "A Seven-Dimensional Analysis of Hashing Methods and its
        // Implications on Query Processing" by Richter et al
        val ^= val >> 33;
        val *= 0xff51afd7ed558ccd;
        val ^= val >> 33;
        val *= 0xc4ceb9fe1a85ec53;
        val ^= val >> 33;
        return val;
    }

    /// First element is the value on which should be joined, second one the rid
    typedef std::tuple<uint64_t, uint64_t> tuple;

    /// Overflow bucket used for chaining
    struct overflow {
        tuple t;
        std::unique_ptr<overflow> next;

        explicit overflow(tuple t): t(t){}
    };

    /// Chained Hash Table used for ST No Partitioning Join, every bucket is protected with a latch
    struct latched_hash_table{
        /// One of the hash table entries
        struct bucket{
            std::mutex lock;
            uint32_t count;
            tuple t1;
            tuple t2;
            std::unique_ptr<overflow> next;

            /// Default constructor
            bucket(): count(0), next(nullptr) {}
        };

        std::unique_ptr<bucket[]> arr;
        uint64_t size;

        explicit latched_hash_table(uint64_t size): size(size){
                arr = std::make_unique<bucket[]>(size);
        }
    };

    /// Simple chained hash table used within the nop join
    struct hash_table{
        /// One of the hash table entries
        struct bucket{
            uint32_t count;
            tuple t1;
            tuple t2;
            std::unique_ptr<overflow> next;

            /// Default constructor
            bucket(): count(0), next(nullptr) {}
        };

        std::unique_ptr<bucket[]> arr;
        uint64_t size;

        explicit hash_table(uint64_t size): size(size){
            arr = std::make_unique<bucket[]>(size);
        }

    };

}  // namespace helpers


#endif  // HASHJOINS_HASH_HELPERS_H
