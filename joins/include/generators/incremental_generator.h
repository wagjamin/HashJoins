//
// Created by benjamin on 01.11.18.
//

#ifndef HASHJOINS_INCREMENTAL_GENERATOR_H
#define HASHJOINS_INCREMENTAL_GENERATOR_H

#include <memory>
#include <vector>
#include <tuple>

namespace generators{

    /**
     * Generates an incremental set of tuples from within a range.
     * The RIDs are guaranteed to be unique within the whole dataset, but
     * do not follow a certain distribution (since they are not relevant
     * to the join itself).
     */
    class incremental_generator{

    public:
        /**
         * Initializes the uniform number generator. The first tuple will have value start, then ext one start + 1
         * and so on until a final ending value is reached (inclusively).
         */
        incremental_generator(uint64_t start, uint64_t ending);
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
        /// Return an exclusive copy of the data in a vector
        std::unique_ptr<std::vector<std::tuple<uint64_t, uint64_t>>> get_vec_copy();

        /// Member-wise copy and move is fine
        ~incremental_generator() = default;
        incremental_generator(const incremental_generator& t) = default;
        incremental_generator(incremental_generator&& t) = default;
        incremental_generator& operator=(const incremental_generator& t) = default;
        incremental_generator& operator=(incremental_generator&& t) = default;
    private:
        uint64_t start;
        uint64_t ending;
        std::shared_ptr<std::tuple<uint64_t, uint64_t>[]> data;
    };

} // namespace generators


#endif //HASHJOINS_INCREMENTAL_GENERATOR_H
