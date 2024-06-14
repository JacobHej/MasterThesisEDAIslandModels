#include "Operation.h"

Operation::Operation(OperationType operation_type, int target) {
	this->operation_type = operation_type;
	this->target = target;
}
