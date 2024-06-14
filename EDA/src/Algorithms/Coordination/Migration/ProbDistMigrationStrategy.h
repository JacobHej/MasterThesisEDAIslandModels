#pragma once
#include "../../Algorithms/EDABase.h"
#include "MigrationStrategy.h"
#include "../Topology/Operation.h"
#include <vector>
#include <mpi.h>
#include "../Communocation/MPIWrapper.h"
#include "../Topology/Topology.h"

/**
 * @brief Naive migration of probability distributions
 * NOTE: could add wheighted version
 */
class ProbDistMigrationStrategy : public MigrationStrategy{
public:
	ProbDistMigrationStrategy(double beta, int migration_interval) : MigrationStrategy(migration_interval) {
		_beta = beta;
	}

	virtual void do_migration(EDABase& optimizer, Topology& topology) {
		std::vector<Operation> opr = topology.get_ordered_operations(MPIInfo::Rank, MPIInfo::Size);
		DynamicArray<uint16_t> recieved_data = MPIWrapper::do_communicate_dynamic_array(optimizer.probability_distribution, opr, MPI_UNSIGNED_SHORT);
		optimizer.combine_probability_distributions(recieved_data, _beta);
	}

private:
	double _beta;
};