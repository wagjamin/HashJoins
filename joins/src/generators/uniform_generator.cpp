//
// Created by benjamin on 13.09.18.
//

#include "generators/uniform_generator.h"
#include <random>

namespace generators {

    uniform_generator::uniform_generator(size_t min, uint64_t max, uint64_t count):
            min(min), max(max), count (count), data(nullptr) {}

    void uniform_generator::build() {

        data = std::make_shared<std::vector<std::tuple<uint64_t, uint64_t>>>(count);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(min, max);

        for(uint64_t i = 0; i < count; ++i){
            uint64_t val = dis(gen);
            (*data.get())[i] = std::tuple<uint64_t, uint64_t>{val, i};
        }

    }

    std::shared_ptr<std::vector<std::tuple<uint64_t, uint64_t>>> uniform_generator::get() {
        if(data == nullptr){
            throw std::logic_error("get() may not be called before distribution has been built.");
        }
        return data;
    }



} // namespace generators




