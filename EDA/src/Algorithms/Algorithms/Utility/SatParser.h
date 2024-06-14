#pragma once
#include "../../Fitness/fitness_functions.h"

class SatParser {
	using Clause = std::pair<std::vector<uint16_t>, std::vector<uint16_t>>;
	using Clauses = std::vector<Clause>;

public:
	static FitnessKSAT parse_maxsat_instance(std::string filename, int& out_clause_count, int& out_literal_count);
};

