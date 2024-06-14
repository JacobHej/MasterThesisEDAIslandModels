#pragma once
#include "CGAShared.h"

void CGA_TwoPeak_SerielcKTest(FitnessClass& fitnessclass, int n, int maxItr) {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string foldername = "../Results/CGA/" + fitnessName + "/Ck/";
	std::filesystem::create_directories(foldername);

	for (int k = 0; k <= 200; k++) {
		for (int run = 0; run < 2; run++) {
			std::vector<std::pair<std::string, std::string>> details = cga_make_details_seriel(
				n, run, k,
				"CGA",
				fitnessName,
				"NA"
			);

			std::string filepath = cga_make_filename(details, foldername);
			cga_run_single(n, k, run, n, fitnessclass, details, filepath, 1, maxItr);
		}
	}
}

void CGA_TwoPeak_NTest_Seriel(FitnessClass& fitnessclass, double cK, int maxN, int segments, std::string lg_type, int maxItr) {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string foldername = "../Results/CGA/" + fitnessName + "/NTestSeriel";
	std::filesystem::create_directories(foldername);

	int segmentSize = maxN / segments;

	for (int n = segmentSize; n <= maxN; n += segmentSize) {

		int k = n * std::log(n) * cK;

		for (int run = 0; run < 4; run++) {
			std::vector<std::pair<std::string, std::string>> details = cga_make_details_seriel(
				n, run, cK,
				"CGA",
				fitnessName,
				"NA"
			);
			std::string filepath = cga_make_filename(details, foldername);

			cga_run_single(n, k, run, n, fitnessclass, details, filepath, 1, maxItr);
			if (MPIInfo::Rank == 0) std::cout << "N TEST -> " << "lg type: " << lg_type << " N=" << n << " K=" << k << " FIT=" << fitnessName << std::endl;
		}
	}
}

void CGA_TwoPeak_NTest(std::vector<std::string> args, FitnessClass& fitnessclass, double cK, Topology& topology, int maxN, int segments, MigrationStrategyEnum mse, int mi, std::string lg_type, int maxItr) {
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string topologyName = typeid(topology).name();
	topologyName = cga_nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);

	std::string foldername = "../Results/CGA/" + fitnessName + "/" + topologyName + "/" + CGAEnumText::enumtext[mse] + "/NTest";
	std::filesystem::create_directories(foldername);

	int segmentSize = maxN / segments;

	for (int n = segmentSize; n <= maxN; n += segmentSize) {
		int k = n * std::log(n) * cK;

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
				"NA",
				CGAEnumText::enumtext[mse]
			);
			std::string filepath = cga_make_filename(details, foldername);
			cga_run_multi(n, k, run, n, fitnessclass, details, filepath, topology, *ms, 1, maxItr);
			if (MPIInfo::Rank == 0) std::cout << "N TEST -> " << "lg type: " << lg_type << " N=" << n << " MI=" << mi << " MS=" << CGAEnumText::enumtext[mse] << " TOP=" << topologyName << " K=" << k << " FIT=" << fitnessName << " Run: " << run - run_start << "/" << run_end - run_start << std::endl;
		}
	}
}

void CGA_TwoPeak_MiTest(std::vector<std::string> args, FitnessClass& fitnessclass, int k, Topology& topology, int n, MigrationStrategyEnum mse, std::string lg_type, int maxItr = 100000) {
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string fitnessName = typeid(fitnessclass).name();
	std::string topologyName = typeid(topology).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	topologyName = cga_nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);
	std::string mst = "";
	std::string foldername = "../Results/CGA/" + fitnessName + "/" + topologyName + "/" + CGAEnumText::enumtext[mse] + "/MiTest";
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

void CGA_TwoPeak_NullTest(std::vector<std::string> args, FitnessClass& fitnessclass, int k, int n, int maxItr, std::string lg_type) {
	int run_start = std::stoi(args[0]);
	int run_end = std::stoi(args[1]);

	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	CompleteTopology topology;

	std::string foldername = "../Results/CGA/" + fitnessName + "/NullTest";
	std::filesystem::create_directories(foldername);

	for (int run = run_start; run <= run_end; run++) {
		std::vector<std::pair<std::string, std::string>> details = cga_make_details(
			n, run, k, 1,
			"UMDA",
			fitnessName,
			"Null",
			"NA",
			"Null"
		);
		NullMigrationStrategy ms;
		std::string filepath = cga_make_filename(details, foldername);
		cga_run_multi(n, k, run, n, fitnessclass, details, filepath, topology, ms, 1, maxItr);
		if (MPIInfo::Rank == 0) std::cout << "NULL TEST -> " << "lg type" << lg_type << " K=" << k << " FIT=" << fitnessName << " Run: " << run - run_start << "/" << run_end - run_start << std::endl;
	}
}

//Used for cluster size test and if you wanna run something simple
void CGA_TwoPeak_ClusterTest(FitnessClass& fitnessclass, int k, Topology& topology, int n, MigrationStrategyEnum mse, int mi, std::string lg_type, int maxItr) {
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
			n, run, k, mi,
			"CGA",
			fitnessName,
			topologyName,
			"NA",
			CGAEnumText::enumtext[mse]
		);
		std::string filepath = cga_make_filename(details, foldername);
		cga_run_multi(n, k, run, n, fitnessclass, details, filepath, topology, *ms, 1, maxItr);
	}
}


void CGA_TwoPeak_ResultsTests(std::vector<std::string> args) {

	CompleteTopology ctop;
	KCircleTopology circletop(1);
	StarTopology startop;
	int taurusSide = sqrt(MPIInfo::Size);
	TaurusTopology taurustop(taurusSide, taurusSide);

	MigrationStrategyEnum mses = MigrationStrategyEnum::sample;
	MigrationStrategyEnum msems = MigrationStrategyEnum::musample;
	MigrationStrategyEnum msep = MigrationStrategyEnum::prop;

	FitnessClass* fitness = new FitnessTwoPeaks();

	int n = 100;
	int k = 101;

	int maxItr = 1000000;

	int nMax = 200;
	int nSegments = 10;

	std::vector<int> Mis = {
		1, 2216, 1,
		1, 2216, 1,
		194, 2216, 437,
		1, 1, 437
	};

	double cK = k / (n * std::log(n));

	std::string log_type = "";

	//CGA_TwoPeak_SerielcKTest(*fitness, n, maxItr);

	//CGA_TwoPeak_NullTest(args, *fitness, k, n, maxItr, "");

	//CGA_TwoPeak_MiTest(args, *fitness, k, ctop, n, mses, "", maxItr);
	//CGA_TwoPeak_MiTest(args, *fitness, k, ctop, n, msems, "", maxItr);
	CGA_TwoPeak_MiTest(args, *fitness, k, ctop, n, msep, "", maxItr);
	//CGA_TwoPeak_MiTest(args, *fitness, k, circletop, n, mses, "", maxItr);
	//CGA_TwoPeak_MiTest(args, *fitness, k, circletop, n, msems, "", maxItr);
	CGA_TwoPeak_MiTest(args, *fitness, k, circletop, n, msep, "", maxItr);
	//CGA_TwoPeak_MiTest(args, *fitness, k, startop, n, mses, "", maxItr);
	//CGA_TwoPeak_MiTest(args, *fitness, k, startop, n, msems, "", maxItr);
	CGA_TwoPeak_MiTest(args, *fitness, k, startop, n, msep, "", maxItr);
	//CGA_TwoPeak_MiTest(args, *fitness, k, taurustop, n, mses, "", maxItr);
	//CGA_TwoPeak_MiTest(args, *fitness, k, taurustop, n, msems, "", maxItr);
	CGA_TwoPeak_MiTest(args, *fitness, k, taurustop, n, msep, "", maxItr);

	/*CGA_ClusterTest(*fitness, k, ctop, n, mses, Mis[0], log_type, maxItr)
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

	//CGA_TwoPeak_NTest_Seriel(*fitness, cK, nMax, nSegments, log_type, maxItr);

	////CGA_TwoPeak_NTest(args, *fitness, cK, ctop, nMax, nSegments, mses, Mis[0], log_type, maxItr);
	////CGA_TwoPeak_NTest(args, *fitness, cK, ctop, nMax, nSegments, msems, Mis[1], log_type, maxItr);
	//CGA_TwoPeak_NTest(args, *fitness, cK, ctop, nMax, nSegments, msep, Mis[2], log_type, maxItr);
	////CGA_TwoPeak_NTest(args, *fitness, cK, circletop, nMax, nSegments, mses, Mis[3], log_type, maxItr);
	////CGA_TwoPeak_NTest(args, *fitness, cK, circletop, nMax, nSegments, msems, Mis[4], log_type, maxItr);
	//CGA_TwoPeak_NTest(args, *fitness, cK, circletop, nMax, nSegments, msep, Mis[5], log_type, maxItr);
	////CGA_TwoPeak_NTest(args, *fitness, cK, startop, nMax, nSegments, mses, Mis[6], log_type, maxItr);
	////CGA_TwoPeak_NTest(args, *fitness, cK, startop, nMax, nSegments, msems, Mis[7], log_type, maxItr);
	//CGA_TwoPeak_NTest(args, *fitness, cK, startop, nMax, nSegments, msep, Mis[8], log_type, maxItr);
	////CGA_TwoPeak_NTest(args, *fitness, cK, taurustop, nMax, nSegments, mses, Mis[9], log_type, maxItr);
	////CGA_TwoPeak_NTest(args, *fitness, cK, taurustop, nMax, nSegments, msems, Mis[10], log_type, maxItr);
	//CGA_TwoPeak_NTest(args, *fitness, cK, taurustop, nMax, nSegments, msep, Mis[11], log_type, maxItr);
}