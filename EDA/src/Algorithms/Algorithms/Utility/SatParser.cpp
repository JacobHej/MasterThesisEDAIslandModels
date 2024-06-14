#include "SatParser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// find problems at https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html

FitnessKSAT SatParser::parse_maxsat_instance(std::string filename, int& out_clause_count, int& out_literal_count) {
	std::ifstream file(filename);

    int literal;
	std::string line;
    std::string normal_form;
    std::string p;
    Clauses clauses;

    while (std::getline(file, line)) {

        std::istringstream iss(line);

        if (line.empty() || line[0] == 'c' || line[0] == '%' || line[0] == '0') {
            continue;
        } 
        else if (line[0] == 'p') {
            iss >> p >> normal_form >> out_literal_count >> out_clause_count;
            if (normal_form != "cnf") throw std::logic_error("not cnf");
        } else {

            Clause clause;

            while (iss >> literal) {

                if (literal == 0) break;

                if (literal < 0) {
                    clause.second.push_back(1); // add as negated
                    literal *= -1; // flip sign
                } else clause.second.push_back(0);

                clause.first.push_back(literal - 1); 
                // add literal. Note dimacs format does not use 0 as literal but for 
                // indexing purposes we do so subtract one from each literal eg. 1 -> 0 (at index 0) ... 20 -> 19 (at index 19)
            }

            clauses.push_back(clause); // add clause
        }
    }

    file.close();

    return FitnessKSAT(clauses);
}