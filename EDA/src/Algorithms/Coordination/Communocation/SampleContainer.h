#pragma once

#include <memory>
#include <mpi.h>
#include "DynamicArray.h"
#include <stdexcept>

class SampleContainer {
public:
    size_t sample_count;
    size_t sample_size;

    DynamicArray<uint16_t> samples;
    DynamicArray<uint16_t> sample_fitnesses;

    SampleContainer(size_t sample_count, size_t sample_size) : samples(sample_count* sample_size), sample_fitnesses(sample_count) {
        this->sample_count = sample_count;
        this->sample_size = sample_size;
    }

    void concat(const SampleContainer& other) {
        if (sample_size != other.sample_size) {
            throw std::logic_error("Trying to concat two sample containers with different sample sizes");
        }

        samples.concat(other.samples);
        sample_fitnesses.concat(other.sample_fitnesses);

        sample_count += other.sample_count;
    }
};