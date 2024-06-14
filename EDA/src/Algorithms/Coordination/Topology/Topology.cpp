#include "Topology.h"
#include <stdexcept>

std::vector<Operation> Topology::get_ordered_operations(int rank, int size) {
	throw std::logic_error("Function not yet implemented");
}

std::vector<Operation> EmptyTopology::get_ordered_operations(int rank, int size) {
	std::vector<Operation> operations;
	return operations;
}

std::vector<Operation> StarTopology::get_ordered_operations(int rank, int size) {
	std::vector<Operation> operations;
	if (rank == 0) {
		for (int i = 1; i < size; i++) {
			Operation o(OperationType::recieve, i);
			operations.push_back(o);
		}
	}
	else {
		Operation o(OperationType::send, 0);
		operations.push_back(o);
	}
	return operations;
}

std::vector<Operation> OneWayCircleTopology::get_ordered_operations(int rank, int size) {
	std::vector<Operation> operations;
	int front = (rank + 1) % size;
	int back = (size + rank - 1) % size;

	if (rank % 2 == 0) {
		Operation send(OperationType::send, front);
		operations.push_back(send);
		Operation recieve(OperationType::recieve, back);
		operations.push_back(recieve);
	}
	else {
		Operation recieve(OperationType::recieve, back);
		operations.push_back(recieve);
		Operation send(OperationType::send, front);
		operations.push_back(send);
	}
	return operations;
}

std::vector<Operation> CompleteTopology::get_ordered_operations(int rank, int size) {
	std::vector<Operation> operations;
	for (int i = 0; i < size; i++) {
		if (rank == i) {
			for (int j = 0; j < size; j++) {
				if (j!=rank) {
					Operation o(OperationType::recieve, j);
					operations.push_back(o);
				}
			}
		}
		else {
			Operation o(OperationType::send, i);
			operations.push_back(o);
		}
	}
	return operations;
}

KCircleTopology::KCircleTopology(int k) {
	this->k = k;
}

std::vector<Operation> KCircleTopology::get_ordered_operations(int rank, int size) {
	std::vector<Operation> operations;
	for (int i = 1; i <= k; i++) {
		int front = (rank + i) % size;
		int back = (size + rank - i) % size;

		if ((rank/i) % 2 == 0) {
			Operation send1(OperationType::send, front);
			Operation recieve2(OperationType::recieve, back);
			Operation send3(OperationType::send, back);
			Operation recieve4(OperationType::recieve, front);
			operations.push_back(send1);
			operations.push_back(recieve2);
			operations.push_back(send3);
			operations.push_back(recieve4);
		}
		else {
			Operation recieve1(OperationType::recieve, back);
			Operation send2(OperationType::send, front);
			Operation recieve3(OperationType::recieve, front);
			Operation send4(OperationType::send, back);
			operations.push_back(recieve1);
			operations.push_back(send2);
			operations.push_back(recieve3);
			operations.push_back(send4);
		}
	}
	return operations;
}

TaurusTopology::TaurusTopology(int h, int w) {
	this->h = h;
	this->w = w;
}

std::vector<Operation> TaurusTopology::get_ordered_operations(int rank, int size) {
	std::vector<Operation> operations;
	int row = rank / w;
	int coloumn = rank % w;
	int front = ((coloumn + 1) % w) + row * w;
	int back = ((coloumn + w - 1) % w) + row * w;
	int up = ((rank + w) % (h*w));
	int down = ((rank + h*w - w) % (h*w));

	if (coloumn % 2 == 0) {
		Operation send1(OperationType::send, front);
		Operation recieve2(OperationType::recieve, back);
		Operation send3(OperationType::send, back);
		Operation recieve4(OperationType::recieve, front);
		operations.push_back(send1);
		operations.push_back(recieve2);
		operations.push_back(send3);
		operations.push_back(recieve4);
	}
	else {
		Operation recieve1(OperationType::recieve, back);
		Operation send2(OperationType::send, front);
		Operation recieve3(OperationType::recieve, front);
		Operation send4(OperationType::send, back);
		operations.push_back(recieve1);
		operations.push_back(send2);
		operations.push_back(recieve3);
		operations.push_back(send4);
	}

	if (row % 2 == 0) {
		Operation send1(OperationType::send, up);
		Operation recieve2(OperationType::recieve, down);
		Operation send3(OperationType::send, down);
		Operation recieve4(OperationType::recieve, up);
		operations.push_back(send1);
		operations.push_back(recieve2);
		operations.push_back(send3);
		operations.push_back(recieve4);
	}
	else {
		Operation recieve1(OperationType::recieve, down);
		Operation send2(OperationType::send, up);
		Operation recieve3(OperationType::recieve, up);
		Operation send4(OperationType::send, down);
		operations.push_back(recieve1);
		operations.push_back(send2);
		operations.push_back(recieve3);
		operations.push_back(send4);
	}
	return operations;
}