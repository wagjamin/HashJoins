//
// Created by benjamin on 04.11.18.
//

#ifndef HASHJOINS_ZIPF_GENERATOR_H
#define HASHJOINS_ZIPF_GENERATOR_H

#include <memory>
#include <vector>
#include <tuple>

namespace generators{

    /**
     * Generates zipf distributed data within a certain range.
     * Then returns an array of tuples where the keys follow the given
     * distribution.
     * The RIDs are guaranteed to be unique within the whole dataset, but
     * do not follow a certain distribution (since they are not relevant
     * to the join itself).
     */
    class zipf_generator{

    public:
        /**
         * Initializes the zipf generator, alpha is the zipf value
         * (higher alpha leads to larger bias towards smaller values)
         */
        zipf_generator(uint64_t max, double alpha, size_t count);
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
        ~zipf_generator() = default;
        zipf_generator(const zipf_generator& t) = default;
        zipf_generator(zipf_generator&& t) = default;
        zipf_generator& operator=(const zipf_generator& t) = default;
        zipf_generator& operator=(zipf_generator&& t) = default;

    private:
        uint64_t max;
        double alpha;
        size_t count;
        std::shared_ptr<std::tuple<uint64_t, uint64_t>[]> data;
    };

}

#endif //HASHJOINS_ZIPF_GENERATOR_H
