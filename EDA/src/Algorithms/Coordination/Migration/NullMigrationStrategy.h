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

class NullMigrationStrategy : public MigrationStrategy {
public:
	/**
	 * @brief Construct a new Migration Strategy object
	 *
	 * @param migration_interval frequenzy of migrations
	 */
	NullMigrationStrategy(): MigrationStrategy(1) {
		
	}

	virtual void do_migration(EDABase& optimizer, Topology& topology) {
	}
};

