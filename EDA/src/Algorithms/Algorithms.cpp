#include <algorithm>
#include <numeric>
#include <vector>
#include <iostream>
#include "Algorithms/UMDA/umda.h"
#include <fstream>
#include <mpi.h>
#include <string>
#include <sstream>
#include "JsonUtil/json_builder.h"
#include "JsonUtil/json_file_writer.h"
#include "Coordination/OptimizerCoordinator.h"
#include "Coordination/Migration/MigrationStrategy.h"
#include "Coordination/Migration/NullMigrationStrategy.h"
#include "Coordination/Migration/SampleMigrationStrategy.h"
#include "Coordination/Migration/MuFromEachSampleMigrationStrategy.h"
#include "Coordination/Migration/ProbDistMigrationStrategy.h"
#include "Coordination/Communocation/MPIWrapper.h"
#include <filesystem>
#include "Algorithms/Utility/SatParser.h"
#include "Algorithms/Utility/DataTruncater.h"
#include <map>
#include "Experiments/CGA/CGATwoPeaks.h"
#include "Experiments/CGA/CGAJumpK.h"
#include "AlgorithmsInfo.h"
#include "Experiments/CGA/CGAMaxSat.h"




const double muLambdaRatio = 3.0 * std::exp(1);

void print_history(const EDABase& eda) {
	for (auto sn : eda.history) {
		std::cout << "Best: " << sn.best_fitness << std::endl;
		std::cout << "Medi: " << sn.median_fitness << std::endl;
		std::cout << "Wors: " << sn.worst_fitness << std::endl;
		std::cout << "Itra: " << sn.iteration << std::endl;
		std::cout << "FEva: " << sn.fitness_evals << std::endl << std::endl;
	}
}

std::string make_filename(
	std::vector<std::pair<std::string, std::string>> details,
	std::string foldername
) {
	std::stringstream filepath;
	filepath << foldername << "/Test" ;
	for (std::pair<std::string, std::string> x : details) {
		filepath << "_" << x.first << "_" << x.second;
	}
	filepath << ".txt";
	return filepath.str();
}

std::vector<std::pair<std::string, std::string>> make_details(
	int n, int run, int lambda, int mu, int mi,
	std::string optimizer,
	std::string fitnessFunction,
	std::string topology,
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
	std::pair<std::string, std::string> lambda_constant("lambda", std::to_string(lambda));
	details.push_back(lambda_constant);
	std::pair<std::string, std::string> mu_constant("mu", std::to_string(mu));
	details.push_back(mu_constant);
	std::pair<std::string, std::string> mi_variable("mi", std::to_string(mi));
	details.push_back(mi_variable);
	std::pair<std::string, std::string> ms_variable("ms", ms);
	details.push_back(ms_variable);
	return details;
}

std::vector<std::pair<std::string, std::string>> make_details_seriel(
	int n, int run, int lambda, int mu,
	std::string optimizer,
	std::string fitnessFunction
) {
	std::vector<std::pair<std::string, std::string>> details;
	std::pair<std::string, std::string> optimizer_variable("optimizer", optimizer);
	details.push_back(optimizer_variable);
	std::pair<std::string, std::string> fitness_function("fitnessFunction", fitnessFunction);
	details.push_back(fitness_function);
	std::pair<std::string, std::string> n_variable("n", std::to_string(n));
	details.push_back(n_variable);
	std::pair<std::string, std::string> run_variable("run", std::to_string(run*MPIInfo::Size+MPIInfo::Rank));
	details.push_back(run_variable);
	std::pair<std::string, std::string> lambda_constant("lambda", std::to_string(lambda));
	details.push_back(lambda_constant);
	std::pair<std::string, std::string> mu_constant("mu", std::to_string(mu));
	details.push_back(mu_constant);
	return details;
}


void run_single(
	int n, int lm, int mu, int run, int best_fitness,
	FitnessClass& fitnessclass,
	std::vector<std::pair<std::string, std::string>> details,
	std::string filepath,
	int maxItr = 10000) {

	umda optimizer(mu, lm, n, fitnessclass);
	optimizer.run_iterations(maxItr, [best_fitness](const EDABase& eda) -> bool { return eda.best_fitness == best_fitness; });

	std::cout << "best fitness" << best_fitness << std::endl;

	std::string _best_fitness = std::to_string(optimizer.best_fitness);
	std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);
	auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, optimizer.history.size() / truncation, optimizer.best_fitness_itr);
	json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);
}

void run_multi(
	int n, int lm, int mu, int run, int best_fitness,
	FitnessClass& fitnessclass, 
	std::vector<std::pair<std::string, std::string>> details, 
	std::string filepath, 
	Topology& topology,
	MigrationStrategy &ms
	) {

	umda optimizer(mu, lm, n, fitnessclass);
	OptimizerCoordinator oc{};
	int itr = 10000;
	oc.run(itr, ms, optimizer, topology, [best_fitness](const EDABase& eda) -> bool { return eda.best_fitness == best_fitness; });

	std::string _best_fitness = std::to_string(optimizer.best_fitness);
	std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);
	if (optimizer.history.size() > truncation) {
		auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, optimizer.history.size() / truncation, optimizer.best_fitness_itr);
		json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);
	}
	else{
		json_file_writer::write_history(details, _best_fitness, best_fitness_itr, optimizer.history, filepath);
	}
	
}

std::string nameCleaner(std::string s) {
	std::string prefix("class ");

	auto res = std::mismatch(prefix.begin(), prefix.end(), s.begin());
	if (res.first == prefix.end())
	{
		s = s.substr(prefix.length());
	}

	while(std::isdigit(s[0])) {
		s = s.substr(1);
	}
	return s;
}


//GENERALISATION ----------------------------------------------------------------

//enum MigrationStrategyEnum
//{
//	sample, musample, prop, nomigration
//};
//struct EnumText {
//	static const char* enumtext[];
//};
//
//const char* EnumText::enumtext[] = { "Sample","MuSample","Prob" , "NoMigration"};
//


void SerielcMcLTest(FitnessClass& fitnessclass, int n, int maxItr, std::vector<std::pair<std::string, std::string>> extradetails) {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string foldername = "../Results/UMDA/" + fitnessName + "/CmCl/";
	std::filesystem::create_directories(foldername);
	int cLs[] = { 1, 2, 4, 8, 16, 32, 64 };
	int cMs[] = { 0, 1, 2, 4, 8,  16, 32, 64 };
	for (int cL : cLs) {
		for (int cM : cMs) {
			int mu = std::log(n) * (1 + cM);
			int lm = (1 + cL) * mu;
			for (int run = 0; run < 4; run++) {
				std::vector<std::pair<std::string, std::string>> details = make_details_seriel(
					n, run, cL, cM,
					"UMDA",
					fitnessName
				);
				for (int i = 0; i < extradetails.size(); i++) {
					details.push_back(extradetails[i]);
				}
				std::string filepath = make_filename(details, foldername);

				run_single(n, lm, mu, run, n, fitnessclass, details, filepath, maxItr);
			}
		}

	}

}

void SerielcMcLTest(FitnessClass& fitnessclass, int n, int maxItr) {
	std::vector<std::pair<std::string, std::string>> noExtraDetails;
	SerielcMcLTest(fitnessclass, n, 10000, noExtraDetails);
}

void SerielcMcLTest(FitnessClass& fitnessclass, int n) {
	SerielcMcLTest(fitnessclass, n, 10000);
}

// -----------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------- LAMBDA TEST --------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------

void SerielLambdaTest(FitnessClass& fitnessclass, int n, int bestfitness, int maxItr, std::vector<std::pair<std::string, std::string>> extradetails, int from, int to, std::string extraPath = "", int delta = 1) {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string path = "../Results/UMDA/" + fitnessName + "/Lambda/"+extraPath + args[0];
	std::string foldername = path+"/";
	std::filesystem::create_directories(foldername);

	int section = std::stoi(args[0]);
	int amountPerSection = 1;


	for (int lambda = from; lambda < to; lambda += delta) {
		for (int run = section * amountPerSection; run < (section + 1) * amountPerSection; run++) {
			int mu = lambda / muLambdaRatio;
			if (mu<=0) {
				mu = 1;
			}
			std::vector<std::pair<std::string, std::string>> details = make_details_seriel(
				n, run, lambda, mu,
				"UMDA",
				fitnessName
			);
			for (int i = 0; i < extradetails.size(); i++) {
				details.push_back(extradetails[i]);
			}
			std::string filepath = make_filename(details, foldername);

			run_single(n, lambda, mu, run, bestfitness, fitnessclass, details, filepath, maxItr);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		if (MPIInfo::Rank == 0) {
			std::string command1 = "zip -q -u -r ../zips/" + args[0] + ".zip " + path;
			if (first) {
				first = false;
				command1 = "zip -q -r ../zips/" + args[0] + ".zip " + path;
			}
			std::system(command1.c_str());
			std::string command2 = "rm -rf " + path+"/*";
			std::system(command2.c_str());
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (MPIInfo::Rank == 0) {
		std::string command2 = "rm -rf " + path;
		std::system(command2.c_str());	
	}
}

void SerielLambdaTest(FitnessClass& fitnessclass, int n, int bestfitness, int maxItr, int from, int to) {
	std::vector<std::pair<std::string, std::string>> noExtraDetails;
	SerielLambdaTest(fitnessclass, n, bestfitness, 10000,noExtraDetails,from,to);
}

void SerielLambdaTest(FitnessClass& fitnessclass, int n, int bestfitness, int from, int to) {
	SerielLambdaTest(fitnessclass, n, bestfitness, 10000, from, to);
}





void UMDANullTest(FitnessClass& fitnessclass, int lambda, int mu, int n, std::vector<std::pair<std::string, std::string>> extradetails) {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	CompleteTopology topology;
	NullMigrationStrategy ms;


	std::string path = "../Results/UMDA/" + fitnessName + "/" + "NullTest/" + args[0];
	std::string foldername = path + "/";
	std::filesystem::create_directories(foldername);

	int section = std::stoi(args[0]);
	for (int run = section * 16; run < (section + 1) * 16; run++) {

		std::vector<std::pair<std::string, std::string>> details = make_details(
			n, run, lambda, mu, 1,
			"UMDA",
			fitnessName,
			"Null",
			"Null"
		);
		for (int i = 0; i < extradetails.size(); i++) {
			details.push_back(extradetails[i]);
		}
		std::string filepath = make_filename(details, foldername);
		run_multi(n, lambda, mu, run, n, fitnessclass, details, filepath, topology, ms);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (MPIInfo::Rank == 0) {
		std::string command1 = "zip -q -u -r ../zips/" + args[0] + ".zip " + path;
		if (first) {
			first = false;
			command1 = "zip -q -r ../zips/" + args[0] + ".zip " + path;
		}
		std::system(command1.c_str());
		std::string command2 = "rm -rf " + path + "/*";
		std::system(command2.c_str());
		std::string command3 = "rm -rf " + path;
		std::system(command3.c_str());
	}
	MPI_Barrier(MPI_COMM_WORLD);
}

//Used for cluster size test and if you wanna run something simple
void UMDATest(FitnessClass& fitnessclass, int mu, int lm, Topology &topology, int n, int bestfitness, MigrationStrategyEnum mse, int mi, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string topologyName = typeid(topology).name();
	topologyName = nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);
	
	std::string path = "../Results/UMDA/" + fitnessName + "/" + topologyName + "/" + EnumText::enumtext[mse] + "/" + endfolder;
	std::string foldername = path + "/";
	std::filesystem::create_directories(foldername);


	MigrationStrategy* ms;
	switch (mse)
	{
	case sample:
	{
		ms = new SampleMigrationStrategy(mi);
		break;
	}
	case musample:
	{
		ms = new MuFromEachSampleMigrationStrategy(mi, mu);
		break;
	}
	case prop:
	{
		ms = new ProbDistMigrationStrategy(0.9, mi);
		break;
	}
	case nomigration:
	{
		ms = new NullMigrationStrategy();
		break;
	}
	default:
		ms = new MigrationStrategy(mi);
		break;
	}

	int section = std::stoi(args[0]);
	for (int run = section * runAmount + runStart; run < (section + 1) * runAmount + runStart; run++) {

		std::vector<std::pair<std::string, std::string>> details = make_details(
			n, run, lm, mu, mi,
			"UMDA",
			fitnessName,
			topologyName,
			EnumText::enumtext[mse]
		);
		for (int i = 0; i < extradetails.size(); i++) {
			details.push_back(extradetails[i]);
		}
		std::string filepath = make_filename(details, foldername);
		run_multi(n, lm, mu, run, bestfitness, fitnessclass, details, filepath, topology, *ms);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (MPIInfo::Rank == 0) {
		std::string command1 = "zip -q -u -r ../zips/" + zipName + ".zip " + path;
		if (first) {
			first = false;
			command1 = "zip -q -r ../zips/" + zipName + ".zip " + path;
		}
		std::system(command1.c_str());
		std::string command2 = "rm -rf " + path + "/*";
		std::system(command2.c_str());
		std::string command3 = "rm -rf " + path;
		std::system(command3.c_str());
	}
	MPI_Barrier(MPI_COMM_WORLD);
}


void UMDAMiTest(FitnessClass& fitnessclass, int lm, int mu, Topology& topology, int n, int bestfitness, MigrationStrategyEnum mse, std::vector<std::pair<std::string, std::string>> extradetails, int from, int to, int sections = 20) {
	double step = ((double)(to - from)) / ((double)(sections-1));
	int oldmi = 0;
	for (int i = 0; i < sections; i++) {
		int mi = from + i * step;
		if (oldmi == mi) {
			continue;
		}
		oldmi = mi;
		UMDATest(fitnessclass, mu, lm, topology, n, bestfitness, mse, mi, extradetails, "MiTest/" + args[0], args[0]);
	}
}

void UMDATestSeriel(FitnessClass& fitnessclass, int mu, int lm, int n, int bestfitness, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);

	std::string path = "../Results/UMDA/" + fitnessName + "/" + endfolder;
	std::string foldername = path + "/";
	std::filesystem::create_directories(foldername);

	int section = std::stoi(args[0]);
	for (int run = section * runAmount/16+runStart; run < (section + 1) * runAmount/16 + runStart; run++) {
		std::vector<std::pair<std::string, std::string>> details = make_details_seriel(
			n, run, lm, mu,
			"UMDA",
			fitnessName
		);
		for (int i = 0; i < extradetails.size(); i++) {
			details.push_back(extradetails[i]);
		}
		std::string filepath = make_filename(details, foldername);

		run_single(n, lm, mu, run, n, fitnessclass, details, filepath);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (MPIInfo::Rank == 0) {
		std::string command1 = "zip -q -u -r ../zips/" + zipName + ".zip " + path;
		if (first) {
			first = false;
			command1 = "zip -q -r ../zips/" + zipName + ".zip " + path;
		}
		std::system(command1.c_str());
		std::string command2 = "rm -rf " + path + "/*";
		std::system(command2.c_str());
		std::string command3 = "rm -rf " + path;
		std::system(command3.c_str());
	}
	MPI_Barrier(MPI_COMM_WORLD);
}


void UMDANTest(FitnessClass& fitnessclass, double paradigmConst, int paradigmType, Topology& topology, int maxN, int segments, MigrationStrategyEnum mse, int mi, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {
	double segmentSize = maxN / segments;
	for (int i = 1; i <= segments; i++) {
		int n = i * segmentSize;
		int lm = 1;
		switch (paradigmType) {
		case LOGN:
		{
			lm = log(n) * paradigmConst;
			break;
		}
		case SQRTNLOGN:
		{
			lm = sqrt(n) * log(n) * paradigmConst;
			break;
		}
		case NLOGN:
		{
			lm = n * log(n) * paradigmConst;
			break;
		}
		}
		int mu = lm / muLambdaRatio;
		if (lm < 1) {
			lm = 1;
		}
		if (mu < 1) {
			mu = 1;
		}
		UMDATest(fitnessclass, mu, lm, topology, n, n, mse, mi, extradetails, endfolder, zipName);
	}
}

void UMDAMuBasedNTest(FitnessClass& fitnessclass, double paradigmConst, int paradigmType, Topology& topology, int maxN, int segments, MigrationStrategyEnum mse, int mi, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {
	double segmentSize = maxN / segments;
	for (int i = 1; i <= segments; i++) {
		int n = i * segmentSize;
		int mu = 1;
		switch (paradigmType) {
		case LOGN:
		{
			mu = log(n) * paradigmConst;
			break;
		}
		case SQRTNLOGN:
		{
			mu = sqrt(n) * log(n) * paradigmConst;
			break;
		}
		case NLOGN:
		{
			mu = n * log(n) * paradigmConst;
			break;
		}
		}
		if (mu < 1) {
			mu = 1;
		}
		int lm = mu * muLambdaRatio;
		if (lm < 1) {
			lm = 1;
		}
		
		UMDATest(fitnessclass, mu, lm, topology, n, n, mse, mi, extradetails, endfolder, zipName);
	}
}

void UMDALambdaTestSeriel(FitnessClass& fitnessclass, int n, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {
	for (int mu = 1; mu <= 20; mu++) {
		for (int lm = mu; lm <= 20; lm++) {
			UMDATestSeriel(fitnessclass, mu, lm, n, n, extradetails, endfolder, zipName);
		}
	}
}



void UMDANTestSeriel(FitnessClass& fitnessclass, double paradigmConst, double muLambdaRatio, int paradigmType, int maxN, int segments, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {
	double segmentSize = maxN / segments;
	for (int i = 1; i <= segments; i++) {
		int n = i * segmentSize;
		int lm = 1;
		switch (paradigmType) {
		case LOGN:
		{
			lm = log(n) * paradigmConst;
			break;
		}
		case SQRTNLOGN:
		{
			lm = sqrt(n) * log(n) * paradigmConst;
			break;
		}
		case NLOGN:
		{
			lm = n * log(n) * paradigmConst;
			break;
		}
		}
		int mu = lm / muLambdaRatio;
		if (lm < 1) {
			lm = 1;
		}
		if (mu < 1) {
			mu = 1;
		}
		UMDATestSeriel(fitnessclass, mu, lm, n, n, extradetails, endfolder, zipName);
	}
}

void UMDANTestSeriel(FitnessClass& fitnessclass, double paradigmConst, int paradigmType, int maxN, int segments, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {
	UMDANTestSeriel(fitnessclass, paradigmConst, muLambdaRatio, paradigmType, maxN, segments, extradetails, endfolder, zipName);
}

void UMDAMuBasedNTestSeriel(FitnessClass& fitnessclass, double paradigmConst, double muLambdaRatio, int paradigmType, int maxN, int segments, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {
	double segmentSize = maxN / segments;
	for (int i = 1; i <= segments; i++) {
		int n = i * segmentSize;
		int mu = 1;
		int lm = 1;
		switch (paradigmType) {
		case LOGN:
		{
			mu = log(n) * paradigmConst;
			break;
		}
		case SQRTNLOGN:
		{
			mu = sqrt(n) * log(n) * paradigmConst;
			break;
		}
		case NLOGN:
		{
			mu = n * log(n) * paradigmConst;
			break;
		}
		}
		if (mu < 1) {
			mu = 1;
		}
		lm = mu * muLambdaRatio;
		if (lm < 1) {
			lm = 1;
		}
		
		UMDATestSeriel(fitnessclass, mu, lm, n, n, extradetails, endfolder, zipName);
	}
}

void UMDAMuBasedNTestSeriel(FitnessClass& fitnessclass, double paradigmConst, int paradigmType, int maxN, int segments, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {
	UMDAMuBasedNTestSeriel(fitnessclass, paradigmConst, muLambdaRatio, paradigmType, maxN, segments, extradetails, endfolder, zipName);
}

void UMDAMaxSatMassiveTest(std::vector<std::string>& args) {
	int clause_count, literal_count;
	int instance_start = std::stoi(args[0]);
	int instance_end = std::stoi(args[1]);

	std::string sizes[] = { "250-1065","225-960","200-860","175-753","150-645","125-538","100-430","75-325","50-218","20-91" };

	for (auto size : sizes) {
		std::string lit_cnt = size.substr(0, size.find_first_of('-'));

		for (int i = instance_start; i <= instance_end; i++) {
			std::string instance_file = (std::stringstream() << "../MaxSat/" << "uf" << size << "/" << "uf" << lit_cnt << "-0" << i << ".cnf").str();
			std::string foldername = "../Results/MaxSatMassive2Mil/" + size;
			std::filesystem::create_directory("../Results");
			std::filesystem::create_directory("../Results/MaxSatMassive2Mil");
			std::filesystem::create_directory(foldername);

			FitnessKSAT fks = SatParser::parse_maxsat_instance(instance_file, clause_count, literal_count);

			for (int cM = 0; cM <= 4; cM++) {
				for (int cL = 1; cL <= 4; cL++) {
					int mu = std::log(literal_count) * (1 + cM);
					int lm = (1 + cL) * mu;
					int max_itr = 2000000;

					umda optimizer(mu, lm, literal_count, fks);
					optimizer.run_iterations(max_itr, [clause_count](const EDABase& eda) -> bool { return eda.best_fitness == clause_count; });
					
					std::vector<std::pair<std::string, std::string>> details = make_details_seriel(
						literal_count, 0, cL, cM, "UMDA", "KSAT"
					);

					std::pair<std::string, std::string> instance_detail("instance", std::to_string(i));
					details.push_back(instance_detail);

					std::string _best_fitness = std::to_string(optimizer.best_fitness);
					std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);
					std::string filepath = make_filename(details, foldername);

					std::cout << filepath << std::endl;

					auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, optimizer.history.size() / 100, optimizer.best_fitness_itr);
					json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);
				}
			}
		}
	}
}

void UMDAMaxSatSerielTest(std::vector<std::string>& args) {
	int clause_count, literal_count;
	int instance_start = std::stoi(args[0]);
	int instance_end = std::stoi(args[1]);

	std::string sizes[] = { "250-1065","225-960","200-860","175-753","150-645","125-538","100-430","75-325","50-218","20-91" };
	
	std::map<std::pair<int, int>, std::pair<int, int>> clcmMap = { // {{size, instance},{cL, cM}, ...}, ...
		{{250, 1}, {1, 1}},		{{250, 2}, {1, 2}},		{{250, 3}, {1, 1}},		{{250, 4}, {2, 0}},		{{250, 5}, {2, 0}},		{{250, 6}, {1, 1}},
		{{225, 1}, {1, 1}},		{{225, 2}, {1, 1}},		{{225, 3}, {3, 1}},		{{225, 4}, {1, 1}},		{{225, 5}, {1, 1}},		{{225, 6}, {1, 1}},
		{{200, 1}, {1, 1}},		{{200, 2}, {1, 1}},		{{200, 3}, {1, 1}},		{{200, 4}, {1, 1}},		{{200, 5}, {2, 0}},		{{200, 6}, {2, 0}},
		{{175, 1}, {1, 1}},		{{175, 2}, {1, 1}},		{{175, 3}, {1, 0}},		{{175, 4}, {1, 1}},		{{175, 5}, {3, 0}},		{{175, 6}, {1, 0}},
		{{150, 1}, {1, 0}},		{{150, 2}, {1, 0}},		{{150, 3}, {1, 0}},		{{150, 4}, {1, 0}},		{{150, 5}, {1, 0}},		{{150, 6}, {1, 0}},
		{{125, 1}, {1, 0}},		{{125, 2}, {1, 1}},		{{125, 3}, {1, 0}},		{{125, 4}, {2, 0}},		{{125, 5}, {2, 0}},		{{125, 6}, {2, 0}},
		{{100, 1}, {2, 0}},		{{100, 2}, {2, 0}},		{{100, 3}, {1, 0}},		{{100, 4}, {1, 0}},		{{100, 5}, {1, 0}},		{{100, 6}, {1, 0}},
		{{75,  1}, {1, 0}},		{{75,  2}, {1, 0}},		{{75,  3}, {1, 1}},		{{75,  4}, {1, 1}},		{{75,  5}, {1, 0}},		{{75,  6}, {1, 0}},
		{{50,  1}, {2, 0}},		{{50,  2}, {2, 0}},		{{50,  3}, {1, 0}},		{{50,  4}, {2, 0}},		{{50,  5}, {2, 0}},		{{50,  6}, {1, 0}},
		{{20,  1}, {2, 0}},		{{20,  2}, {2, 0}},		{{20,  3}, {2, 0}},		{{20,  4}, {2, 0}},		{{20,  5}, {2, 0}},		{{20,  6}, {1, 0}}
	};

	for (auto size : sizes) {
		std::string lit_cnt = size.substr(0, size.find_first_of('-'));

		for (int i = instance_start; i <= instance_end; i++) {
			std::string instance_file = (std::stringstream() << "../MaxSat/" << "uf" << size << "/" << "uf" << lit_cnt << "-0" << i << ".cnf").str();
			std::string foldername = "../Results/MaxSatNullMigration/" + size;
			std::filesystem::create_directory("../Results");
			std::filesystem::create_directory("../Results/MaxSatNullMigration");
			std::filesystem::create_directory(foldername);

			FitnessKSAT fks = SatParser::parse_maxsat_instance(instance_file, clause_count, literal_count);

			int cM, cL;
			auto conf_it = clcmMap.find({ literal_count, i });

			if (conf_it != clcmMap.end()) {
				cL = conf_it->first.first;
				cM = conf_it->first.second;
			}
			else {
				throw std::logic_error((std::stringstream() << "Configuration for (" << literal_count << ", " << i << ") not found.").str());
			}

			int mu = std::log(literal_count) * (1 + cM);
			int lm = (1 + cL) * mu;
			int max_itr = 2000000;

			for (int run = 0; run < 50; run++) {
				umda optimizer(mu, lm, literal_count, fks);
				NullMigrationStrategy ms; KCircleTopology topology(1);
				OptimizerCoordinator oc{};

				oc.run(max_itr, ms, optimizer, topology, [clause_count](const EDABase& eda) -> bool { return eda.best_fitness == clause_count; });
				
				std::vector<std::pair<std::string, std::string>> details = make_details(
					literal_count, run, cL, cM, 1, "UMDA", "KSAT", "Null", "Null"
				);

				std::pair<std::string, std::string> instance_detail("instance", std::to_string(i));
				details.push_back(instance_detail);

				std::string _best_fitness = std::to_string(optimizer.best_fitness);
				std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);
				std::string filepath = make_filename(details, foldername);

				if(MPIInfo::Rank == 0) std::cout << "Run: " << run << " Instance: " << i << " Size: " << literal_count << " Itr: " << optimizer._itr_count << std::endl;

				auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, optimizer.history.size() / 500, optimizer.best_fitness_itr);
				json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);
			}

			if (MPIInfo::Rank == 0) std::cout << std::endl << std::endl;
		}
	}
}

void UMDAMaxSatParallelTest(std::vector<std::string>& args, Topology& topology, MigrationStrategyEnum mse) {
	int clause_count, literal_count;
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string sizes[] = { "200-860" };
	int instances[] = { 1, 3, 5 };

	std::string topologyName = typeid(topology).name();
	topologyName = nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);

	std::map<std::pair<int, int>, std::pair<int, int>> clcmMap = { // {{size, instance},{cL, cM}, ...}, ...
		{{250, 1}, {1, 1}},		{{250, 2}, {1, 2}},		{{250, 3}, {1, 1}},		{{250, 4}, {2, 0}},		{{250, 5}, {2, 0}},		{{250, 6}, {1, 1}},
		{{225, 1}, {1, 1}},		{{225, 2}, {1, 1}},		{{225, 3}, {3, 1}},		{{225, 4}, {1, 1}},		{{225, 5}, {1, 1}},		{{225, 6}, {1, 1}},
		{{200, 1}, {1, 1}},		{{200, 2}, {1, 1}},		{{200, 3}, {1, 1}},		{{200, 4}, {1, 1}},		{{200, 5}, {2, 0}},		{{200, 6}, {2, 0}},
		{{175, 1}, {1, 1}},		{{175, 2}, {1, 1}},		{{175, 3}, {1, 0}},		{{175, 4}, {1, 1}},		{{175, 5}, {3, 0}},		{{175, 6}, {1, 0}},
		{{150, 1}, {1, 0}},		{{150, 2}, {1, 0}},		{{150, 3}, {1, 0}},		{{150, 4}, {1, 0}},		{{150, 5}, {1, 0}},		{{150, 6}, {1, 0}},
		{{125, 1}, {1, 0}},		{{125, 2}, {1, 1}},		{{125, 3}, {1, 0}},		{{125, 4}, {2, 0}},		{{125, 5}, {2, 0}},		{{125, 6}, {2, 0}},
		{{100, 1}, {2, 0}},		{{100, 2}, {2, 0}},		{{100, 3}, {1, 0}},		{{100, 4}, {1, 0}},		{{100, 5}, {1, 0}},		{{100, 6}, {1, 0}},
		{{75,  1}, {1, 0}},		{{75,  2}, {1, 0}},		{{75,  3}, {1, 1}},		{{75,  4}, {1, 1}},		{{75,  5}, {1, 0}},		{{75,  6}, {1, 0}},
		{{50,  1}, {2, 0}},		{{50,  2}, {2, 0}},		{{50,  3}, {1, 0}},		{{50,  4}, {2, 0}},		{{50,  5}, {2, 0}},		{{50,  6}, {1, 0}},
		{{20,  1}, {2, 0}},		{{20,  2}, {2, 0}},		{{20,  3}, {2, 0}},		{{20,  4}, {2, 0}},		{{20,  5}, {2, 0}},		{{20,  6}, {1, 0}}
	};

	for (auto size : sizes) {
		std::string lit_cnt = size.substr(0, size.find_first_of('-'));
		for (auto i : instances) {
			std::string instance_file = (std::stringstream() << "../MaxSat/" << "uf" << size << "/" << "uf" << lit_cnt << "-0" << i << ".cnf").str();
			std::string foldername = "../Results/MaxSat/" + topologyName + "/" + EnumText::enumtext[mse] + "/MiTest";
			std::filesystem::create_directories(foldername);

			FitnessKSAT fks = SatParser::parse_maxsat_instance(instance_file, clause_count, literal_count);

			//Get cl cm
			int cM, cL;
			auto conf_it = clcmMap.find({ literal_count, i });
			if (conf_it != clcmMap.end()) { cL = conf_it->second.first; cM = conf_it->second.second; }
			else { throw std::logic_error((std::stringstream() << "Configuration for (" << literal_count << ", " << i << ") not found.").str()); }

			int mu = std::log(literal_count) * (1 + cM);
			int lm = (1 + cL) * mu;
			int max_itr = 2000000;

			//						 0  1  2  3  4   5   6   7    8    9    10    11    12    13    14     15     16    17      18
			std::vector<int> mis = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536,131072, 262144 };;
			std::vector<int> misFinal;

			int topI = topologyName == "Complete" ? 0 : topologyName == "KCircle" ? 1 : topologyName == "Star" ? 2 : topologyName == "Taurus" ? 3 : -1;
			int stratI = -1;
			switch (mse) {
			case 0: { stratI = 1; break; }
			case 1: { stratI = 0; break; }
			case 2: { stratI = 2; break; }
			case 3: { stratI = -1000; break; }
			default: stratI = -1; break;
			}
			int instI = i == 1 ? 0 : i == 3 ? 1 : i == 5 ? 2 : -100;
			int startIndexIndex = 9 * topI + 3 * stratI + instI;
			int startIndex[] = {
				//Complete
				//MuSample
				14, 9, 16, // instance 2 4 5
				//Sample
				14, 13, 16,
				//prob
				8, 6, 8,
				// KCircle
				14,7,16,
				14,8,14,
				8,6,8,
				//Star
				0,0,0,
				0,0,0,
				0,0,0,
				//Torus
				14,10,17,
				13,8,14,
				7,4,8
			};

			for (int j = startIndex[startIndexIndex]; j < mis.size(); j++) { misFinal.push_back(mis[j]); }

			for (int mi : misFinal) {
				MigrationStrategy* ms;
				switch (mse) {
				case sample: { ms = new SampleMigrationStrategy(mi); break; }
				case musample: { ms = new MuFromEachSampleMigrationStrategy(mi, mu); break; }
				case prop: { ms = new ProbDistMigrationStrategy(0.9, mi); break; }
				case nomigration: { ms = new NullMigrationStrategy(); break; }
				default: ms = new MigrationStrategy(mi); break; }


				if (MPIInfo::Rank == 0) std::cout << topologyName << " " << EnumText::enumtext[mse] << " Instance: " << i << " MI: " << mi  << std::endl;

				for (int run = run_start; run <= run_end; run++) {
					umda optimizer(mu, lm, literal_count, fks);
                                        OptimizerCoordinator oc{};
					oc.run(max_itr, *ms, optimizer, topology, [clause_count](const EDABase& eda) -> bool { return eda.best_fitness == clause_count; });

					std::vector<std::pair<std::string, std::string>> details = make_details(
						literal_count, run, cL, cM, mi, "UMDA", "KSAT", topologyName, EnumText::enumtext[mse]
					);

					std::pair<std::string, std::string> instance_detail("instance", std::to_string(i));
					details.push_back(instance_detail);

					std::string _best_fitness = std::to_string(optimizer.best_fitness);
					std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);
					std::string filepath = make_filename(details, foldername);

					auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, 1, optimizer.best_fitness_itr);
					json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);
				}
			}
		}
	}
}
	
void parse_args(int argc, char* argv[]) {
	if (argc <= 0 || argc > 64) {
		args.push_back("0");
		return;
	}
	std::vector<std::string> argsHolder(argv + 1, argv + argc);
	args = argsHolder;
}



void ResultsTests(int fitnessIndex) {

	CompleteTopology ctop;
	KCircleTopology circletop(1);
	StarTopology startop;
	int taurusSide = sqrt(MPIInfo::Size);
	TaurusTopology taurustop(taurusSide, taurusSide);

	MigrationStrategyEnum mses = MigrationStrategyEnum::sample;
	MigrationStrategyEnum msems = MigrationStrategyEnum::musample;
	MigrationStrategyEnum msep = MigrationStrategyEnum::prop;
	FitnessClass* fitness = new FitnessLeadingOnes();
	int n, lambda, mu;
	std::vector<std::pair<std::string, std::string>> extraDetails;
	int Mis[12] = {
				1,1,1,
				1,1,1,
				1,1,1,
				1,1,1 };
	int mifrom = 1;
	int mito = 64;
	double paradigmConst = 1;
	double muParadigmConst = 1;
	int paradigmType = LOGN;
	int section = std::stoi(args[0]);
	std::string testName = "test/" + args[0];
	std::string zipName = args[0];
	switch (fitnessIndex) {
		case 0:
		{
			fitness = new FitnessOneMax();
			n = 1000;
			lambda = 17;
			//lambda = 367;
			mu = lambda / muLambdaRatio;
			std::pair<std::string, std::string> paradigm("paradigm", "log(n)");
			extraDetails.push_back(paradigm);
			mifrom = 1;
			mito = 1000;
			Mis[9] = 3;
			Mis[10] = 16;
			Mis[11] = 3;
			paradigmConst = lambda / log(n);
			muParadigmConst = mu / log(n);
			paradigmType = LOGN;
			break;
		}
		case 1:
		{
			fitness = new FitnessOneMax();
			n = 1000;
			lambda = 367;
			mu = lambda / muLambdaRatio;
			std::pair<std::string, std::string> paradigm("paradigm", "sqrt(n) log(n)");
			extraDetails.push_back(paradigm);
			mifrom = 1;
			mito = 50;
			Mis[10] = 53;
			paradigmConst = lambda / (sqrt(n) * log(n));
			muParadigmConst = mu / (sqrt(n) * log(n));
			paradigmType = SQRTNLOGN;
			break;
		}
		case 2:
		{
			fitness = new FitnessLeadingOnes();
			n = 100;
			lambda = 15;
			mu = lambda / muLambdaRatio;
			std::pair<std::string, std::string> paradigm("paradigm", "n log(n)");
			extraDetails.push_back(paradigm);
			mifrom = 1;
			mito = 600;
			paradigmConst = lambda / (n * log(n));
			muParadigmConst = mu / (n * log(n));
			paradigmType = NLOGN;
			break;
		}
		case 3:
		{
			fitness = new FitnessTwoPeaks();
			n = 100;
			lambda = 16;
			mu = lambda / muLambdaRatio;
			std::pair<std::string, std::string> paradigm("paradigm", "n log(n)");
			extraDetails.push_back(paradigm);
			//mifrom = 1;
			//mito = 600;
			mifrom = 1;
			mito = 100;
			paradigmConst = lambda / (n * log(n));
			muParadigmConst = mu / (n * log(n));
			paradigmType = NLOGN;
			break;
		}
	}
	//SerielcMcLTest(fitness, n, 10000);

	//int originalFrom = 1;
	//int originalTo = 451;
	//for (n = 50;n <= 500;n+=50) {
	//	SerielLambdaTest(*fitness, n, 10000, originalFrom, originalTo);
	//}

	//UMDANullTest(*fitness, lambda, mu, n, extraDetails);

	//UMDAMiTest(*fitness, lambda, mu, ctop, n, mses, extraDetails,mifrom,mito);
	//UMDAMiTest(*fitness, lambda, mu, ctop, n, msems, extraDetails, mifrom, mito);
	//UMDAMiTest(*fitness, lambda, mu, ctop, n, msep, extraDetails, mifrom, mito);
	//UMDAMiTest(*fitness, lambda, mu, circletop, n, mses, extraDetails, mifrom, mito);
	//UMDAMiTest(*fitness, lambda, mu, circletop, n, msems, extraDetails, mifrom, mito);
	//UMDAMiTest(*fitness, lambda, mu, circletop, n, msep, extraDetails, mifrom, mito);
	//UMDAMiTest(*fitness, lambda, mu, startop, n, mses, extraDetails, mifrom, mito);
	//UMDAMiTest(*fitness, lambda, mu, startop, n, msems, extraDetails, mifrom, mito);
	//UMDAMiTest(*fitness, lambda, mu, startop, n, msep, extraDetails, mifrom, mito);
	//UMDAMiTest(*fitness, lambda, mu, taurustop, n, mses, extraDetails, mifrom, mito);
	//UMDAMiTest(*fitness, lambda, mu, taurustop, n, msems, extraDetails, mifrom, mito);
	//UMDAMiTest(*fitness, lambda, mu, taurustop, n, msep, extraDetails, mifrom, mito);


	std::string size = std::to_string(MPIInfo::Size);


	//UMDATest(*fitness, mu, lambda, ctop, n, nomigration, Mis[0], extraDetails, "cluster_test/"+size, "Null" + size);
	//UMDATest(*fitness, mu, lambda, ctop, n, mses, Mis[0], extraDetails, "cluster_test/"+size, "complete" + size);
	//UMDATest(*fitness, mu, lambda, ctop, n, msems, Mis[1], extraDetails, "cluster_test/"+size,"complete"+size);
	//UMDATest(*fitness, mu, lambda, ctop, n, msep, Mis[2], extraDetails, "cluster_test/"+size,"complete"+size);
	//UMDATest(*fitness, mu, lambda, circletop, n, mses, Mis[3], extraDetails, "cluster_test/"+size,"circle"+size);
	//UMDATest(*fitness, mu, lambda, circletop, n, msems, Mis[4], extraDetails, "cluster_test/"+size,"circle"+size);
	//UMDATest(*fitness, mu, lambda, circletop, n, msep, Mis[5], extraDetails, "cluster_test/"+size,"circle"+size);
	//UMDATest(*fitness, mu, lambda, startop, n, mses, Mis[6], extraDetails, "cluster_test/","star"+size);
	//UMDATest(*fitness, mu, lambda, startop, n, msems, Mis[7], extraDetails, "cluster_test/"+size,"star"+size);
	//UMDATest(*fitness, mu, lambda, startop, n, msep, Mis[8], extraDetails, "cluster_test/"+size,"star"+size);

	//UMDATest(*fitness, mu, lambda, taurustop, n, mses, Mis[8], extraDetails, "cluster_test/"+size, "taurus"+size);
	//UMDATest(*fitness, mu, lambda, taurustop, n, msems, Mis[8], extraDetails, "cluster_test/"+size, "taurus"+size);
	//UMDATest(*fitness, mu, lambda, taurustop, n, msep, Mis[8], extraDetails, "cluster_test/"+size, "taurus"+size);


	//std::string testName = "Ntest/" + args[0];
	//testName = "LargerNtest/" + args[0];
	//zipName = args[0];

	//paradigmConst = paradigmConst * 3;

	//UMDANTestSeriel(*fitness, paradigmConst, paradigmType, n * 2, 20, extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, ctop, n*2, 20, nomigration, Mis[0], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, ctop, n*2, 20, mses, Mis[0], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, ctop, n*2, 20, msems, Mis[1], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, ctop, n*2, 20, msep, Mis[2], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, circletop, n*2, 20, mses, Mis[3], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, circletop, n*2, 20, msems, Mis[4], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, circletop, n*2, 20, msep, Mis[5], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, startop, n*2, 20, mses, Mis[6], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, startop, n*2, 20, msems, Mis[7], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, startop, n*2, 20, msep, Mis[8], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, taurustop, n*2, 20, mses, Mis[9], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, taurustop, n*2, 20, msems, Mis[10], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, taurustop, n*2, 20, msep, Mis[11], extraDetails, testName, zipName);

	//testName = "MuBasedNTest/" + args[0];
	//zipName = args[0];
	//UMDAMuBasedNTestSeriel(*fitness, muParadigmConst, paradigmType, n * 2, 20, extraDetails, testName, zipName);
	//UMDAMuBasedNTest(*fitness, muParadigmConst, paradigmType, ctop, n*2, 20, nomigration, Mis[0], extraDetails, testName, zipName);
	//UMDAMuBasedNTest(*fitness, muParadigmConst, paradigmType, ctop, n*2, 20, mses, Mis[0], extraDetails, testName, zipName);
	//UMDAMuBasedNTest(*fitness, muParadigmConst, paradigmType, ctop, n*2, 20, msems, Mis[1], extraDetails, testName, zipName);
	//UMDAMuBasedNTest(*fitness, muParadigmConst, paradigmType, ctop, n*2, 20, msep, Mis[2], extraDetails, testName, zipName);

	testName = "SpecificMiTest/" + args[0];
	zipName = args[0];
	int testMis[13] = { 1, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600 };
	//int testMis[1] = { 250};
	for (int mi : testMis) {
		//UMDATest(*fitness, mu, lambda, ctop, n, nomigration, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, ctop, n, n, mses, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, ctop, n, n, msems, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, ctop, n, n, msep, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, circletop, n, n, mses, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, circletop, n, n, msems, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, circletop, n, n, msep, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, taurustop, n, n, mses, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, taurustop, n, n, msems, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, taurustop, n, n, msep, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, startop, n, n, mses, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, startop, n, n, msems, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, startop, n, n, msep, mi, extraDetails, testName, zipName);
	}
}

void ResultsTestsJump() {

	CompleteTopology ctop;
	KCircleTopology circletop(1);
	StarTopology startop;
	int taurusSide = sqrt(MPIInfo::Size);
	TaurusTopology taurustop(taurusSide, taurusSide);

	MigrationStrategyEnum mses = MigrationStrategyEnum::sample;
	MigrationStrategyEnum msems = MigrationStrategyEnum::musample;
	MigrationStrategyEnum msep = MigrationStrategyEnum::prop;
	FitnessClass* fitness;
	int n, lambda, mu;
	std::vector<std::pair<std::string, std::string>> extraDetails;
	int Mis[12] = {
				1,1,1,
				1,1,1,
				1,1,1,
				1,1,1 };
	int mifrom = 1;
	int mito = 10;
	double paradigmConst = 1;
	int paradigmType = NLOGN;

	fitness = new FitnessJumpk(6);
	n = 100;
	lambda = 376;
	mu = lambda / muLambdaRatio;
	std::pair<std::string, std::string> paradigm("paradigm", "nlog(n)");
	extraDetails.push_back(paradigm);
	paradigmConst = lambda / log(n);


	int originalFrom = 1;
	int originalTo = 501;

	//truncation = 1;
	//for (int k = 2; k <= 10; k++) {
	//	fitness = new FitnessJumpk(k);
	//	std::vector<std::pair<std::string, std::string>> localDetails;
	//	std::pair<std::string, std::string> jumpKValue("jumpK", std::to_string(k));
	//	localDetails.push_back(jumpKValue);
	//	SerielLambdaTest(*fitness, n, n+k, 10000, localDetails, originalFrom, originalTo,"jumpK_"+ std::to_string(k)+"/",20);
	//}

	//fitness = new FitnessJumpk(6);
	//std::vector<std::pair<std::string, std::string>> jumpkDetails;
	//std::pair<std::string, std::string> jumpkDetail("jumpK", std::to_string(6));
	//jumpkDetails.push_back(jumpkDetail);
	//SerielLambdaTest(*fitness, n, 10000, jumpkDetails, 300, 600, "jumpK_" + std::to_string(6) + "/", 20);

	//UMDANullTest(*fitness, lambda, mu, n, extraDetails);

	int sections = 10;

	//UMDAMiTest(*fitness, lambda, mu, ctop, n, mses, extraDetails,mifrom,mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, ctop, n, msems, extraDetails, mifrom, mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, ctop, n, msep, extraDetails, mifrom, mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, circletop, n, mses, extraDetails, mifrom, mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, circletop, n, msems, extraDetails, mifrom, mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, circletop, n, msep, extraDetails, mifrom, mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, startop, n, mses, extraDetails, mifrom, mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, startop, n, msems, extraDetails, mifrom, mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, startop, n, msep, extraDetails, mifrom, mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, taurustop, n, mses, extraDetails, mifrom, mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, taurustop, n, msems, extraDetails, mifrom, mito, sections);
	//UMDAMiTest(*fitness, lambda, mu, taurustop, n, msep, extraDetails, mifrom, mito, sections);


	std::string size = std::to_string(MPIInfo::Size);


	//UMDATest(*fitness, mu, lambda, ctop, n, nomigration, Mis[0], extraDetails, "cluster_test/" + size, "Null" + size);
	//UMDATest(*fitness, mu, lambda, ctop, n, mses, Mis[0], extraDetails, "cluster_test/" + size, "complete" + size);
	//UMDATest(*fitness, mu, lambda, ctop, n, msems, Mis[1], extraDetails, "cluster_test/" + size, "complete" + size);
	//UMDATest(*fitness, mu, lambda, ctop, n, msep, Mis[2], extraDetails, "cluster_test/" + size, "complete" + size);
	//UMDATest(*fitness, mu, lambda, circletop, n, mses, Mis[3], extraDetails, "cluster_test/" + size, "circle" + size);
	//UMDATest(*fitness, mu, lambda, circletop, n, msems, Mis[4], extraDetails, "cluster_test/" + size, "circle" + size);
	//UMDATest(*fitness, mu, lambda, circletop, n, msep, Mis[5], extraDetails, "cluster_test/" + size, "circle" + size);
	//UMDATest(*fitness, mu, lambda, startop, n, mses, Mis[6], extraDetails, "cluster_test/", "star" + size);
	//UMDATest(*fitness, mu, lambda, startop, n, msems, Mis[7], extraDetails, "cluster_test/" + size, "star" + size);
	//UMDATest(*fitness, mu, lambda, startop, n, msep, Mis[8], extraDetails, "cluster_test/" + size, "star" + size);

	//UMDATest(*fitness, mu, lambda, taurustop, n, mses, Mis[8], extraDetails, "cluster_test/"+size, "taurus"+size);
	//UMDATest(*fitness, mu, lambda, taurustop, n, msems, Mis[8], extraDetails, "cluster_test/"+size, "taurus"+size);
	//UMDATest(*fitness, mu, lambda, taurustop, n, msep, Mis[8], extraDetails, "cluster_test/"+size, "taurus"+size);

	//truncation = 1;
	//for (int k = 6; k <= 10; k += 1) {
	//	fitness = new FitnessJumpk(k);
	//	std::vector<std::pair<std::string, std::string>> localDetails;
	//	std::pair<std::string, std::string> jumpKValue("jumpK", std::to_string(k));
	//	localDetails.push_back(jumpKValue);

	//	std::string testName = "jumpKTest/"+args[0];
	//	std::string zipName = args[0];
	//	UMDATest(*fitness, mu, lambda, ctop, n, n+k, nomigration, Mis[0], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, ctop, n, n+k, mses, Mis[0], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, ctop, n, n + k, msems, Mis[1], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, ctop, n, n + k, msep, Mis[2], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, circletop, n, n + k, mses, Mis[3], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, circletop, n, n + k, msems, Mis[4], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, circletop, n, n + k, msep, Mis[5], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, startop, n, n + k, mses, Mis[6], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, startop, n, n + k, msems, Mis[7], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, startop, n, n + k, msep, Mis[8], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, taurustop, n, n + k, mses, Mis[8], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, taurustop, n, n + k, msems, Mis[8], localDetails, testName, zipName);
	//	UMDATest(*fitness, mu, lambda, taurustop, n, n + k, msep, Mis[8], localDetails, testName, zipName);
	//}
}

void OneMaxSampleSpecificMiTest() {
	CompleteTopology ctop;
	KCircleTopology circletop(1);
	StarTopology startop;
	int taurusSide = sqrt(MPIInfo::Size);
	TaurusTopology taurustop(taurusSide, taurusSide);

	MigrationStrategyEnum mses = MigrationStrategyEnum::sample;
	MigrationStrategyEnum msems = MigrationStrategyEnum::musample;
	MigrationStrategyEnum msep = MigrationStrategyEnum::prop;
	FitnessClass* fitness;
	int n, lambda, mu;
	std::vector<std::pair<std::string, std::string>> extraDetails;
	int Mis[12] = {
				1,1,1,
				1,1,1,
				1,1,1,
				1,1,1 };
	int mifrom = 1;
	int mito = 64;
	double paradigmConst = 1;
	int paradigmType = LOGN;

	fitness = new FitnessOneMax();
	n = 1000;
	lambda = 17;
	//lambda = 367;
	mu = lambda / muLambdaRatio;
	std::pair<std::string, std::string> paradigm("paradigm", "log(n)");
	extraDetails.push_back(paradigm);
	mifrom = 1;
	mito = 1000;
	Mis[9] = 3;
	Mis[10] = 16;
	Mis[11] = 3;
	paradigmConst = lambda / log(n);
	paradigmType = LOGN;

	int testMis[5] = {1, 211, 369, 579 ,842 };
	std::string testName = "SpecificMiTest/" + args[0];
	std::string zipName = args[0];
	for (int mi : testMis) {
		UMDATest(*fitness, mu, lambda, ctop, n, n, mses, mi, extraDetails, testName, zipName);
	}
}

void LeadingOnesTest() {
	CompleteTopology ctop;
	KCircleTopology circletop(1);
	StarTopology startop;
	int taurusSide = sqrt(MPIInfo::Size);
	TaurusTopology taurustop(taurusSide, taurusSide);

	MigrationStrategyEnum mses = MigrationStrategyEnum::sample;
	MigrationStrategyEnum msems = MigrationStrategyEnum::musample;
	MigrationStrategyEnum msep = MigrationStrategyEnum::prop;
	FitnessClass* fitness;
	int n, lambda, mu;
	std::vector<std::pair<std::string, std::string>> extraDetails;
	double paradigmConst;
	int paradigmType;
	int section = std::stoi(args[0]);

	int Mis[12] = {
				1,1,1,
				1,1,1,
				1,1,1,
				1,1,1 };

	fitness = new FitnessLeadingOnes();
	n = 100;
	lambda = 15;
	mu = lambda / muLambdaRatio;
	std::pair<std::string, std::string> paradigm("paradigm", "n log(n)");
	extraDetails.push_back(paradigm);
	paradigmConst = lambda / (n * log(n));
	double newMuLambdaRatio;
	newMuLambdaRatio = lambda / mu;
	paradigmType = NLOGN;

	std::string testName;
	std::string zipName;

	//testName = "LambdaTestSpecific40/" + args[0];
	//zipName = args[0];
	//UMDALambdaTestSeriel(*fitness, n, extraDetails, testName, zipName);

	//testName = "Mu1Lambda13NTest/" + args[0];
	//zipName = args[0];
	//UMDANTestSeriel(*fitness, paradigmConst, paradigmType, 200, 20, extraDetails, testName, zipName);

	//testName = "Mu1Lambda13NTest/" + args[0];
	//zipName = args[0];
	//UMDANTest(*fitness, paradigmConst, paradigmType, ctop, 200, 20, msems, Mis[1], extraDetails, testName, zipName);
	//UMDANTest(*fitness, paradigmConst, paradigmType, ctop, 200, 20, msep, Mis[2], extraDetails, testName, zipName);
	 
	testName = "SpecificMiTest/"+args[0];
	zipName = args[0];
	int testMis[5] = { 1, 50, 100, 150, 200 };
	for (int mi : testMis) {
		UMDATest(*fitness, mu, lambda, ctop, n, n, nomigration, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, ctop, n, n, mses, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, ctop, n, n, msems, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, ctop, n, n, msep, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, circletop, n, n, mses, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, circletop, n, n, msems, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, circletop, n, n, msep, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, taurustop, n, n, mses, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, taurustop, n, n, msems, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, taurustop, n, n, msep, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, startop, n, n, mses, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, startop, n, n, msems, mi, extraDetails, testName, zipName);
		UMDATest(*fitness, mu, lambda, startop, n, n, msep, mi, extraDetails, testName, zipName);
	}
}

void JacobPlayground(FitnessClass* fitnessclass, MigrationStrategy*ms, Topology* topology, std::string foldername) {

	//CompleteTopology circletop;

	//ProbDistMigrationStrategy* ms = new ProbDistMigrationStrategy(0.9, 129);
	////NullMigrationStrategy* ms = new NullMigrationStrategy();

	//FitnessClass* fitness = new FitnessOneMax();

	//JacobPlayground(fitness, ms, &circletop, "D:\\Workspaces\\TestResults\\");

	int n = 1000;
	//int k = (132 / (std::log(1000) * std::sqrt(1000)))  *  std::sqrt(n) * std::log(n);
	int k = 0;
	int maxItr = 100000;

	int step_count = 500;

	for (int run = 0; run < 20; run++) {
		CGA optimizer(n, k, *fitnessclass);
		OptimizerCoordinator oc{};
		oc.run(maxItr, *ms, optimizer, *topology, [n](const EDABase& eda) -> bool { return eda.best_fitness == n; });

		std::string _best_fitness = std::to_string(optimizer.best_fitness);
		std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);

		auto details = cga_make_details(n, run, 0, k, "CGA", "Something", "something", "something", "something");
		std::string filepath = make_filename(details, foldername);
		if(MPIInfo::Rank == 0)std::cout << "Done Run: " << run << std::endl;
		auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, optimizer.history.size() / step_count, optimizer.best_fitness_itr);
		json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);
	}
}

void UMDAMaxSatParallelTest2(std::vector<std::string>& args, Topology& topology, MigrationStrategyEnum mse, std::vector<int> misFinal, int i) {
	int clause_count, literal_count;
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string sizes[] = { "200-860" };

	std::string topologyName = typeid(topology).name();
	topologyName = nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);

	std::map<std::pair<int, int>, std::pair<int, int>> clcmMap = { // {{size, instance},{cL, cM}, ...}, ...
		{{250, 1}, {1, 1}},		{{250, 2}, {1, 2}},		{{250, 3}, {1, 1}},		{{250, 4}, {2, 0}},		{{250, 5}, {2, 0}},		{{250, 6}, {1, 1}},
		{{225, 1}, {1, 1}},		{{225, 2}, {1, 1}},		{{225, 3}, {3, 1}},		{{225, 4}, {1, 1}},		{{225, 5}, {1, 1}},		{{225, 6}, {1, 1}},
		{{200, 1}, {1, 1}},		{{200, 2}, {1, 1}},		{{200, 3}, {1, 1}},		{{200, 4}, {1, 1}},		{{200, 5}, {2, 0}},		{{200, 6}, {2, 0}},
		{{175, 1}, {1, 1}},		{{175, 2}, {1, 1}},		{{175, 3}, {1, 0}},		{{175, 4}, {1, 1}},		{{175, 5}, {3, 0}},		{{175, 6}, {1, 0}},
		{{150, 1}, {1, 0}},		{{150, 2}, {1, 0}},		{{150, 3}, {1, 0}},		{{150, 4}, {1, 0}},		{{150, 5}, {1, 0}},		{{150, 6}, {1, 0}},
		{{125, 1}, {1, 0}},		{{125, 2}, {1, 1}},		{{125, 3}, {1, 0}},		{{125, 4}, {2, 0}},		{{125, 5}, {2, 0}},		{{125, 6}, {2, 0}},
		{{100, 1}, {2, 0}},		{{100, 2}, {2, 0}},		{{100, 3}, {1, 0}},		{{100, 4}, {1, 0}},		{{100, 5}, {1, 0}},		{{100, 6}, {1, 0}},
		{{75,  1}, {1, 0}},		{{75,  2}, {1, 0}},		{{75,  3}, {1, 1}},		{{75,  4}, {1, 1}},		{{75,  5}, {1, 0}},		{{75,  6}, {1, 0}},
		{{50,  1}, {2, 0}},		{{50,  2}, {2, 0}},		{{50,  3}, {1, 0}},		{{50,  4}, {2, 0}},		{{50,  5}, {2, 0}},		{{50,  6}, {1, 0}},
		{{20,  1}, {2, 0}},		{{20,  2}, {2, 0}},		{{20,  3}, {2, 0}},		{{20,  4}, {2, 0}},		{{20,  5}, {2, 0}},		{{20,  6}, {1, 0}}
	};

	for (auto size : sizes) {
		std::string lit_cnt = size.substr(0, size.find_first_of('-'));
		std::string instance_file = (std::stringstream() << "D:/Workspaces/MasterEDA2/MaxSat/" << "uf" << size << "/" << "uf" << lit_cnt << "-0" << i << ".cnf").str();
		//std::string foldername = "../Results/MaxSat/" + topologyName + "/" + EnumText::enumtext[mse] + "/MiTest";
		//std::filesystem::create_directories(foldername);

		FitnessKSAT fks = SatParser::parse_maxsat_instance(instance_file, clause_count, literal_count);

		//Get cl cm
		int cM, cL;
		auto conf_it = clcmMap.find({ literal_count, i });
		if (conf_it != clcmMap.end()) { cL = conf_it->second.first; cM = conf_it->second.second; }
		else { throw std::logic_error((std::stringstream() << "Configuration for (" << literal_count << ", " << i << ") not found.").str()); }

		int mu = std::log(literal_count) * (1 + cM);
		int lm = (1 + cL) * mu;
		int max_itr = 2000000;

		for (int mi : misFinal) {
			MigrationStrategy* ms;
			switch (mse) {
			case sample: { ms = new SampleMigrationStrategy(mi); break; }
			case musample: { ms = new MuFromEachSampleMigrationStrategy(mi, mu); break; }
			case prop: { ms = new ProbDistMigrationStrategy(0.9, mi); break; }
			case nomigration: { ms = new NullMigrationStrategy(); break; }
			default: ms = new MigrationStrategy(mi); break;
			}

			for (int run = run_start; run <= run_end; run++) {
				if (MPIInfo::Rank == 0) std::cout << topologyName << " " << EnumText::enumtext[mse] << " Instance: " << i << " MI: " << mi << " Run: " << run << std::endl;
				umda optimizer(mu, lm, literal_count, fks);
				OptimizerCoordinator oc{};
				oc.run(max_itr, *ms, optimizer, topology, [clause_count](const EDABase& eda) -> bool { return eda.best_fitness == clause_count; });

				//std::vector<std::pair<std::string, std::string>> details = make_details(
				//	literal_count, run, cL, cM, mi, "UMDA", "KSAT", topologyName, EnumText::enumtext[mse]
				//);

				//std::pair<std::string, std::string> instance_detail("instance", std::to_string(i));
				//details.push_back(instance_detail);

				//std::string _best_fitness = std::to_string(optimizer.best_fitness);
				//std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);
				//std::string filepath = make_filename(details, foldername);

				//auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, optimizer.history.size(), optimizer.best_fitness_itr);
				//json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);
			}
		}

	}
}


int main(int argc, char* argv[])
{
	MPIWrapper::mpi_init(argc, argv);
	//truncation = 1000000;
	truncation = 10000;
	runAmount = 16;
	runStart = 0;

	// parse args if any
	parse_args(argc, argv);
	//UMDAMaxSatSerielTest(args);
	//ErikPlayGround();
	//ResultsTests(0);
	//ResultsTests(1);
	//ResultsTests(2);
	//ResultsTests(3);
	//zipper();
	//ResultsTestsJump();
	cga_ResultsTestsJump();
	//LeadingOnesTest();
	//OneMaxSampleSpecificMiTest();
	//std::vector<std::string> args = parse_args(argc, argv);

	//CGA_MaxSat_Results_Test(args);
	//CGA_TwoPeak_ResultsTests(args);
	//std::vector<std::string> args = parse_args(argc, argv);
	//CGA_MaxSat_Results_Test(args);

	//std::vector<std::string> args = parse_args(argc, argv);
	//CGA_TwoPeak_ResultsTests(args);
	
	MPIWrapper::mpi_finalize();
}


