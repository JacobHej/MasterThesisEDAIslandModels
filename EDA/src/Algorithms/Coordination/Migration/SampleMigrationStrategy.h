#pragma once

#include "../../Algorithms/EDABase.h"
#include "MigrationStrategy.h"
#include "../Topology/Operation.h"
#include <vector>
#include <mpi.h>

/**
 * @brief Migration strategy for sampling new full set of samples 
 * and apply the recieved samples along with existing samples using
 * the EDA's default update probability method
 */
class SampleMigrationStrategy : public MigrationStrategy{
public:

	/**
	 * @brief Construct a new Sample Migration Strategy object
	 * 
	 * @param migration_interval frequenzy of migrations
	 */
	SampleMigrationStrategy(int migration_interval) : MigrationStrategy(migration_interval) {}

	virtual void do_migration(EDABase& optimizer, Topology& topology) {
		optimizer.sample();
		std::vector<Operation> opr = topology.get_ordered_operations(MPIInfo::Rank, MPIInfo::Size);
		SampleContainer recieved_data = MPIWrapper::do_communicate_sample_container(optimizer.sample_container, opr, MPI_UNSIGNED_SHORT);

		recieved_data.concat(optimizer.sample_container);

		optimizer.update_probability_distribution(recieved_data);
	}
};