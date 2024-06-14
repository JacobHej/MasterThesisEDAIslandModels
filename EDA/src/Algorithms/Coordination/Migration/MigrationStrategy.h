#pragma once
#include "../../Algorithms/EDABase.h"
#include "../Topology/Topology.h"
#include <vector>
#include <memory>
#include <mpi.h>

/**
 * @brief Base class for migration strategies
 * 
 * @tparam TSerializedValue Base type of serialized value
 * @tparam TDeserialized Type of data when deserialized
 */

class MigrationStrategy {
public:
	int migation_interval;

	/**
	 * @brief Construct a new Migration Strategy object
	 * 
	 * @param migration_interval frequenzy of migrations
	 */
	MigrationStrategy(int migration_interval) {
		this->migation_interval = migration_interval;
	}
	
	virtual void do_migration(EDABase& optimizer, Topology& topology) {
		throw std::logic_error("Function not yet implemented");
	}
};

