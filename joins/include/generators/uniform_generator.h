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
        /**
         * Initializes the uniform number generator
         */
        uniform_generator(uint64_t min, uint64_t max, size_t count);
        /**
         * Performs the actual build process of creating data according to the distribution parameters
         */
        void build();
        /**
         * Returns a shared array containing the generated data.
         * May not be called without previous call to build.
         * The first tuple element contains the values following the distribution, the
         * second tuple element contains the RID.
         */
        std::shared_ptr<std::tuple<uint64_t, uint64_t>[]> get();
        /// Return the number of elements generated
        uint64_t get_count();

        /// Member-wise copy and move is fine
        ~uniform_generator() = default;
        uniform_generator(const uniform_generator& t) = default;
        uniform_generator(uniform_generator&& t) = default;
        uniform_generator& operator=(const uniform_generator& t) = default;
        uniform_generator& operator=(uniform_generator&& t) = default;

    private:
        uint64_t min;
        uint64_t max;
        size_t count;
        std::shared_ptr<std::tuple<uint64_t, uint64_t>[]> data;
    };

} // namespace generators

#endif //HASHJOINS_UNIFORM_GENERATOR_H
