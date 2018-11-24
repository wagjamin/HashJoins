//
// Benjamin Wagner 2018
//

#include "generators/zipf_generator.h"
#include <cmath>
#include <random>

namespace generators{

    zipf_generator::zipf_generator(uint64_t max, double alpha, size_t count):
            built(false), max(max), alpha(alpha), count(count), data(){
        // Generate a pseudo random seed value
        std::random_device rd;
        seed = rd();
    }

    zipf_generator::zipf_generator(uint64_t max, double alpha, size_t count, uint64_t seed):
        built(false), max(max), alpha(alpha), count(count), seed(seed), data(){
    };

    // Adapted C Zipf Generator from: https://stackoverflow.com/a/48279287/4945380
    void zipf_generator::build() {
        built = true;
        data.reserve(count);

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

        // Pull a uniform random number (0 < z < 1)
        std::random_device rd;
        std::mt19937 gen(seed);
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

            data.emplace_back(zipf_value, k);
        }
    }

    uint64_t zipf_generator::get_count() {
        return count;
    }

    std::vector<std::tuple<uint64_t, uint64_t>> zipf_generator::get_vec_copy() {
        if(!built){
            throw std::logic_error("copying may not be called before distribution has been built.");
        }
        return data;
    }

}


