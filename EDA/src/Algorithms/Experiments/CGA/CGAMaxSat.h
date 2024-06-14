#pragma once
#include "CGAFunctions.h"
#include "../../AlgorithmsInfo.h"
#include <filesystem>
#include <map>



void CGAMaxSatMassiveTest(std::vector<std::string>& args) {
	int clause_count, literal_count;
	int instance_start = std::stoi(args[0]);
	int instance_end = std::stoi(args[1]);

	std::string sizes[] = { "20-91","50-218","75-325","100-430","125-538","150-645","175-753","200-860","225-960", "250-1065" };

	for (auto size : sizes) {
		std::string lit_cnt = size.substr(0, size.find_first_of('-'));

		for (int i = instance_start; i <= instance_end; i++) {
			std::string instance_file = (std::stringstream() << "../MaxSat/" << "uf" << size << "/" << "uf" << lit_cnt << "-0" << i << ".cnf").str();
			std::string foldername = "../Results/CGA/MaxSat/" + size;
			std::filesystem::create_directories(foldername);

			FitnessKSAT fks = SatParser::parse_maxsat_instance(instance_file, clause_count, literal_count);

			for (int k = 0; k <= 400; k++) {
				int max_itr = 2000000;

				CGA optimizer(literal_count, k, fks);
				optimizer.run_iterations(max_itr, [clause_count](const EDABase& eda) -> bool { return eda.best_fitness == clause_count; });

				std::vector<std::pair<std::string, std::string>> details = cga_make_details_seriel(
					literal_count, 0, k, "CGA", "KSAT", "Null"
				);

				std::pair<std::string, std::string> instance_detail("instance", std::to_string(i));
				details.push_back(instance_detail);

				std::string _best_fitness = std::to_string(optimizer.best_fitness);
				std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);
				std::string filepath = cga_make_filename(details, foldername);

				if (MPIInfo::Rank == 0) std::cout << " Instance: " << i << " Size: " << literal_count << " Itr: " << optimizer._itr_count << std::endl;

				auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, optimizer.history.size() / 1, optimizer.best_fitness_itr);
				json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);
			}
		}
	}
}



void CGAMaxSatMiTest(std::vector<std::string>& args, Topology& topology, MigrationStrategyEnum mse) {
	int clause_count, literal_count;
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string sizes[] = { "100-430" };
	int instances[] = { 4, 5, 2 };

	std::string topologyName = typeid(topology).name();
	topologyName = cga_nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);

	std::map<std::pair<int, int>, int> clcmMap = { // {{size, instance},{cL, cM}, ...}, ...
		{{100, 2}, 7},		{{100, 4}, 6},		{{100, 5}, 6}
	};

	for (auto size : sizes) {
		std::string lit_cnt = size.substr(0, size.find_first_of('-'));
		for (auto i : instances) {
			std::vector<int> mis = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536,131072, 262144 };;
			std::vector<int> misFinal;

			int topI = topologyName == "Complete" ? 0 : topologyName == "KCircle" ? 1 : topologyName == "Star" ? 2 : topologyName == "Taurus" ? 3 : -1;
			int stratI = -1;
			switch (mse) {
			case sample: { int stratI = 1; break; }
			case musample: { int stratI = 0; break; }
			case prop: { int stratI = 2; break; }
			case nomigration: { int stratI = -1000; break; }
			default: int stratI = -1; break;
			}
			int instI = i == 2 ? 0 : i == 4 ? 1 : i == 5 ? 2 : -100;
			int startIndexIndex = 9 * topI + 3 * stratI + instI;
			int startIndex[] = {
				//Complete
				//MuSample
				6, 2, 7, // instance 2 4 5
				//Sample
				6, 2, 6,
				//prob
				11, 6, 11,
				// KCircle
				5,0,6,
				5,0,5,
				11,6,10,
				//Star
				0,0,0,
				0,0,0,
				0,0,0,
				//Torus
				6,0,6,
				5,2,5,
				10,6,11
			};

			for (int j = startIndex[startIndexIndex]; j < mis.size(); j++) { misFinal.push_back(mis[j]); }

			std::string instance_file = (std::stringstream() << "../MaxSat/" << "uf" << size << "/" << "uf" << lit_cnt << "-0" << i << ".cnf").str();
			std::string foldername = "../ResultsMiTest/MaxSat/" + topologyName + "/" + EnumText::enumtext[mse] + "/MiTest";
			std::filesystem::create_directories(foldername);

			FitnessKSAT fks = SatParser::parse_maxsat_instance(instance_file, clause_count, literal_count);

			//Get cl cm
			int k;
			auto conf_it = clcmMap.find({ literal_count, i });
			if (conf_it != clcmMap.end()) { k = conf_it->second; }
			else { throw std::logic_error((std::stringstream() << "Configuration for (" << literal_count << ", " << i << ") not found.").str()); }

			int max_itr = 10000000;

			for (int mi : misFinal) {
				MigrationStrategy* ms;
				switch (mse) {
				case sample: { ms = new SampleMigrationStrategy(mi); break; }
				case musample: { ms = new CGAMuFromEachSampleMigrationStrategy(mi); break; }
				case prop: { ms = new ProbDistMigrationStrategy(0.9, mi); break; }
				case nomigration: { ms = new NullMigrationStrategy(); break; }
				default: ms = new MigrationStrategy(mi); break;
				}

				for (int run = run_start; run <= run_end; run++) {
					CGA optimizer(literal_count, k, fks);
					OptimizerCoordinator oc{};
					oc.run(max_itr, *ms, optimizer, topology, [clause_count](const EDABase& eda) -> bool { return eda.best_fitness == clause_count; });

					std::vector<std::pair<std::string, std::string>> details = cga_make_details(
						literal_count, run, k, mi, "CGA", "KSAT", topologyName, "NAN", EnumText::enumtext[mse]
					);

					std::pair<std::string, std::string> instance_detail("instance", std::to_string(i));
					details.push_back(instance_detail);

					std::string _best_fitness = std::to_string(optimizer.best_fitness);
					std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);
					std::string filepath = cga_make_filename(details, foldername);

					auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, optimizer.history.size() / 1, optimizer.best_fitness_itr);
					json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);

					if (MPIInfo::Rank == 0) std::cout << topologyName << " " << EnumText::enumtext[mse] << " Instance: " << i << " MI: " << mi << " RUN: " << run << std::endl;
				}
			}
		}
	}
}



void CGAMaxSatNoMigrationTest(std::vector<std::string>& args) {
	int clause_count, literal_count;
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string sizes[] = { "100-430" };
	int instances[] = { 4, 5, 2 };

	std::map<std::pair<int, int>, int> clcmMap = { // {{size, instance},{cL, cM}, ...}, ...
		{{100, 2}, 7},		{{100, 4}, 6},		{{100, 5}, 6}
	};

	for (auto size : sizes) {
		std::string lit_cnt = size.substr(0, size.find_first_of('-'));
		for (auto i : instances) {
			std::string instance_file = (std::stringstream() << "../MaxSat/" << "uf" << size << "/" << "uf" << lit_cnt << "-0" << i << ".cnf").str();
			std::string foldername = "../ResultsMiTest/MaxSat/Complete/NoMigration/MiTest";
			std::filesystem::create_directories(foldername);

			FitnessKSAT fks = SatParser::parse_maxsat_instance(instance_file, clause_count, literal_count);

			//Get cl cm
			int k;
			auto conf_it = clcmMap.find({ literal_count, i });
			if (conf_it != clcmMap.end()) { k = conf_it->second; }
			else { throw std::logic_error((std::stringstream() << "Configuration for (" << literal_count << ", " << i << ") not found.").str()); }

			int max_itr = 10000000;

			MigrationStrategy* ms = new NullMigrationStrategy();
			CompleteTopology topology;

			for (int run = run_start; run <= run_end; run++) {
				CGA optimizer(literal_count, k, fks);
				OptimizerCoordinator oc{};
				oc.run(max_itr, *ms, optimizer, topology, [clause_count](const EDABase& eda) -> bool { return eda.best_fitness == clause_count; });

				std::vector<std::pair<std::string, std::string>> details = cga_make_details(
					literal_count, run, k, 1, "CGA", "KSAT", "NA", "NAN", "NA"
				);

				std::pair<std::string, std::string> instance_detail("instance", std::to_string(i));
				details.push_back(instance_detail);

				std::string _best_fitness = std::to_string(optimizer.best_fitness);
				std::string best_fitness_itr = std::to_string(optimizer.best_fitness_itr);
				std::string filepath = cga_make_filename(details, foldername);

				auto truncated_history = DataTruncater::distributed_truncate_history(optimizer.history, optimizer.history.size() / 1, optimizer.best_fitness_itr);
				json_file_writer::write_history(details, _best_fitness, best_fitness_itr, truncated_history, filepath);

				if (MPIInfo::Rank == 0) std::cout << " Instance: " << i << " MI: " << 1 << " RUN: " << run << std::endl;
			}
		}
	}
}

void CGA_MaxSat_Results_Test(std::vector<std::string> args) {
	CompleteTopology ctop;
	KCircleTopology circletop(1);
	StarTopology startop;
	int taurusSide = sqrt(MPIInfo::Size);
	TaurusTopology taurustop(taurusSide, taurusSide);

	MigrationStrategyEnum mses = MigrationStrategyEnum::sample;
	MigrationStrategyEnum msems = MigrationStrategyEnum::musample;
	MigrationStrategyEnum msep = MigrationStrategyEnum::prop;
	MigrationStrategyEnum msenm = MigrationStrategyEnum::nomigration;

	//CGAMaxSatMassiveTest(args);

	//CGAMaxSatNoMigrationTest(args);

	//CGAMaxSatMiTest(args, ctop, mses);
	//CGAMaxSatMiTest(args, ctop, msems);
	//CGAMaxSatMiTest(args, ctop, msep);

	//CGAMaxSatMiTest(args, circletop, mses);
	//CGAMaxSatMiTest(args, circletop, msems);
	//CGAMaxSatMiTest(args, circletop, msep);

	CGAMaxSatMiTest(args, startop, mses);
	CGAMaxSatMiTest(args, startop, msems);
	CGAMaxSatMiTest(args, startop, msep);

	CGAMaxSatMiTest(args, taurustop, mses);
	CGAMaxSatMiTest(args, taurustop, msems);
	CGAMaxSatMiTest(args, taurustop, msep);
}