#pragma once
#include "../../Fitness/fitness_functions.h"
#include "../../Algorithms/cGA/CGA.h"


void preliminary_test() {
	std::cout << "Start test" << std::endl;
	FitnessOneMax fit;
	int n = 1000;

	std::vector<double> bestFE;
	std::vector<double> bestITR;

	for (int c = -11; c <= 11; c++) {
		double k = std::log(n) * std::pow(1.5, c);

		for (int run = 0; run < 20; run++) {
			CGA optimizer(k, n, fit);
			optimizer.run_iterations(10000, [n](const EDABase& eda) -> bool { return eda.best_fitness == n; });
			if (optimizer.best_fitness < n) {
				std::cout << "Did not optimize" << std::endl;
			}
			else {
				bestITR.push_back(optimizer.best_fitness_itr);
				bestFE.push_back(optimizer.history[optimizer.best_fitness_itr].fitness_evals);
			}
		}

		double averageITR = std::accumulate(bestITR.begin(), bestITR.end(), 0.0) / bestITR.size();
		double averageFE = std::accumulate(bestFE.begin(), bestFE.end(), 0.0) / bestFE.size();
		std::cout << "c: " << c << " FE AVG: " << averageFE << " IR AVG: " << averageITR << std::endl;
	}	
}