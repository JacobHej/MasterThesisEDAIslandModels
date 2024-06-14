#include "DataTruncater.h"
#include "../../Coordination/Communocation/MPIWrapper.h"

#include <algorithm>
#include <iostream>

std::vector<iteration_snapshot> DataTruncater::distributed_truncate_history(const std::vector<iteration_snapshot>& history, int iterations, int best_fit_itr) {

	DynamicArray<int> best_fit_itrs = MPIWrapper::do_communicate_bestFitFe(best_fit_itr);

	return truncate_history(history, iterations, best_fit_itr, best_fit_itrs);
	
}


std::vector<iteration_snapshot> DataTruncater::truncate_history(const std::vector<iteration_snapshot>& history, int iterations, int best_fit_itr, DynamicArray<int>& best_fit_itrs) {
	
	iterations = std::max(iterations, 1);

	std::vector<iteration_snapshot> truncated_history;

	// add select iterations
	for (int i = 0; i < history.size(); i += iterations) {
		truncated_history.push_back(history[i]);
	}

	// ensure that we add the last iterations
	truncated_history.push_back(history[history.size() - 1]);

	// add all best fit itr
	for (int i = 0; i < best_fit_itrs.size(); i++) {
		if (best_fit_itrs[i] < history.size()) {
			truncated_history.push_back(history[best_fit_itrs[i]]);
		}
	}

	// add own best fit itr
	truncated_history.push_back(history[best_fit_itr]);

	// sort by iteration
	std::sort(truncated_history.begin(), truncated_history.end(),
		[&](const iteration_snapshot& a, const iteration_snapshot& b) {
			return a.iteration < b.iteration;
		});

	// remove duplicate iterations
	auto ip = std::unique(truncated_history.begin(), truncated_history.end(),
		[&](const iteration_snapshot& a, const iteration_snapshot& b) {
			return a.iteration == b.iteration;
		});

	auto x = std::distance(truncated_history.begin(), ip);
	truncated_history.resize(x);

	return truncated_history;
}