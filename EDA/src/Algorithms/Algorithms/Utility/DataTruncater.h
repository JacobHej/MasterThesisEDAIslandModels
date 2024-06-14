#pragma once
#include <vector>
#include "../DataObjects/iteration_snapshot.h"
#include "../../Coordination/Communocation/DynamicArray.h"
class DataTruncater {
public:
	/// <summary>
	/// Selects every "iterations" iteration snapshot and adds other iterations such tha the following holds ->
	/// Will ensure that first, last and all best fit itr from cluster (including own) is present if within the bounds if the history snapshot
	/// </summary>
	/// <param name="history"></param>
	/// <param name="iterations"></param>
	/// <param name="best_fit_itr"></param>
	/// <returns></returns>
	static std::vector<iteration_snapshot> distributed_truncate_history(const std::vector<iteration_snapshot>& history, int iterations, int best_fit_itr);
	static std::vector<iteration_snapshot> truncate_history(const std::vector<iteration_snapshot>& history, int iterations, int best_fit_itr, DynamicArray<int>& best_fit_itrs);
};

