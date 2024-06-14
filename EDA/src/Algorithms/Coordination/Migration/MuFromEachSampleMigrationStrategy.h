#pragma once
#include "../../Algorithms/EDABase.h"
#include "MigrationStrategy.h"
#include "../Communocation/DynamicArray.h"
#include "../Topology/Operation.h"
#include <vector>
#include <mpi.h>

/**
 * @brief Migration strategy that will sample the x best samples from EDA and force apply all recieved samples
 */
class MuFromEachSampleMigrationStrategy : public MigrationStrategy{
public:

	/**
	 * @brief Construct a new Mu From Each Sample Migration Strategy object
	 * 
	 * @param migration_interval Frequency of migration
	 * @param select_count How many samples to select from EDA (Myst be smaller than Lambda)
	 */
	MuFromEachSampleMigrationStrategy(int migration_interval, int select_count) : MigrationStrategy(migration_interval) {
		_select_count = select_count;
	}


	virtual void do_migration(EDABase& optimizer, Topology& topology) {

		optimizer.sample();

		DynamicArray<uint16_t> _last_x_best = optimizer.get_x_best(optimizer.sample_container, _select_count);
		
		std::vector<Operation> opr = topology.get_ordered_operations(MPIInfo::Rank, MPIInfo::Size);
		DynamicArray<uint16_t> recieved_data = MPIWrapper::do_communicate_dynamic_array(_last_x_best, opr, MPI_UNSIGNED_SHORT);


		_last_x_best.concat(recieved_data);

		optimizer.force_update_probability_distribution(_last_x_best);


	}

private:
	/**
	 * @brief Amount of samples to select from EDA
	 */
	int _select_count;
};