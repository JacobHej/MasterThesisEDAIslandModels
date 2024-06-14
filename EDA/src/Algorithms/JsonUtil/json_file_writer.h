#pragma once
#include <vector>
#include <string>
#include "../Algorithms/DataObjects/iteration_snapshot.h"
class json_file_writer {
	public:
		static void write_history(
			std::vector<std::pair<std::string, std::string>>& details, 
			std::string &best_fit, 
			std::string &best_fit_itr,
			std::vector<iteration_snapshot>& history, 
			std::string& file_path);
};

