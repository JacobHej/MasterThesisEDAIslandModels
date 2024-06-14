#pragma once
#include "../../AlgorithmsIncludes.h"

std::string cga_make_filename(
	std::vector<std::pair<std::string, std::string>> details,
	std::string foldername
) {
	std::stringstream filepath;
	filepath << foldername << "/Test";
	for (std::pair<std::string, std::string> x : details) {
		filepath << "_" << x.first << "_" << x.second;
	}
	filepath << ".txt";
	return filepath.str();
}

std::vector<std::pair<std::string, std::string>> cga_make_details(
	int n, int run, int cK, int mi,
	std::string optimizer,
	std::string fitnessFunction,
	std::string topology,
	std::string KType,
	std::string ms
) {
	std::vector<std::pair<std::string, std::string>> details;
	std::pair<std::string, std::string> optimizer_variable("optimizer", optimizer);
	details.push_back(optimizer_variable);
	std::pair<std::string, std::string> fitness_function("fitnessFunction", fitnessFunction);
	details.push_back(fitness_function);
	std::pair<std::string, std::string> topology_variable("topology", topology);
	details.push_back(topology_variable);
	std::pair<std::string, std::string> n_variable("n", std::to_string(n));
	details.push_back(n_variable);
	std::pair<std::string, std::string> run_variable("run", std::to_string(run));
	details.push_back(run_variable);
	std::pair<std::string, std::string> rank_variable("rank", std::to_string(MPIInfo::Rank));
	details.push_back(rank_variable);
	std::pair<std::string, std::string> cluster_size("cluster_size", std::to_string(MPIInfo::Size));
	details.push_back(cluster_size);
	std::pair<std::string, std::string> k_constant("cK", std::to_string(cK));
	details.push_back(k_constant);
	std::pair<std::string, std::string> K_type("kType", KType);
	details.push_back(K_type);
	std::pair<std::string, std::string> mi_variable("mi", std::to_string(mi));
	details.push_back(mi_variable);
	std::pair<std::string, std::string> ms_variable("ms", ms);
	details.push_back(ms_variable);
	return details;
}

std::vector<std::pair<std::string, std::string>> cga_make_details_seriel(
	int n, int run, int cK,
	std::string optimizer,
	std::string fitnessFunction,
	std::string KType
) {
	std::vector<std::pair<std::string, std::string>> details;
	std::pair<std::string, std::string> optimizer_variable("optimizer", optimizer);
	details.push_back(optimizer_variable);
	std::pair<std::string, std::string> fitness_function("fitnessFunction", fitnessFunction);
	details.push_back(fitness_function);
	std::pair<std::string, std::string> n_variable("n", std::to_string(n));
	details.push_back(n_variable);
	std::pair<std::string, std::string> run_variable("run", std::to_string(run * MPIInfo::Size + MPIInfo::Rank));
	details.push_back(run_variable);
	std::pair<std::string, std::string> k_constant("cK", std::to_string(cK));
	details.push_back(k_constant);
	std::pair<std::string, std::string> K_type("kType", KType);
	details.push_back(K_type);
	return details;
}

void cga_run_single(
	int n, int k, int run, int best_fitness,
	FitnessClass& fitnessclass,
	std::vector<std::pair<std::string, std::string>> details,
	std::string filepath,
	int step_count,
	int maxItr) {

	CGA optimizer(n, k, fitnessclass);
	optimizer.run_iterations(maxItr, [best_fitness](const EDABase& eda) -> bool { return eda.best_fitness == best_fitness; });

	std::string _best_fitness = std::to_string(optimizer.best_fitness);
	std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);

	DynamicArray<int> dummy(0);
	auto truncated_history = DataTruncater::truncate_history(optimizer.history, optimizer.history.size() / step_count, optimizer.best_fitness_itr, dummy);
	json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);

	std::cout << " N: " << n << " K: " << k << " RUN: " << run << std::endl;
}

void cga_run_multi(
	int n, int k, int run, int best_fitness,
	FitnessClass& fitnessclass,
	std::vector<std::pair<std::string, std::string>> details,
	std::string filepath,
	Topology& topology,
	MigrationStrategy& ms,
	int step_count,
	int maxItr
) {
	CGA optimizer(n, k, fitnessclass);
	OptimizerCoordinator oc{};
	oc.run(maxItr, ms, optimizer, topology, [best_fitness](const EDABase& eda) -> bool { return eda.best_fitness == best_fitness; });

	std::string _best_fitness = std::to_string(optimizer.best_fitness);
	std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);

	auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, optimizer.history.size() / step_count, optimizer.best_fitness_itr);
	json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);
}

std::string cga_nameCleaner(std::string s) {
	std::string prefix("class ");

	auto res = std::mismatch(prefix.begin(), prefix.end(), s.begin());
	if (res.first == prefix.end())
	{
		s = s.substr(prefix.length());
	}

	while (std::isdigit(s[0])) {
		s = s.substr(1);
	}
	return s;
}
