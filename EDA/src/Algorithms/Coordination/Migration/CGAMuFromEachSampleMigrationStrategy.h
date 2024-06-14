#pragma once
#include "../../Algorithms/EDABase.h"
#include "MigrationStrategy.h"
#include "../Communocation/DynamicArray.h"
#include "../Topology/Operation.h"
#include <vector>
#include <mpi.h>
#include "../Communocation/MPIInfo.h"
#include "../Communocation/MPIWrapper.h"

/**
 * @brief Migration strategy that will sample the x best samples from EDA and force apply all recieved samples
 */
class CGAMuFromEachSampleMigrationStrategy : public MigrationStrategy {
public:

	/**
	 * @brief Construct a new Mu From Each Sample Migration Strategy object
	 *
	 * @param migration_interval Frequency of migration
	 * @param select_count How many samples to select from EDA (Myst be smaller than Lambda)
	 */
	CGAMuFromEachSampleMigrationStrategy(int migration_interval) : MigrationStrategy(migration_interval) {
	}


	virtual void do_migration(EDABase& optimizer, Topology& topology) {
		optimizer.sample();
		std::vector<Operation> opr = topology.get_ordered_operations(MPIInfo::Rank, MPIInfo::Size);
		SampleContainer recieved_data = MPIWrapper::do_communicate_sample_container(optimizer.sample_container, opr, MPI_UNSIGNED_SHORT);
		recieved_data.concat(optimizer.sample_container);
		optimizer.force_update_probability_distribution(recieved_data);

	}
};