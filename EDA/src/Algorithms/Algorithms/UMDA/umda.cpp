#include "umda.h"
#include "../../Fitness/fitness_functions.h"
#include "../Utility/Random.h"
#include "../DataObjects/iteration_snapshot.h"
#include "../../Coordination/Communocation/DynamicArray.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>
#include <functional>
#include <cmath>
#include <mpi.h>
#include "../../Coordination/Communocation/MPIInfo.h"

umda::umda(int my, int lambda, int n, FitnessClass & fitnessclass) : EDABase(lambda, n, fitnessclass){
	this->_mu = my;
}

void umda::run_iterations(int iterations, std::function<bool(const EDABase&)> stopping_criteria) {
    for (int i = 0; i < iterations; i++) { 
        sample();
        DynamicArray<uint16_t> indices = index_sort(sample_container);
        update_probability_distribution(sample_container, indices);
        log_state(sample_container, indices);
        if (stopping_criteria(*this)) return;
        _itr_count += 1;
    }
}

void umda::run_iterations(int iterations) {
    run_iterations(iterations, [](const EDABase& eda) -> bool { return false; });
}

void umda::update_probability_distribution(SampleContainer& sample_container) {
    DynamicArray<uint16_t> indices = index_sort(sample_container);
    update_probability_distribution(sample_container, indices);
}

void umda::update_probability_distribution(SampleContainer& sample_container, DynamicArray<uint16_t>& indices) {
    std::vector<uint16_t> new_dist(_n);

    for (int k = 0; k < _mu; k++) {
        int sample_index = indices[k] * _n;
        for (int j = 0; j < _n; j++) {
            new_dist[j] += sample_container.samples[sample_index + j];
        }
    }

    for (int k = 0; k < _n; k++) {
        probability_distribution[k] = std::round(0xffff * std::max(_lower, std::min(_upper, new_dist[k] / _mu)));
    }
}

void umda::force_update_probability_distribution(DynamicArray<uint16_t>& samples){

    if (samples.size() < _n) { throw new std::logic_error("Forcing update with less than n bits????"); }

    double sample_count = samples.size() / ((double)_n);

    std::vector<uint16_t> new_dist(_n);

    for (int k = 0; k < sample_count; k++) {
        int sample_index = k * _n;
        for (int j = 0; j < _n; j++) {
            new_dist[j] += samples[sample_index + j];
        }
    }

    for (int k = 0; k < _n; k++) {
        probability_distribution[k] = std::round(0xffff * std::max(_lower, std::min(_upper, new_dist[k] / sample_count)));
    }
}

void umda::force_update_probability_distribution(SampleContainer& samples) {
    std::logic_error("Not implemented!");
}

void umda::combine_probability_distributions(DynamicArray<uint16_t>& probability_distributions, double beta){
    if (probability_distributions.size() == 0) {
        return;
    }

    double probability_distribution_count = probability_distributions.size() / (_n * 1.0);

    std::unique_ptr<double[]> prob_sum = std::make_unique<double[]>(_n); // since they use the whole uint16_t might overflow when summed
    for (int i = 0; i < _n; i++) prob_sum[i] = 0;

    for (int i = 0; i < probability_distribution_count; i++) {
        int prob_index = i * _n;
        for (int j = 0; j < _n; j++) {
            prob_sum[j] += probability_distributions[prob_index + j];
        }
    }

    for (int j = 0; j < _n; j++) {
        prob_sum[j] = (1.0 - beta) * (prob_sum[j] / probability_distribution_count);
        this->probability_distribution[j] = std::round(prob_sum[j] + beta * this->probability_distribution[j]);
    }
}

