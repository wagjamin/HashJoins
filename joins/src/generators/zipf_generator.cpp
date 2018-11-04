//
// Created by benjamin on 04.11.18.
//

#include "generators/zipf_generator.h"
#include <cmath>
#include <random>

namespace generators{

    zipf_generator::zipf_generator(uint64_t max, double alpha, size_t count):
        max(max), alpha(alpha), count(count), data(nullptr){}

    // Adapted C Zipf Generator from: https://stackoverflow.com/a/48279287/4945380
    void zipf_generator::build() {
        double c = 0;                      // Normalization constant
        double sum_probs[max+1];            // Pre-calculated sum of probabilities
        double z;                          // Uniform random number (0 < z < 1)
        uint64_t zipf_value = 0;           // Computed exponential value to be returned
        uint64_t i;                        // Loop counter
        uint64_t low, high, mid;           // Binary-search bounds

        // Compute normalization constants
        for (i=1; i<=max; i++) {
            c = c + (1.0 / pow(i, alpha));
        }
        c = 1.0 / c;
        sum_probs[0] = 0;
        for (i=1; i<=max; i++) {
            sum_probs[i] = sum_probs[i-1] + c / pow(i, alpha);
        }

        // Initialize result array
        data = std::shared_ptr<std::tuple<uint64_t, uint64_t>[]>(new std::tuple<uint64_t, uint64_t>[count]);

        // Pull a uniform random number (0 < z < 1)
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        // Generate the random numbers
        for(uint64_t k = 0; k < count; ++k){
            do
            {
                z = dis(gen);
            } while ((z == 0) || (z == 1));

            // Map z to the value
            low = 1, high = max;
            do {
                mid = static_cast<uint64_t>(floor((low+high)/2.0));
                if (sum_probs[mid] >= z && sum_probs[mid-1] < z) {
                    zipf_value = mid;
                    break;
                } else if (sum_probs[mid] >= z) {
                    high = mid-1;
                } else {
                    low = mid+1;
                }
            } while (low <= high);

            data[k] = {zipf_value, k};
        }
    }

    uint64_t zipf_generator::get_count() {
        return count;
    }

    std::unique_ptr<std::vector<std::tuple<uint64_t, uint64_t>>> zipf_generator::get_vec_copy() {
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

    std::shared_ptr<std::tuple<uint64_t, uint64_t>[]> zipf_generator::get() {
        if(data == nullptr){
            throw std::logic_error("get() may not be called before distribution has been built.");
        }
        return data;
    }

}


