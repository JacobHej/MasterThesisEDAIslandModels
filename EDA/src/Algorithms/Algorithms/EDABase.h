#pragma once
#include <vector>
#include "DataObjects/iteration_snapshot.h"
#include <memory>
#include "Utility/Random.h"
#include "../Fitness/fitness_functions.h"
#include <functional>
#include "../Coordination/Communocation/DynamicArray.h"
#include "../Coordination/Communocation/SampleContainer.h"

class EDABase {
public: 
    std::vector<iteration_snapshot> history;

    /**
     * @brief Buffer array for generated random numbers
     */
    DynamicArray<uint16_t> _rng_buffer;
    DynamicArray<uint16_t> probability_distribution;
    SampleContainer sample_container;

    /**
     * @brief Current amount of run iterations
     */
    int _itr_count;

    /**
     * @brief Current amount of fitness evaluations
     */
    int _fe_count;

    /**
     * @brief Best fitness found so far (exclusively through sampling)
     */
    int best_fitness;

    /**
     * @brief Iteration at which the best fitness so far was found
     */
    int best_fitness_itr;

    /**
     * @brief Construct a new EDABase (default constructor)
     */
    EDABase(int lambda, int n, FitnessClass& fitness_class);

    /**
     * @brief Destroy the EDABase (default destructor)
     * 
     */
    ~EDABase() = default;

    /**
     * @brief Will run select amount of iterations.
     * 
     * @param iterations 
     */
    virtual void run_iterations(int iterations) = 0;

    /**
     * @brief Will run select amount of iterations checking one each iteration 
     * if the stopping criteria is met and stop if so
     * 
     * @param iterations Amount of iterations in total to run
     * @param stopping_criteria Function to evaluate the state of the EDA and returns true if it should stop
     */
    virtual void run_iterations(int iterations, std::function<bool(const EDABase&)> stopping_criteria) = 0;
    
    /**
     * @brief Will based on the provided samples select the mu best and update 
     * the probability distribution
     * 
     * @param samples Set of samples to use for update in a flat 2D 
     * array (each sample must have length n, as provided in the constructor)
     * @param samples_size 
     */
    virtual void update_probability_distribution(SampleContainer& samples) = 0;
    
    /**
     * @brief Will use all provided samples to update the probability distribution
     * without any sorting or selection
     * 
     * @param samples Set of samples to use for update in a flat 2D 
     * array (each sample must have length n, as provided in the constructor)
     * @param samples_size length of samples
     */ 
    virtual void force_update_probability_distribution(DynamicArray<uint16_t>& samples) = 0;
    virtual void force_update_probability_distribution(SampleContainer& samples) = 0;
    /** 
     * @brief Will combine the probability distributions (also using current probability distribution) 
     * and update current probability distribution. 
     * 
     * @param probability_distributions 
     */
    virtual void combine_probability_distributions(DynamicArray<uint16_t>& probability_distributions, double beta) = 0;

    /**
     * @brief Get the x best samples
     * 
     * @param sample_size size of samples
     * @param samples samples to be selected from
     * @param x amount of best samples that will be returned (must be shorted than sample size)
     * @param out_size Return parameter representing the size of the select samples
     * @return std::unique_ptr<uint16_t[]> x best samples
     */
    virtual DynamicArray<uint16_t> get_x_best(SampleContainer& samples, int x);

    /**
     * @brief Update the samples by sampling new ones
     */
    void sample();

protected:
    /**
     * @brief Class handling the fitness evaluation
     */
    FitnessClass* _fitnessclass;

    /**
     * @brief RNG class
     */
    Random _random;

    /**
     * @brief Amount of samples to be generated each iteration
     */
    int _lambda;

    /**
     * @brief Bit length
     */
    int _n;


    /**
     * @brief Lower bound for the probability probability distribution
     */
    double _lower;

    /**
     * @brief Upper bound for the probability distribution
     */
    double _upper;

    virtual void log_state(SampleContainer& sample_container, DynamicArray<uint16_t>& indices);

    virtual DynamicArray<uint16_t> index_sort(SampleContainer& samples);
};

