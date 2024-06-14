#pragma once
#include "Migration/MigrationStrategy.h"
#include "Topology/Topology.h"
#include "Communocation/MPIWrapper.h"
#include "Migration/MigrationStrategy.h"
#include "../Algorithms/EDABase.h"
#include "Communocation/MPIInfo.h"
#include <iostream>
#include <functional>

/**
 * @brief Class managing the running and coordination of multiple EDA's
 * 
 * @tparam TSerializedValue 
 * @tparam TDeserialized 
 */
class OptimizerCoordinator {
public: 

	/**
	 * @brief Will run EDA and handle communication and migration according
	 * to the provided settings
	 * 
	 * @param iterations Amount of iterations to run
	 * @param mpi_wrapper Wrapper to be used for communication
	 * @param migration_strategy Which migration strategy to be used
	 * @param optimizer EDA to use
	 * @param topology Which topology to use for communication
	 * @param stopping_criteria Criteria for Halting
	 * @return std::vector<iteration_snapshot> 
	 */
	std::vector<iteration_snapshot> run(
		int& iterations, 
		MigrationStrategy& migration_strategy, 
		EDABase& optimizer, 
		Topology& topology,
		std::function<bool(const EDABase&)> stopping_criteria) {

		int migration_iterations = iterations / migration_strategy.migation_interval;
		int remaining_iterations = iterations - (migration_iterations * migration_strategy.migation_interval);

		for (int i = 0; i < migration_iterations; i++) {
			if (MPIWrapper::should_halt(stopping_criteria(optimizer))) {
				return optimizer.history;
			}
			optimizer.run_iterations(migration_strategy.migation_interval);
			migration_strategy.do_migration(optimizer, topology);
		}

		optimizer.run_iterations(remaining_iterations);
		return optimizer.history;
	}
};

