//
// Created by benjamin on 13.09.18.
//

#include "generators/uniform_generator.h"
#include <random>

namespace generators {

    uniform_generator::uniform_generator(size_t min, uint64_t max, uint64_t count):
            min(min), max(max), count (count), data(new std::tuple<uint64_t, uint64_t>[count]) {}

    void uniform_generator::build() {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(min, max);
        // Write data into new array
        data = std::unique_ptr<std::tuple<uint64_t, uint64_t>[]>(new std::tuple<uint64_t, uint64_t>[count]);

        for(uint64_t i = 0; i < count; ++i){
            uint64_t val = dis(gen);
            data[i] = std::tuple<uint64_t, uint64_t>{val, i};
        }

    }

    uint64_t uniform_generator::get_count() {
        return count;
    }

    std::shared_ptr<std::tuple<uint64_t, uint64_t>[]> uniform_generator::get() {
        if(data == nullptr){
            throw std::logic_error("get() may not be called before distribution has been built.");
        }
        return data;
    }

} // namespace generators




