#pragma once

#include <vector>
#include <algorithm>
#include <memory>
#include "../DataObjects/iteration_snapshot.h"
#include "../Utility/Random.h"
#include "../../Fitness/fitness_functions.h"
#include "../EDABase.h"

class CGA : public EDABase {
public:
    /**
     * @brief Construct a new umda object
     *
     * @param my Amount of samples each iteration that will be used to update
     * probability distribution
     * @param lambda Amount of samples sampled each iteration
     * @param n Biit count
     * @param fitnessclass Class that handles fitness evaluations
     */
    CGA(int n, double k, FitnessClass& fitnessclass);

    /**
     * @brief Will run select amount of iterations checking one each iteration
     * if the stopping criteria is met and stop if so
     *
     * @param iterations Amount of iterations in total to run
     * @param stopping_criteria Function to evaluate the state of the EDA and returns true if it should stop
     */
    void run_iterations(int iterations, std::function<bool(const EDABase&)> stopping_criteria);

    /**
     * @brief Will run select amount of iterations.
     *
     * @param iterations
     */
    void run_iterations(int iterations);

    /**
     * @brief Will based on the provided samples select the mu best and update
     * the probability distribution
     *
     * @param samples Set of samples to use for update in a flat 2D
     * array (each sample must have length n, as provided in the constructor)
     * @param samples_size length of samples
     */
    void update_probability_distribution(SampleContainer& samples);

    /**
     * @brief Will use all provided samples to update the probability distribution
     * without any sorting or selection
     *
     * @param samples Set of samples to use for update in a flat 2D
     * array (each sample must have length n, as provided in the constructor)
     * @param samples_size length of samples
     */
    void force_update_probability_distribution(DynamicArray<uint16_t>& samples);

    void force_update_probability_distribution(SampleContainer& samples);

    /**
     * @brief Will combine the probability distributions (also using current probability distribution)
     * and update current probability distribution.
     *
     * @param probability_distributions_size
     * @param probability_distributions
     */
    void combine_probability_distributions(DynamicArray<uint16_t>& probability_distributions, double beta);

private:
    /**
     * @brief Amount of samples that will be selected to update the
     * probability distribution     
     */
    double _mu;

    /**
    * Population size
    */
    double _k;

    void _update_probability_distribution(DynamicArray<uint16_t>& best_sample, DynamicArray<uint16_t>& worst_sample, double weight);
    void _update_probability_distribution_with_best(SampleContainer& samples);
    void _update_probability_distribution_with_x_best(SampleContainer& samples, int x);
};

