#pragma once
#include "../../AlgorithmsInfo.h"
#include "../../AlgorithmsIncludes.h"
#include "CGAFunctions.h"
#include <filesystem>

// -----------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------- LAMBDA TEST --------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------

void cga_SerielKTest(FitnessClass& fitnessclass, int n, int bestfitness, int maxItr, std::vector<std::pair<std::string, std::string>> extradetails, int from, int to, std::string extraPath = "") {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string path = "../Results/CGA/" + fitnessName + "/Lambda/" + extraPath + args[0];
	std::string foldername = path + "/";
	std::filesystem::create_directories(foldername);
	int delta = 5;

	int section = std::stoi(args[0]);
	int amountPerSection = 1;


	for (int k = from; k < to; k += delta) {
		for (int run = section * amountPerSection; run < (section + 1) * amountPerSection; run++) {
			std::vector<std::pair<std::string, std::string>> details = cga_make_details_seriel(
				n, run, k,
				"CGA",
				fitnessName,
				"N_LOG_N"
			);
			for (int i = 0; i < extradetails.size(); i++) {
				details.push_back(extradetails[i]);
			}
			std::string filepath = cga_make_filename(details, foldername);

			cga_run_single(n, k, run, bestfitness, fitnessclass, details, filepath, truncation, maxItr);
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
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (MPIInfo::Rank == 0) {
		std::string command2 = "rm -rf " + path;
		std::system(command2.c_str());
	}
}

void cga_SerielLambdaTest(FitnessClass& fitnessclass, int n, int bestfitness, int maxItr, int from, int to) {
	std::vector<std::pair<std::string, std::string>> noExtraDetails;
	cga_SerielKTest(fitnessclass, n, bestfitness, maxItr, noExtraDetails, from, to);
}

void cga_SerielLambdaTest(FitnessClass& fitnessclass, int n, int bestfitness, int from, int to) {
	cga_SerielLambdaTest(fitnessclass, n, bestfitness, 10000, from, to);
}

void cga_JumpK_MiTest(FitnessClass& fitnessclass, int k, Topology& topology, int n, int maxItr, MigrationStrategyEnum mse, std::vector<std::pair<std::string, std::string>> extradetails) {
	std::string fitnessName = typeid(fitnessclass).name();
	std::string topologyName = typeid(topology).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	topologyName = cga_nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);
	std::string mst = "";
	std::string path = "../Results/CGA/" + fitnessName + "/" + topologyName + "/" + EnumText::enumtext[mse] + "/MiTest/" + args[0];
	std::string foldername = path + "/";
	std::filesystem::create_directories(foldername);
	int oldmi = 0;
	for (int p = 0; p <= 20; p++) {
		int mi = std::pow(1.5, p);
		if (oldmi == mi) {
			continue;
		}
		oldmi = mi;
		//for (int p = 0; p <= 20; p++) {
		//	int mi = std::pow(1.5, p);
		MigrationStrategy* ms;
		switch (mse) {
		case sample: { ms = new SampleMigrationStrategy(mi); break; }
		case musample: { ms = new CGAMuFromEachSampleMigrationStrategy(mi); break; }
		case prop: { ms = new ProbDistMigrationStrategy(0.9, mi); break; }
		case nomigration: { ms = new NullMigrationStrategy(); break; }
		default: ms = new MigrationStrategy(mi); break;
		}

		int section = std::stoi(args[0]);
		for (int run = section * 16; run < (section + 1) * 16; run++) {

			std::vector<std::pair<std::string, std::string>> details = cga_make_details(
				n, run, k, mi,
				"CGA",
				fitnessName,
				topologyName,
				"N_LOG_N",
				CGAEnumText::enumtext[mse]
			);
			for (int i = 0; i < extradetails.size(); i++) {
				details.push_back(extradetails[i]);
			}
			std::string filepath = cga_make_filename(details, foldername);
			cga_run_multi(n, k, run, n, fitnessclass, details, filepath, topology, *ms, truncation, maxItr);

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
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (MPIInfo::Rank == 0) {
		std::string command2 = "rm -rf " + path;
		std::system(command2.c_str());
	}
	MPI_Barrier(MPI_COMM_WORLD);
}

void cga_Test(FitnessClass& fitnessclass, int k, Topology& topology, int n, int bestfitness, int maxItr, MigrationStrategyEnum mse, int mi, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {


	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);
	std::string topologyName = typeid(topology).name();
	topologyName = cga_nameCleaner(topologyName);
	topologyName = topologyName.substr(0, topologyName.length() - 8);

	std::string path = "../Results/CGA/" + fitnessName + "/" + topologyName + "/" + EnumText::enumtext[mse] + "/" + endfolder;
	std::string foldername = path + "/";
	std::filesystem::create_directories(foldername);


	MigrationStrategy* ms;
	switch (mse) {
	case sample: { ms = new SampleMigrationStrategy(mi); break; }
	case musample: { ms = new CGAMuFromEachSampleMigrationStrategy(mi); break; }
	case prop: { ms = new ProbDistMigrationStrategy(0.9, mi); break; }
	case nomigration: { ms = new NullMigrationStrategy(); break; }
	default: ms = new MigrationStrategy(mi); break;
	}

	int section = 0;
	if (args.size()>0) {
		section = std::stoi(args[0]);
	}
	for (int run = section * runAmount + runStart; run < (section + 1) * runAmount + runStart; run++) {
		if (MPIInfo::Rank == 0) {
			std::cout << "run " << run << std::endl;
		}

		std::vector<std::pair<std::string, std::string>> details = cga_make_details(
			n, run, k, mi,
			"CGA",
			fitnessName,
			topologyName,
			"N_LOG_N",
			CGAEnumText::enumtext[mse]
		);
		for (int i = 0; i < extradetails.size(); i++) {
			details.push_back(extradetails[i]);
		}
		std::string filepath = cga_make_filename(details, foldername);
		cga_run_multi(n, k, run, bestfitness, fitnessclass, details, filepath, topology, *ms, truncation, maxItr);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (MPIInfo::Rank == 0) {
		std::cout << "zipping" << std::endl;
		std::string command1 = "zip -q -u -r ../zips/" + zipName + ".zip " + path;
		if (first) {
			first = false;
			command1 = "zip -q -r ../zips/" + zipName + ".zip " + path;
		}
		std::cout << MPIInfo::Rank << command1 << std::endl;
		std::system(command1.c_str());
		std::string command2 = "rm -rf " + path + "/*";
		std::system(command2.c_str());
		std::string command3 = "rm -rf " + path;
		std::system(command3.c_str());
	}
	MPI_Barrier(MPI_COMM_WORLD);

}

void cga_TestSeriel(FitnessClass& fitnessclass, int k, int n, int bestfitness, int maxItr, std::vector<std::pair<std::string, std::string>> extradetails, std::string endfolder = "Test", std::string zipName = "Test") {
	std::string fitnessName = typeid(fitnessclass).name();
	fitnessName = cga_nameCleaner(fitnessName);
	fitnessName = fitnessName.substr(7);

	std::string path = "../Results/CGA/" + fitnessName + "/" + endfolder;
	std::string foldername = path + "/";
	std::filesystem::create_directories(foldername);

	int section = 0;
	if (args.size() > 0) {
		section = std::stoi(args[0]);
	}
	for (int run = section * runAmount + runStart; run < (section + 1) * runAmount + runStart; run++) {
		std::vector<std::pair<std::string, std::string>> details = cga_make_details_seriel(
			n, run, k,
			"CGA",
			fitnessName,
			"N_LOG_N"
		);
		for (int i = 0; i < extradetails.size(); i++) {
			details.push_back(extradetails[i]);
		}
		std::string filepath = cga_make_filename(details, foldername);

		cga_run_single(n, k, run, bestfitness, fitnessclass, details, filepath, truncation, maxItr);
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


void cga_ResultsTestsJump() {

	CompleteTopology ctop;
	KCircleTopology circletop(1);
	StarTopology startop;
	int taurusSide = sqrt(MPIInfo::Size);
	TaurusTopology taurustop(taurusSide, taurusSide);

	MigrationStrategyEnum mses = MigrationStrategyEnum::sample;
	MigrationStrategyEnum msems = MigrationStrategyEnum::musample;
	MigrationStrategyEnum msep = MigrationStrategyEnum::prop;
	FitnessClass* fitness;
	int n, cgak;
	std::vector<std::pair<std::string, std::string>> extraDetails;
	int Mis[12] = {
				1,1,1,
				1,1,1,
				1,1,1,
				1,1,1 };
	int mifrom = 1;
	int mito = 1000;
	double paradigmConst = 1;
	int paradigmType = LOGN;

	int jumpK = 4;

	fitness = new FitnessJumpk(jumpK);
	n = 100;
	cgak = 86;
	
	Mis[1] = 100;
	Mis[4] = 100;
	Mis[7] = 100;
	Mis[10] = 100;
	paradigmConst = cgak / log(n);
	paradigmType = LOGN;
	int maxItr = 200000;

	int originalFrom = 1;
	int originalTo = 501;

	//truncation = 1;
	//for (int k = 2; k <= 10; k++) {
	//	fitness = new FitnessJumpk(k);
	//	std::vector<std::pair<std::string, std::string>> localDetails;
	//	std::pair<std::string, std::string> jumpKValue("jumpK", std::to_string(k));
	//	localDetails.push_back(jumpKValue);
	//	cga_SerielKTest(*fitness, n, k, maxItr, localDetails, originalFrom, originalTo, "jumpK_" + std::to_string(k) + "/");
	//}

	//cga_JumpK_MiTest(*fitness, cgak, ctop, n, maxItr, mses, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, ctop, n, maxItr, msems, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, ctop, n, maxItr, msep, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, circletop, n, maxItr, mses, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, circletop, n, maxItr, msems, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, circletop, n, maxItr, msep, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, startop, n, maxItr, mses, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, startop, n, maxItr, msems, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, startop, n, maxItr, msep, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, taurustop, n, maxItr, mses, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, taurustop, n, maxItr, msems, extraDetails);
	//cga_JumpK_MiTest(*fitness, cgak, taurustop, n, maxItr, msep, extraDetails);


	std::string size = std::to_string(MPIInfo::Size);


	//cga_Test(*fitness,cgak, ctop, n, maxItr, nomigration, Mis[0], extraDetails, "cluster_test/" + size, "Null" + size);
	//cga_Test(*fitness, cgak, ctop, n, maxItr, mses, Mis[0], extraDetails, "cluster_test/" + size, "complete" + size);
	//cga_Test(*fitness, cgak, ctop, n, maxItr, msems, Mis[1], extraDetails, "cluster_test/" + size, "complete" + size);
	//cga_Test(*fitness, cgak, ctop, n, maxItr, msep, Mis[2], extraDetails, "cluster_test/" + size, "complete" + size);
	//cga_Test(*fitness, cgak, circletop, n, maxItr, mses, Mis[3], extraDetails, "cluster_test/" + size, "circle" + size);
	//cga_Test(*fitness, cgak, circletop, n, maxItr, msems, Mis[4], extraDetails, "cluster_test/" + size, "circle" + size);
	//cga_Test(*fitness, cgak, circletop, n, maxItr, msep, Mis[5], extraDetails, "cluster_test/" + size, "circle" + size);
	//cga_Test(*fitness, cgak, startop, n, maxItr, mses, Mis[6], extraDetails, "cluster_test/" + size, "star" + size);
	//cga_Test(*fitness, cgak, startop, n, maxItr, msems, Mis[7], extraDetails, "cluster_test/" + size, "star" + size);
	//cga_Test(*fitness, cgak, startop, n, maxItr, msep, Mis[8], extraDetails, "cluster_test/" + size, "star" + size);

	//cga_Test(*fitness, cgak, taurustop, n, maxItr, mses, Mis[9], extraDetails, "cluster_test/"+size, "taurus"+size);
	//cga_Test(*fitness, cgak, taurustop, n, maxItr, msems, Mis[10], extraDetails, "cluster_test/"+size, "taurus"+size);
	//cga_Test(*fitness, cgak, taurustop, n, maxItr, msep, Mis[11], extraDetails, "cluster_test/"+size, "taurus"+size);

	//for (int k = 2; k <= 10; k += 1) {
	//	fitness = new FitnessJumpk(k);
	//	std::vector<std::pair<std::string, std::string>> localDetails;
	//	std::pair<std::string, std::string> jumpKValue("jumpK", std::to_string(k));
	//	localDetails.push_back(jumpKValue);


	//	std::string testName = "jumpKTest/" + args[0];
	//	std::string zipName = args[0];

	//	cga_Test(*fitness, cgak, ctop, n, maxItr,nomigration, Mis[0], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, ctop, n, maxItr,mses, Mis[0], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, ctop, n, maxItr,msems, Mis[1], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, ctop, n, maxItr,msep, Mis[2], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, circletop, n, maxItr,mses, Mis[3], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, circletop, n, maxItr,msems, Mis[4], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, circletop, n, maxItr,msep, Mis[5], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, startop, n, maxItr,mses, Mis[6], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, startop, n, maxItr,msems, Mis[7], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, startop, n, maxItr,msep, Mis[8], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, taurustop, n, maxItr,mses, Mis[8], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, taurustop, n, maxItr,msems, Mis[8], localDetails, testName, zipName);
	//	cga_Test(*fitness, cgak, taurustop, n, maxItr,msep, Mis[8], localDetails, testName, zipName);
	//}
	 
	truncation = 1000000;
	std::string testName = "SpecificMiTest/" + args[0];
	std::string zipName = args[0];
	cga_TestSeriel(*fitness, cgak, n, n + jumpK, maxItr, extraDetails, testName, zipName);
	cga_Test(*fitness, cgak, ctop, n, n + jumpK, maxItr, nomigration, 1, extraDetails, testName, zipName);
	int testMis[10] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
	//int testMis[1] = { 250};
	for (int mi : testMis) {
			cga_Test(*fitness, cgak, ctop, n, n + jumpK, maxItr,mses, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, ctop, n, n + jumpK, maxItr,msems, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, ctop, n, n + jumpK, maxItr,msep, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, circletop, n, n + jumpK, maxItr,mses, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, circletop, n, n + jumpK, maxItr,msems, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, circletop, n, n + jumpK, maxItr,msep, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, startop, n, n + jumpK, maxItr,mses, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, startop, n, n + jumpK, maxItr,msems, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, startop, n, n + jumpK, maxItr,msep, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, taurustop, n, n + jumpK, maxItr,mses, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, taurustop, n, n + jumpK, maxItr,msems, mi, extraDetails, testName, zipName);
			cga_Test(*fitness, cgak, taurustop, n, n + jumpK, maxItr,msep, mi, extraDetails, testName, zipName);
	}
}