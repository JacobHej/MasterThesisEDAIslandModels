#include "CGA.h"
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

CGA::CGA(int n, double k, FitnessClass& fitnessclass) : EDABase(2, n, fitnessclass) {
    _k = k;
    _mu = 0;
}


void CGA::run_iterations(int iterations, std::function<bool(const EDABase&)> stopping_criteria) {
    for (int i = 0; i < iterations; i++) { // iterations
        sample();
        DynamicArray<uint16_t> indices = index_sort(sample_container);
        _update_probability_distribution_with_best(sample_container);
        log_state(sample_container, indices);
        if (stopping_criteria(*this)) return;
        _itr_count += 1;
    }
}


void CGA::run_iterations(int iterations) {
    run_iterations(iterations, [](const EDABase& eda) -> bool { return false; });
}


void CGA::combine_probability_distributions(DynamicArray<uint16_t>& probability_distributions, double beta) {
    if (probability_distributions.size() == 0) {
        return;
    }

    double probability_distribution_count = probability_distributions.size() / (_n*1.0);

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

void CGA::_update_probability_distribution_with_best(SampleContainer& samples) {
    DynamicArray<uint16_t> indices = index_sort(samples); 
    DynamicArray<uint16_t> best(samples.samples.get() + indices[0] * _n, _n);
    int worst_index = indices[0] % 2 == 0 ? indices[0] + 1 : indices[0] - 1;
    DynamicArray<uint16_t> worst(samples.samples.get() + worst_index * _n, _n);
    _update_probability_distribution(best, worst, 1);
}

void CGA::_update_probability_distribution_with_x_best(SampleContainer& samples, int x) {
    std::vector<int> bests;

    for (int i = 0; i < samples.sample_fitnesses.size(); i += 2) {
        bests.push_back(samples.sample_fitnesses[i] >= samples.sample_fitnesses[i + 1] ? i : i + 1);
    }

    std::sort(bests.begin(), bests.end(),
        [&](const int& a, const int& b) {
            return samples.sample_fitnesses[a] > samples.sample_fitnesses[b];
        });

    x = x < bests.size() ? x : bests.size();

    DynamicArray<double> Offsets(_n);

    //For i = 0 just set values
    int i = 0;
    DynamicArray<uint16_t> best(samples.samples.get() + bests[i] * _n, _n);
    int worst_index = bests[i] % 2 == 0 ? bests[i] + 1 : bests[i] - 1;
    DynamicArray<uint16_t> worst(samples.samples.get() + worst_index * _n, _n);

    for (int k = 0; k < _n; k++) {
        double diff = 0xffff * (best[k] - worst[k]) / (_k * x);
        Offsets[k] = diff;
    }

    for (i=1; i < x; i++) {
        DynamicArray<uint16_t> best(samples.samples.get() + bests[i] * _n, _n);
        worst_index = bests[i] % 2 == 0 ? bests[i] + 1 : bests[i] - 1;
        DynamicArray<uint16_t> worst(samples.samples.get() + worst_index * _n, _n);

        for (int k = 0; k < _n; k++) {
            double diff = 0xffff * (best[k] - worst[k]) / (_k * x);
            Offsets[k] = diff + Offsets[k];
        }
    }

    for (int k = 0; k < _n; k++) {
        probability_distribution[k] = std::round(std::max((0xffff * _lower), std::min((0xffff * _upper), probability_distribution[k] + Offsets[k])));
    }

}

void CGA::_update_probability_distribution(DynamicArray<uint16_t>& best_sample, DynamicArray<uint16_t>& worst_sample, double weight) {
    for (int k = 0; k < _n; k++) {
        double diff = 0xffff * (best_sample[k] - worst_sample[k]) / (_k * weight);
        probability_distribution[k] = std::round(std::max((0xffff * _lower), std::min((0xffff * _upper), diff + probability_distribution[k])));
    }
}

void CGA::update_probability_distribution(SampleContainer& samples)
{
    _update_probability_distribution_with_best(samples);
}

void CGA::force_update_probability_distribution(DynamicArray<uint16_t>& samples) {
    std::logic_error("Not implemented");
}

void CGA::force_update_probability_distribution(SampleContainer& samples) {
    _update_probability_distribution_with_x_best(samples, INT32_MAX);
}

