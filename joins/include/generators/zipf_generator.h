//
// Created by benjamin on 09.09.18.
//

#ifndef HASHJOINS_ZIPF_GENERATOR_H
#define HASHJOINS_ZIPF_GENERATOR_H

#include<memory>
#include<vector>

class uniform_generator{

public:
    /**
     * Initializes the uniform number generator
     */
    uniform_generator(size_t count, uint64_t min, uint64_t max): count(count), min(min), max(max){}
    /**
     * Performs the actual build process of creating data according to the distribution parameters
     */
    void build();
    /**
     * Returns a shared vector containing the generated data
     */
    std::shared_ptr<std::vector<uint64_t>> get();

private:
    size_t count;
    uint64_t min;
    uint64_t max;
    std::shared_ptr<std::vector<uint64_t>> data;

};

#endif //HASHJOINS_ZIPF_GENERATOR_H
