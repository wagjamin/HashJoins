//
// Created by benjamin on 01.11.18.
//

#include "generators/incremental_generator.h"

namespace generators {

    incremental_generator::incremental_generator(uint64_t start, uint64_t ending):
            start(start), ending(ending), data(nullptr){}

    void incremental_generator::build() {

        // Create Array which will hold the tuples
        data = std::shared_ptr<std::tuple<uint64_t, uint64_t>[]>(new std::tuple<uint64_t, uint64_t>[get_count()]);

        // Populate the arrays
        for(uint64_t k = 0; k < get_count(); ++k){
            data[k] = {k, k};
        }

    }

    std::unique_ptr<std::vector<std::tuple<uint64_t, uint64_t>>> incremental_generator::get_vec_copy() {
        if(data == nullptr){
            throw std::logic_error("copying may not be called before distribution has been built.");
        }
        std::unique_ptr<std::vector<std::tuple<uint64_t, uint64_t>>> ptr =
                std::make_unique<std::vector<std::tuple<uint64_t, uint64_t>>>(get_count());
        // Copy elements into the new vector
        for(uint64_t k = 0; k < get_count(); ++k){
            (*ptr)[k] = data[k];
        }
        return ptr;
    }

    uint64_t incremental_generator::get_count() {
        return ending - start + 1;
    }

    std::shared_ptr<std::tuple<uint64_t, uint64_t>[]> incremental_generator::get() {
        if(data == nullptr){
            throw std::logic_error("get() may not be called before distribution has been built.");
        }
        return data;
    }


} // namespace generators
