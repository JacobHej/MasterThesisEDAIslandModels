#pragma once

enum OperationType {
	send,
	recieve
};

struct Operation {
public:
	OperationType operation_type;
	int target;

	Operation(OperationType operation_type, int target);
	~Operation() = default;
};


