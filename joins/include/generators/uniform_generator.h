//
// Created by benjamin on 09.09.18.
//

#ifndef HASHJOINS_UNIFORM_GENERATOR_H
#define HASHJOINS_UNIFORM_GENERATOR_H

#include <memory>
#include <vector>
#include <tuple>

namespace generators{

    /**
     * Generates uniformly distributed data within a certain range.
     * Then returns an array of tuples where the keys follow the given
     * distribution.
     * The RIDs are guaranteed to be unique within the whole dataset, but
     * do not follow a certain distribution (since they are not relevant
     * to the join itself).
     */
    class uniform_generator{

    public:
        /// Initializes the uniform number generator
        uniform_generator(uint64_t min, uint64_t max, size_t count);
        /// For Benchmarking purposes the generator can be initialized with a fixed seed
        uniform_generator(uint64_t min, uint64_t max, size_t count, uint64_t seed);
        /// Performs the actual build process of creating data according to the distribution parameters
        void build();
        /// Return the number of elements generated
        uint64_t get_count();
        /// Return an exclusive copy of the data in a vector. May not be called before previous call to build.
        std::vector<std::tuple<uint64_t, uint64_t>> get_vec_copy();

        /// Member-wise copy and move is fine
        ~uniform_generator() = default;
        uniform_generator(const uniform_generator& t) = default;
        uniform_generator(uniform_generator&& t) = default;
        uniform_generator& operator=(const uniform_generator& t) = default;
        uniform_generator& operator=(uniform_generator&& t) = default;

    private:
        bool built;
        uint64_t min;
        uint64_t max;
        size_t count;
        /// Seed value, depends on which constructor is called
        uint64_t seed;
        std::vector<std::tuple<uint64_t, uint64_t>> data;
    };

} // namespace generators

#endif //HASHJOINS_UNIFORM_GENERATOR_H
