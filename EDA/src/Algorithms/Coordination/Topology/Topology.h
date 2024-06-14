#pragma once
#include <vector>
#include "Operation.h"
class Topology {
public:
	virtual std::vector<Operation> get_ordered_operations(int rank, int size);
};

class EmptyTopology : public Topology {
public:
	std::vector<Operation> get_ordered_operations(int rank, int size) override;
};

class StarTopology : public Topology {
public:
	std::vector<Operation> get_ordered_operations(int rank, int size) override;
};

class OneWayCircleTopology : public Topology {
public:
	std::vector<Operation> get_ordered_operations(int rank, int size) override;
};

class CompleteTopology : public Topology {
public:
	std::vector<Operation> get_ordered_operations(int rank, int size) override;
};

class KCircleTopology : public Topology {
private:
	int k;
public:
	KCircleTopology(int k);
	std::vector<Operation> get_ordered_operations(int rank, int size) override;
};

class TaurusTopology : public Topology {
private:
	int h;
	int w;
public:
	TaurusTopology(int h, int w);
	std::vector<Operation> get_ordered_operations(int rank, int size) override;
};

