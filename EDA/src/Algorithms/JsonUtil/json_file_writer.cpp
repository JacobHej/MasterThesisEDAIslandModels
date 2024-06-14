#include "json_file_writer.h"
#include "json_builder.h"

#include <fstream>
#include <filesystem>

void json_file_writer::write_history(
	std::vector<std::pair<std::string, std::string>> &details,
	std::string &best_fit,
	std::string& best_fit_itr,
	std::vector<iteration_snapshot>& history, 
	std::string& file_path) {

	json_builder json = json_builder();
	json.start_array("Details");
	for (int i = 0; i < details.size(); i++) {
		std::pair<std::string, std::string> x = details[i];
		json.start_object();
		json.write_property(std::string("VariableName"), x.first, true);
		json.write_last_property(std::string("VariableValue"), x.second, true);
		if (i< details.size()-1) {
			json.end_object();
		}
		else {
			json.end_last_object();
		}
	}
	json.end_array();
	json.write_property(std::string("BestFit"), best_fit, false);
	json.write_property(std::string("BestFitItr"), best_fit_itr, false);

	json.start_array("IterationSnapshots");

	for (int i = 0; i < history.size(); i++) {
		json.start_object();
		json.write_property(std::string("LowerBoundHits"), std::to_string(history[i].lower_bound_hits), false);
		json.write_property(std::string("UpperBoundHits"), std::to_string(history[i].upper_bound_hits), false);
		json.write_property(std::string("BestFitness"), std::to_string(history[i].best_fitness), false);
		json.write_property(std::string("WorstFitness"), std::to_string(history[i].worst_fitness), false);
		json.write_property(std::string("MedianFitness"), std::to_string(history[i].median_fitness), false);
		json.write_property(std::string("Iteration"), std::to_string(history[i].iteration), false);
		json.write_last_property(std::string("FitnessEvals"), std::to_string(history[i].fitness_evals), false);
		if (i <history.size() - 1) {
			json.end_object();
		}
		else {
			json.end_last_object();
		}
	}

	json.end_last_array();

	std::string json_string = json.finish();
	
	std::ofstream myfile;
	myfile.open(file_path);
	myfile << json_string;
	myfile.close();
}
