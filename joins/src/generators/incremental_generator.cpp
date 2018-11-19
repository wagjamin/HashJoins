//
// Created by benjamin on 01.11.18.
//

#include "generators/incremental_generator.h"

namespace generators {

    incremental_generator::incremental_generator(uint64_t start, uint64_t ending):
            built(false), start(start), ending(ending), data(){}

    void incremental_generator::build() {

        data.reserve(get_count());
        built = true;

        // Populate the arrays
        for(uint64_t k = 0; k < get_count(); ++k){
            data.emplace_back(k, k);
        }

    }

    std::vector<std::tuple<uint64_t, uint64_t>> incremental_generator::get_vec_copy() {
        return data;
    }

    uint64_t incremental_generator::get_count() {
        return ending - start + 1;
    }


} // namespace generators
