#pragma once

#include <algorithm>
#include <numeric>
#include <vector>
#include <iostream>
#include "../../Algorithms/UMDA/umda.h"
#include <fstream>
#include <mpi.h>
#include <string>
#include <sstream>
#include "../../JsonUtil/json_builder.h"
#include "../../JsonUtil/json_file_writer.h"
#include "../../Coordination/OptimizerCoordinator.h"
#include "../../Coordination/Migration/MigrationStrategy.h"
#include "../../Coordination/Migration/NullMigrationStrategy.h"
#include "../../Coordination/Migration/SampleMigrationStrategy.h"
#include "../../Coordination/Migration/CGAMuFromEachSampleMigrationStrategy.h"
#include "../../Coordination/Migration/ProbDistMigrationStrategy.h"
#include "../../Coordination/Communocation/MPIWrapper.h"
#include <filesystem>
#include "../../Algorithms/Utility/SatParser.h"
#include "../../Algorithms/Utility/DataTruncater.h"
#include <map>
#include "../../Algorithms/cGA/CGA.h"
#include "../../Experiments/CGA/CGAOneMax.h"
#include "CGAFunctions.h"
#include "../../AlgorithmsInfo.h"
#include "../../AlgorithmsIncludes.h"






void SerielcKTest_cga(FitnessClass& fitnessclass, int n, int maxItr) {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string foldername = "../Results/CGA/" + fitnessName + "/Ck/";
	std::filesystem::create_directories(foldername);

	for (int k = 0; k <= 400; k++) {
		for (int run = 0; run < 2; run++) {
			std::vector<std::pair<std::string, std::string>> details = cga_make_details_seriel(
				n, run, k,
				"CGA",
				fitnessName,
				"lg"
			);

			std::string filepath = cga_make_filename(details, foldername);
			cga_run_single(n, k, run, n, fitnessclass, details, filepath, 1, maxItr);
		}
	}
}

void CGA_NTest_Seriel(FitnessClass& fitnessclass, double cK, int maxN, int segments, std::string lg_type, int maxItr) {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string foldername = "../Results/CGA" + lg_type + fitnessName + "NTestSeriel";
	std::filesystem::create_directories(foldername);

	int segmentSize = maxN / segments;

	for (int n = segmentSize; n <= maxN; n += segmentSize) {
		
		int k = lg_type == "lg" ? std::log(n) * cK : std::sqrt(n) * std::log(n) * cK;

		for (int run = 0; run < 4; run++) {
			std::vector<std::pair<std::string, std::string>> details = cga_make_details_seriel(
				n, run, cK,
				"CGA",
				fitnessName,
				lg_type
			);
			std::string filepath = cga_make_filename(details, foldername);

			cga_run_single(n, k, run, n, fitnessclass, details, filepath, 1, maxItr);
			if (MPIInfo::Rank == 0) std::cout << "N TEST -> " << "lg type: " << lg_type << " N=" << n << " K=" << k << " FIT=" << fitnessName << std::endl;
		}
	}
}

void CGA_NTest(std::vector<std::string> args, FitnessClass& fitnessclass, double cK, Topology& topology, int maxN, int segments, MigrationStrategyEnum mse, int mi, std::string lg_type, int maxItr) {
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string topologyName = typeid(topology).name();
	topologyName = cga_nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);

	std::string foldername = "../Results/CGA/" + lg_type + "/" + fitnessName + "/" + topologyName + "/" + CGAEnumText::enumtext[mse] + "/NTest";
	std::filesystem::create_directories(foldername);

	int segmentSize = maxN / segments;

	for (int n = segmentSize; n <= maxN; n += segmentSize) {
		int k = lg_type == "lg" ? std::log(n) * cK : std::sqrt(n) * std::log(n) * cK;

		MigrationStrategy* ms;
		switch (mse) {
		case sample: { ms = new SampleMigrationStrategy(mi); break; }
		case musample: { ms = new CGAMuFromEachSampleMigrationStrategy(mi); break; }
		case prop: { ms = new ProbDistMigrationStrategy(0.9, mi); break; }
		case nomigration: { ms = new NullMigrationStrategy(); break; }
		default: ms = new MigrationStrategy(mi); break;
		}

		for (int run = run_start; run <= run_end; run++) {
			std::vector<std::pair<std::string, std::string>> details = cga_make_details(
				n, run, k, mi,
				"CGA",
				fitnessName,
				topologyName,
				lg_type,
				CGAEnumText::enumtext[mse]
			);
			std::string filepath = cga_make_filename(details, foldername);
			cga_run_multi(n, k, run, n, fitnessclass, details, filepath, topology, *ms, 1, maxItr);
			if (MPIInfo::Rank == 0) std::cout << "N TEST -> " << "lg type: " << lg_type << " N=" << n << " MI=" << mi << " MS=" << CGAEnumText::enumtext[mse] << " TOP=" << topologyName << " K=" << k << " FIT=" << fitnessName << " Run: " << run - run_start << "/" << run_end - run_start << std::endl;
		}
	}
}

void CGA_NTestRange(std::vector<std::string> args, FitnessClass& fitnessclass, double cK, Topology& topology, std::vector<int> ns, MigrationStrategyEnum mse, int mi, std::string lg_type, int maxItr) {
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string topologyName = typeid(topology).name();
	topologyName = cga_nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);

	std::string foldername = "../Results/CGA/" + lg_type + "/" + fitnessName + "/" + topologyName + "/" + CGAEnumText::enumtext[mse] + "/NTest";
	std::filesystem::create_directories(foldername);

	for (int n : ns) {
		int k = lg_type == "lg" ? std::log(n) * cK : std::sqrt(n) * std::log(n) * cK;

		MigrationStrategy* ms;
		switch (mse) {
		case sample: { ms = new SampleMigrationStrategy(mi); break; }
		case musample: { ms = new CGAMuFromEachSampleMigrationStrategy(mi); break; }
		case prop: { ms = new ProbDistMigrationStrategy(0.9, mi); break; }
		case nomigration: { ms = new NullMigrationStrategy(); break; }
		default: ms = new MigrationStrategy(mi); break;
		}

		for (int run = run_start; run <= run_end; run++) {
			std::vector<std::pair<std::string, std::string>> details = cga_make_details(
				n, run, k, mi,
				"CGA",
				fitnessName,
				topologyName,
				lg_type,
				CGAEnumText::enumtext[mse]
			);
			std::string filepath = cga_make_filename(details, foldername);
			cga_run_multi(n, k, run, n, fitnessclass, details, filepath, topology, *ms, 100, maxItr);
			if (MPIInfo::Rank == 0) std::cout << "N TEST -> " << "lg type: " << lg_type << " N=" << n << " MI=" << mi << " MS=" << CGAEnumText::enumtext[mse] << " TOP=" << topologyName << " K=" << k << " FIT=" << fitnessName << " Run: " << run - run_start << "/" << run_end - run_start << std::endl;
		}
	}
}

void CGA_MiTest(std::vector<std::string> args, FitnessClass& fitnessclass, int k, Topology& topology, int n, MigrationStrategyEnum mse, std::string lg_type, int maxItr = 100000) {
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string fitnessName = typeid(fitnessclass).name();
	std::string topologyName = typeid(topology).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	topologyName = cga_nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);
	std::string mst = "";
	std::string foldername = "../Results/CGA/" + lg_type + "/" + fitnessName + "/" + topologyName + "/" + CGAEnumText::enumtext[mse] + "/MiTest";
	std::filesystem::create_directories(foldername);
	
	for (int p = 0; p <= 20; p++) {
		int mi = std::pow(1.5, p);

		MigrationStrategy* ms;
		switch (mse) {
		case sample: { ms = new SampleMigrationStrategy(mi); break; }
		case musample: { ms = new CGAMuFromEachSampleMigrationStrategy(mi); break; }
		case prop: { ms = new ProbDistMigrationStrategy(0.9, mi); break; }
		case nomigration: { ms = new NullMigrationStrategy(); break; }
		default: ms = new MigrationStrategy(mi); break;
		}

		for (int run = run_start; run <= run_end; run++) {

			std::vector<std::pair<std::string, std::string>> details = cga_make_details(
				n, run, k, mi,
				"CGA",
				fitnessName,
				topologyName,
				lg_type,
				CGAEnumText::enumtext[mse]
			);
			std::string filepath = cga_make_filename(details, foldername);
			cga_run_multi(n, k, run, n, fitnessclass, details, filepath, topology, *ms, 1, maxItr);
			if (MPIInfo::Rank == 0) std::cout << "MI TEST -> " << "lg type" << lg_type << " K=" << k << " MI=" << mi << " MS=" << CGAEnumText::enumtext[mse] << " TOP=" << topologyName << " FIT=" << fitnessName << " Run: " << run - run_start << "/" << run_end - run_start << std::endl;
		}
	}
}

void CGA_NullTest(std::vector<std::string> args, FitnessClass& fitnessclass, int k, int n, int maxItr, std::string lg_type) {
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	CompleteTopology topology;

	std::string foldername = "../Results/CGA" + fitnessName + "NullTest_" + lg_type;
	std::filesystem::create_directories(foldername);

	for (int run = run_start; run <= run_end; run++) {
		std::vector<std::pair<std::string, std::string>> details = cga_make_details(
			n, run, k, 1,
			"UMDA",
			fitnessName,
			"Null", 
			lg_type,
			"Null"
		);
		NullMigrationStrategy ms;
		std::string filepath = cga_make_filename(details, foldername);
		cga_run_multi(n, k, run, n, fitnessclass, details, filepath, topology, ms, 1, maxItr);
		if (MPIInfo::Rank == 0) std::cout << "NULL TEST -> " << "lg type"  << lg_type << " K=" << k << " FIT=" << fitnessName << " Run: " << run - run_start << "/" << run_end - run_start << std::endl;
	}
}

//Used for cluster size test and if you wanna run something simple
void CGA_ClusterTest(FitnessClass& fitnessclass, int k, Topology& topology, int n, MigrationStrategyEnum mse, int mi, std::string lg_type, int maxItr) {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string topologyName = typeid(topology).name();
	topologyName = cga_nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);


	std::string foldername = "../Results/CGA/" + lg_type + "/" + fitnessName + "/" + topologyName + "/" + CGAEnumText::enumtext[mse] + "/ClusterSizeTest";
	std::filesystem::create_directories(foldername);

	MigrationStrategy* ms;
	switch (mse) {
	case sample: { ms = new SampleMigrationStrategy(mi); break; }
	case musample: { ms = new CGAMuFromEachSampleMigrationStrategy(mi); break; }
	case prop: { ms = new ProbDistMigrationStrategy(0.9, mi); break; }
	case nomigration: { ms = new NullMigrationStrategy(); break; }
	default: ms = new MigrationStrategy(mi); break;
	}

	for (int run = 0; run < 256; run++) {
		std::vector<std::pair<std::string, std::string>> details = cga_make_details(
			n, run, k, 1,
			"UMDA",
			fitnessName,
			topologyName, 
			lg_type,
			CGAEnumText::enumtext[mse]
		);
		std::string filepath = cga_make_filename(details, foldername);
		cga_run_multi(n, k, run, n, fitnessclass, details, filepath, topology, *ms, 1, maxItr);
	}
}

void cga_GeneticDriftAnalysisTest(std::vector<std::string> args) {
	CompleteTopology ctop;
	KCircleTopology circletop(1);
	StarTopology startop;
	int taurusSide = sqrt(MPIInfo::Size);
	TaurusTopology taurustop(taurusSide, taurusSide);

	MigrationStrategyEnum mses = MigrationStrategyEnum::sample;
	MigrationStrategyEnum msems = MigrationStrategyEnum::musample;
	MigrationStrategyEnum msep = MigrationStrategyEnum::prop;
	MigrationStrategyEnum msenm = MigrationStrategyEnum::nomigration;
	FitnessClass* fitness;// = new FitnessClass();

	fitness = new FitnessOneMax();

	int n = 1000;
	int k_log = 11;
	int k_sqrt_log = 132;
	int maxItr = 1000000;
	int nMax = 5000;
	int nSegments = 500;

	std::vector<int> MisLg = {
		1, 11, 129,
		1, 3, 57,
		1, 11, 291,
		2, 7, 86
	};
	std::vector<int> MisSqrtLg = {
		1, 3325, 25,
		1, 3325, 25,
		1, 194,  656,
		1, 3325, 17
	};

	double cK_log = k_log / (std::log(n));
	double cK_sqrt_log = k_sqrt_log / (std::sqrt(n) * std::log(n));

	double cK = cK_log;
	int k = k_log;
	std::string log_type = "lg";

	CGA_NTestRange(args, *fitness, cK, ctop, { 2300, 2400, 2500 }, msep, MisLg[2], log_type, maxItr);
	CGA_NTestRange(args, *fitness, cK, ctop, { 3600, 3800, 4000 }, msems, MisLg[1], log_type, maxItr);
	CGA_NTestRange(args, *fitness, cK, ctop, { 2300, 2400, 2500, 3600, 3800, 4000 }, msenm, -1, log_type, maxItr);
}

void cga_ResultsTests(std::vector<std::string> args, int fitnessIndex = 0, bool log_paradigm = true) {

	CompleteTopology ctop;
	KCircleTopology circletop(1);
	StarTopology startop;
	int taurusSide = sqrt(MPIInfo::Size);
	TaurusTopology taurustop(taurusSide, taurusSide);

	MigrationStrategyEnum mses = MigrationStrategyEnum::sample;
	MigrationStrategyEnum msems = MigrationStrategyEnum::musample;
	MigrationStrategyEnum msep = MigrationStrategyEnum::prop;
	FitnessClass* fitness = new FitnessLeadingOnes();
	int n, nMax, nSegments, k_log, k_sqrt_log, maxItr;
	double cK_log, cK_sqrt_log;
	std::vector<int> MisLg;
	std::vector<int> MisSqrtLg;
	std::vector<int> Mis;

	switch (fitnessIndex) {
		case 0:
		{
			fitness = new FitnessOneMax();
			n = 1000;
			k_log = 11;
			k_sqrt_log = 132;
			maxItr = 1000000;
			nMax = 5000;
			nSegments = 500;

			MisLg = {
				1, 11, 129,
				1, 3, 57,
				1, 11, 291,
				2, 7, 86
			};
			MisSqrtLg = {
				1, 3325, 25,
				1, 3325, 25,
				1, 194,  656,
				1, 3325, 17
			};

			break;
		}
		case 1:
		{
			fitness = new FitnessLeadingOnes();
			n = 100;
			k_log = 21;
			k_sqrt_log = 99;
			maxItr = 3000000;
			nMax = 500;
			nSegments = 50;

			MisSqrtLg = {
				1, 1, 129,
				1, 1, 57,
				1, 656, 985,
				1, 1, 129
			};
			break;
		}
	}

	cK_log = k_log / (std::log(n));
	cK_sqrt_log = k_sqrt_log / (std::sqrt(n) * std::log(n));

	double cK = log_paradigm ? cK_log : cK_sqrt_log;
	int k = log_paradigm ? k_log : k_sqrt_log;
	std::string log_type = log_paradigm ? "lg" : "sqrt_lg";
	Mis = log_paradigm ? MisLg : MisSqrtLg;

	//SerielcKTest_cga(*fitness, n, maxItr);

	/*CGA_NullTest(args, *fitness, k, n, maxItr, log_type);

	CGA_MiTest(args, *fitness, k, ctop, n, mses, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, ctop, n, msems, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, ctop, n, msep, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, circletop, n, mses, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, circletop, n, msems, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, circletop, n, msep, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, startop, n, mses, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, startop, n, msems, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, startop, n, msep, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, taurustop, n, mses, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, taurustop, n, msems, log_type, maxItr);
	CGA_MiTest(args, *fitness, k, taurustop, n, msep, log_type, maxItr);*/

	/*CGA_ClusterTest(*fitness, k, ctop, n, mses, Mis[0], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, ctop, n, msems, Mis[1], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, ctop, n, msep, Mis[2], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, circletop, n, mses, Mis[3], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, circletop, n, msems, Mis[4], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, circletop, n, msep, Mis[5], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, startop, n, mses, Mis[6], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, startop, n, msems, Mis[7], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, startop, n, msep, Mis[8], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, taurustop, n, mses, Mis[9], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, taurustop, n, msems, Mis[10], log_type, maxItr);
	CGA_ClusterTest(*fitness, k, taurustop, n, msep, Mis[11], log_type, maxItr);*/

	CGA_NTest_Seriel(*fitness, cK, nMax, nSegments, log_type, maxItr);

	/*CGA_NTest(args, *fitness, cK, ctop, nMax, nSegments, mses, Mis[0], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, ctop, nMax, nSegments, msems, Mis[1], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, ctop, nMax, nSegments, msep, Mis[2], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, circletop, nMax, nSegments, mses, Mis[3], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, circletop, nMax, nSegments, msems, Mis[4], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, circletop, nMax, nSegments, msep, Mis[5], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, startop, nMax, nSegments, mses, Mis[6], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, startop, nMax, nSegments, msems, Mis[7], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, startop, nMax, nSegments, msep, Mis[8], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, taurustop, nMax, nSegments, mses, Mis[9], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, taurustop, nMax, nSegments, msems, Mis[10], log_type, maxItr);
	CGA_NTest(args, *fitness, cK, taurustop, nMax, nSegments, msep, Mis[11], log_type, maxItr);*/
}