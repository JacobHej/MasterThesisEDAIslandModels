#pragma once
#include <cstdint>
#include "../../Coordination/Communocation/DynamicArray.h"
#include <vector>
/**
 * @brief Class containing relevant metrics representing a snapshot of an iteration
 * 
 */
struct iteration_snapshot {
	public:
		uint16_t best_fitness;
		uint16_t worst_fitness;
		uint16_t median_fitness;
		uint32_t iteration;
		uint32_t fitness_evals;
		int lower_bound_hits;
		int upper_bound_hits;
		/**
		 * @brief Construct a new iteration snapshot object
		 * 
		 * @param bf Best fitness
		 * @param wf Worst fitness
		 * @param mf Median fitness
		 * @param i Current iteration
		 * @param fe Current fitness evaluation count
		 */
		iteration_snapshot(uint16_t bf, uint16_t wf, uint16_t mf, uint32_t i, uint32_t fe);
		iteration_snapshot(uint16_t bf, uint16_t wf, uint16_t mf, uint32_t i, uint32_t fe, int lbh, int ubh);
		iteration_snapshot(const iteration_snapshot& ref);
		iteration_snapshot() = default;
};

