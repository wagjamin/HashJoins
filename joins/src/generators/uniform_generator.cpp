//
// Created by benjamin on 13.09.18.
//

#include "generators/uniform_generator.h"
#include <random>

namespace generators {

    uniform_generator::uniform_generator(size_t min, uint64_t max, uint64_t count):
            built(false), min(min), max(max), count (count), data() {
        // Generate a pseudo random seed value
        std::random_device rd;
        seed = rd();
    }

    uniform_generator::uniform_generator(size_t min, uint64_t max, uint64_t count, uint64_t seed):
            built(false), min(min), max(max), count (count), seed(seed), data()
    {}

    void uniform_generator::build() {

        std::mt19937 gen(seed);
        std::uniform_int_distribution<uint64_t> dis(min, max);
        data.reserve(count);
        built = true;

        for(uint64_t i = 0; i < count; ++i){
            uint64_t val = dis(gen);
            data.emplace_back(val, i);
        }

    }

    std::vector<std::tuple<uint64_t, uint64_t>> uniform_generator::get_vec_copy() {
        if(!built){
            throw std::logic_error("copying may not be called before distribution has been built.");
        }
        return data;
    }

    uint64_t uniform_generator::get_count() {
        return count;
    }

} // namespace generators




