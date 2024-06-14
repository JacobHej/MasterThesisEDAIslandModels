#include "pch.h"
#include "../Algorithms/Fitness/fitness_functions.h"
#include "../Algorithms/Algorithms/UMDA/umda.h"
#include "../Algorithms/Algorithms/UMDA/umda.cpp"
#include "../Algorithms/Algorithms/UMDA/umda.h"
#include <algorithm>
#include "../Algorithms/Algorithms/Utility/SatParser.h"
#include "../Algorithms/Algorithms/Utility/DataTruncater.h"
#include "../Algorithms/Algorithms/DataObjects/iteration_snapshot.h"
#include <filesystem>
#include <string>
#include <iostream>
#include "../Algorithms/Algorithms/cGA/CGA.h"

void printArr(DynamicArray<uint16_t>& arr) {
	for (int i = 0; i < arr.size(); i++) {
		std::cout << arr[i] << ", ";
	}
	std::cout << std::endl;
}

template <typename T>
DynamicArray<T> da(std::vector<T> in) {
	DynamicArray<T> arr(in.size());
	for (int i = 0; i < in.size(); i++) {
		arr[i] = in[i];
	}
	//printArr(arr);
	return arr;
}

std::unique_ptr<uint16_t[]> up(std::vector<int> in) {
	auto arr = std::make_unique<uint16_t[]>(in.size());
	for (int i = 0; i < in.size(); i++) {
		arr[i] = in[i];
	}
	return arr;
}

SampleContainer sc(std::vector<int> in, FitnessClass& fitnessclass) {
	SampleContainer sa(4, 4);
	for (int i = 0; i < in.size(); i++) {
		sa.samples[i] = in[i];
	}

	for (int j = 0; j < sa.sample_count; j++) {
		sa.sample_fitnesses[j] = fitnessclass.fitness(sa.samples, j * sa.sample_size, sa.sample_size);
	}
	return sa;
}

SampleContainer sc(std::vector<int> in, FitnessClass& fitnessclass, int sample_size, int sample_count) {
	SampleContainer sa(sample_count, sample_size);
	for (int i = 0; i < in.size(); i++) {
		sa.samples[i] = in[i];
	}

	for (int j = 0; j < sa.sample_count; j++) {
		sa.sample_fitnesses[j] = fitnessclass.fitness(sa.samples, j * sa.sample_size, sa.sample_size);
	}
	return sa;
}

void sample(int _n, int _lambda, const std::unique_ptr<uint16_t[]>& samples, EDABase& optimizer) {
	int start = (_n / 4) * 4;
	int rest = _n - start;

	for (int j = 0; j < _lambda; j++) {
		for (int k = 0; k < _n - 3; k += 4) {
			int index = j * _n + k;
			samples[index]     = optimizer._rng_buffer[index] < optimizer.probability_distribution[k];
			samples[index + 1] = optimizer._rng_buffer[index + 1] < optimizer.probability_distribution[k + 1];
			samples[index + 2] = optimizer._rng_buffer[index + 2] < optimizer.probability_distribution[k + 2];
			samples[index + 3] = optimizer._rng_buffer[index + 3] < optimizer.probability_distribution[k + 3];
		}

		for (int k = start; k < start + rest; k++) {
			samples[j * _n + k] = optimizer._rng_buffer[j * _n + k] < optimizer.probability_distribution[k];
		}
	}
}

uint16_t weightedAvg(DynamicArray<uint16_t>& vals, double weight) {
	uint16_t res = (std::accumulate(vals.get(), vals.get() + vals.size(), 0) / vals.size()) * weight;
	//printArr(vals); std::cout << weight << std::endl << res << std::endl << std::endl;
	return res;
}

FitnessKSAT ParseTestProblem(int& out_clause_count, int& out_literal_count) {
	return SatParser::parse_maxsat_instance("uf20-0996.cnf", out_clause_count, out_literal_count);
}

#pragma region Sat parsing Tests

void AssertCorrectParsing(std::string directory_path, int exp_literal_count, int exp_clause_count) {
	int clause_count, literal_count;

	for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(directory_path)) {
		FitnessKSAT fks = SatParser::parse_maxsat_instance(file.path().string(), clause_count, literal_count);

		EXPECT_EQ(clause_count, exp_clause_count);
		EXPECT_EQ(literal_count, exp_literal_count);
		EXPECT_EQ(fks.sat_clauses.size(), exp_clause_count);
	}
}

//TEST(SatParsing, Parse_all_probs_20_91) {
//	AssertCorrectParsing("../../../MaxSat/uf20-91", 20, 91);
//}
//
//TEST(SatParsing, Parse_all_probs_50_218) {
//	AssertCorrectParsing("../../../MaxSat/uf50-218", 50, 218);
//}
//
//TEST(SatParsing, Parse_all_probs_75_325) {
//	AssertCorrectParsing("../../../MaxSat/uf75-325", 75, 325);
//}
//
//TEST(SatParsing, Parse_all_probs_100_430) {
//	AssertCorrectParsing("../../../MaxSat/uf100-430", 100, 430);
//}
//
//TEST(SatParsing, Parse_all_probs_125_538) {
//	AssertCorrectParsing("../../../MaxSat/uf125-538", 125, 538);
//}


TEST(SatParsing, Parse_test_problem) {
	int clause_count;
	int literal_count;

	FitnessKSAT fit = ParseTestProblem(clause_count, literal_count);
	EXPECT_EQ(clause_count, 5);
	EXPECT_EQ(literal_count, 10);
	EXPECT_EQ(fit.sat_clauses.size(), 5);


	// check clause 1
	auto clause = fit.sat_clauses[0];
	// check signs
	EXPECT_EQ(clause.second[0], 1);
	EXPECT_EQ(clause.second[1], 1);
	EXPECT_EQ(clause.second[2], 1);
	// check literals
	EXPECT_EQ(clause.first[0], 0);
	EXPECT_EQ(clause.first[1], 1);
	EXPECT_EQ(clause.first[2], 2);

	// check clause 2
	clause = fit.sat_clauses[1];
	// check signs
	EXPECT_EQ(clause.second[0], 0);
	EXPECT_EQ(clause.second[1], 0);
	EXPECT_EQ(clause.second[2], 0);
	// check literals
	EXPECT_EQ(clause.first[0], 0);
	EXPECT_EQ(clause.first[1], 1);
	EXPECT_EQ(clause.first[2], 2);

	// check clause 3
	clause = fit.sat_clauses[2];
	// check signs
	EXPECT_EQ(clause.second[0], 0);
	EXPECT_EQ(clause.second[1], 1);
	EXPECT_EQ(clause.second[2], 1);
	// check literals
	EXPECT_EQ(clause.first[0], 3);
	EXPECT_EQ(clause.first[1], 4);
	EXPECT_EQ(clause.first[2], 5);

	// check clause 4
	clause = fit.sat_clauses[3];
	// check signs
	EXPECT_EQ(clause.second[0], 0);
	EXPECT_EQ(clause.second[1], 1);
	EXPECT_EQ(clause.second[2], 1);
	// check literals
	EXPECT_EQ(clause.first[0], 5);
	EXPECT_EQ(clause.first[1], 6);
	EXPECT_EQ(clause.first[2], 7);

	// check clause 4
	clause = fit.sat_clauses[4];
	// check signs
	EXPECT_EQ(clause.second[0], 0);
	EXPECT_EQ(clause.second[1], 1);
	EXPECT_EQ(clause.second[2], 1);
	// check literals
	EXPECT_EQ(clause.first[0], 7);
	EXPECT_EQ(clause.first[1], 8);
	EXPECT_EQ(clause.first[2], 9);
}
#pragma endregion


#pragma region Migration Tests

TEST(Migration, ProbDist75) {
	FitnessOneMax x;
	umda optimizer(1, 1, 6, x);

	uint16_t lower = (0xffff * (1.0 / 4));
	uint16_t mid = (0xffff * (1.0 / 2));
	uint16_t upper = (0xffff * (3.0 / 4));
	double beta = 0.75;

	DynamicArray<uint16_t> probDistExternal = da<uint16_t>({ upper, mid,   lower, mid,   upper, mid });
	probDistExternal.concat(da<uint16_t>({ lower, upper, lower, lower, upper, mid }));

	// initial prob dist is all mid
	optimizer.combine_probability_distributions(probDistExternal, beta);
	EXPECT_EQ(optimizer.probability_distribution[0],
		weightedAvg(da<uint16_t>({ upper, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));
	EXPECT_EQ(optimizer.probability_distribution[1],
		weightedAvg(da<uint16_t>({ mid, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));
	EXPECT_EQ(optimizer.probability_distribution[2],
		weightedAvg(da<uint16_t>({ lower, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));
	EXPECT_EQ(optimizer.probability_distribution[3],
		weightedAvg(da<uint16_t>({ mid, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));
	EXPECT_EQ(optimizer.probability_distribution[4],
		weightedAvg(da<uint16_t>({ upper, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));
	EXPECT_EQ(optimizer.probability_distribution[5],
		weightedAvg(da<uint16_t>({ mid, mid }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));

	// initial prob dist is all lower
	for (int i = 0; i < 6; i++) optimizer.probability_distribution[i] = lower;
	optimizer.combine_probability_distributions(probDistExternal, beta);
	EXPECT_EQ(optimizer.probability_distribution[0],
		weightedAvg(da<uint16_t>({ upper, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));
	EXPECT_EQ(optimizer.probability_distribution[1],
		weightedAvg(da<uint16_t>({ mid, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));
	EXPECT_EQ(optimizer.probability_distribution[2],
		weightedAvg(da<uint16_t>({ lower, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));
	EXPECT_EQ(optimizer.probability_distribution[3],
		weightedAvg(da<uint16_t>({ mid, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));
	EXPECT_EQ(optimizer.probability_distribution[4],
		weightedAvg(da<uint16_t>({ upper, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));
	EXPECT_EQ(optimizer.probability_distribution[5],
		weightedAvg(da<uint16_t>({ mid, mid }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));

	// initial prob dist is all upper
	for (int i = 0; i < 6; i++) optimizer.probability_distribution[i] = upper;
	optimizer.combine_probability_distributions(probDistExternal, beta);
	EXPECT_EQ(optimizer.probability_distribution[0],
		weightedAvg(da<uint16_t>({ upper, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
	EXPECT_EQ(optimizer.probability_distribution[1],
		weightedAvg(da<uint16_t>({ mid, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
	EXPECT_EQ(optimizer.probability_distribution[2],
		weightedAvg(da<uint16_t>({ lower, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
	EXPECT_EQ(optimizer.probability_distribution[3],
		weightedAvg(da<uint16_t>({ mid, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
	EXPECT_EQ(optimizer.probability_distribution[4],
		weightedAvg(da<uint16_t>({ upper, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
	EXPECT_EQ(optimizer.probability_distribution[5],
		weightedAvg(da<uint16_t>({ mid, mid }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
}

TEST(Migration, ProbDist05) {
	FitnessOneMax x;
	umda optimizer(1, 1, 6, x);

	uint16_t lower = (0xffff * (1.0 / 4));
	uint16_t mid = (0xffff * (1.0 / 2));
	uint16_t upper = (0xffff * (3.0 / 4));
	double beta = 0.5;

	DynamicArray<uint16_t> probDistExternal = da<uint16_t>({upper, mid,   lower, mid,   upper, mid});
	probDistExternal.concat(da<uint16_t>({				  lower, upper, lower, lower, upper, mid }));

	// initial prob dist is all mid
	optimizer.combine_probability_distributions(probDistExternal, beta);
	EXPECT_EQ(optimizer.probability_distribution[0],
		weightedAvg(da<uint16_t>({ upper, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));
	EXPECT_EQ(optimizer.probability_distribution[1],
		weightedAvg(da<uint16_t>({ mid, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));
	EXPECT_EQ(optimizer.probability_distribution[2],
		weightedAvg(da<uint16_t>({ lower, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));
	EXPECT_EQ(optimizer.probability_distribution[3],
		weightedAvg(da<uint16_t>({ mid, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));
	EXPECT_EQ(optimizer.probability_distribution[4],
		weightedAvg(da<uint16_t>({ upper, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));
	EXPECT_EQ(optimizer.probability_distribution[5],
		weightedAvg(da<uint16_t>({ mid, mid }), 1 - beta) + weightedAvg(da<uint16_t>({ mid }), beta));

	// initial prob dist is all lower
	for (int i = 0; i < 6; i++) optimizer.probability_distribution[i] = lower;
	optimizer.combine_probability_distributions(probDistExternal, beta);
	EXPECT_EQ(optimizer.probability_distribution[0],
		weightedAvg(da<uint16_t>({ upper, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));
	EXPECT_EQ(optimizer.probability_distribution[1],
		weightedAvg(da<uint16_t>({ mid, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));
	EXPECT_EQ(optimizer.probability_distribution[2],
		weightedAvg(da<uint16_t>({ lower, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));
	EXPECT_EQ(optimizer.probability_distribution[3],
		weightedAvg(da<uint16_t>({ mid, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));
	EXPECT_EQ(optimizer.probability_distribution[4],
		weightedAvg(da<uint16_t>({ upper, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));
	EXPECT_EQ(optimizer.probability_distribution[5],
		weightedAvg(da<uint16_t>({ mid, mid }), 1 - beta) + weightedAvg(da<uint16_t>({ lower }), beta));

	// initial prob dist is all upper
	for (int i = 0; i < 6; i++) optimizer.probability_distribution[i] = upper;
	optimizer.combine_probability_distributions(probDistExternal, beta);
	EXPECT_EQ(optimizer.probability_distribution[0],
		weightedAvg(da<uint16_t>({ upper, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
	EXPECT_EQ(optimizer.probability_distribution[1],
		weightedAvg(da<uint16_t>({ mid, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
	EXPECT_EQ(optimizer.probability_distribution[2],
		weightedAvg(da<uint16_t>({ lower, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
	EXPECT_EQ(optimizer.probability_distribution[3],
		weightedAvg(da<uint16_t>({ mid, lower }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
	EXPECT_EQ(optimizer.probability_distribution[4],
		weightedAvg(da<uint16_t>({ upper, upper }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
	EXPECT_EQ(optimizer.probability_distribution[5],
		weightedAvg(da<uint16_t>({ mid, mid }), 1 - beta) + weightedAvg(da<uint16_t>({ upper }), beta));
}

#pragma endregion

#pragma region Sampling tests

TEST(EDABase, sampling) {
	FitnessOneMax x;
	umda optimizer(20, 50, 50, x);
	auto samples = std::make_unique<uint16_t[]>(50 * 50);

	for (int j = 0; j < 10; j++) {
		optimizer.sample();
		sample(50, 50, samples, optimizer);

		for (int i = 0; i < 50 * 50; i++)  EXPECT_EQ(optimizer.sample_container.samples[i], samples[i], );		

		optimizer.run_iterations(5);
	}
}

#pragma endregion

#pragma region Fitness tests

TEST(OneMax, BaseCases) {
	FitnessOneMax x;
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,0,0 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,0,1 }), 0, 5), 1);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,0,0,0,0 }), 0, 5), 1);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,1,1,1,0 }), 0, 5), 3);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,1,1,1 }), 0, 5), 5);
}

TEST(LeadingOnes, BaseCases) {
	FitnessLeadingOnes x;

	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,0,0 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,1,1,1,1 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,0,0,0,0 }), 0, 5), 1);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,0,1,1 }), 0, 5), 2);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,1,1,1 }), 0, 5), 5);
}

TEST(FlipFlop, BaseCases) {
	FitnessFlipFlop x;

	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,0,0 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,1,1,1 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,1,1,1,1 }), 0, 5), 1);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,0,0,0,0 }), 0, 5), 1);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,0,1,1 }), 0, 5), 2);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,0,1,0,1 }), 0, 5), 4);
}

TEST(Needle, BaseCases) {

	FitnessNeedle x(up({ 0,1,0,1,1 }));

	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,0,0 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,1,1,1 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,1,0,1,0 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,0,1,0,1 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,1,0,1,1 }), 0, 5), 5);
}

TEST(Jumpk, BaseCases) {
	FitnessJumpk x(2);

	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,0,0 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,1,0,0 }), 0, 5), 1);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,1,1,0 }), 0, 5), 2);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,1,1,1,0 }), 0, 5), 3);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,1,1,1,1 }), 0, 5), 1);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,1,1,1 }), 0, 5), 5);
}

TEST(KSAT, BaseCases) {
	int clause_count;
	int literal_count;

	FitnessKSAT x = ParseTestProblem(clause_count, literal_count);

	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,0,0,0,0,0,0,0 }), 0, 10), 4);
	//						 1 2 3 4 5 6 7 8 9 10
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,1,1,1,1,1,1,1,1 }), 0, 10), 4);
	//						 1 2 3 4 5 6 7 8 9 10
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,0,0,1,0,1,0,1,0,0 }), 0, 10), 5);
	//						 1 2 3 4 5 6 7 8 9 10
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,1,0,1,1,1,1,0,0 }), 0, 10), 3);
	//						 1 2 3 4 5 6 7 8 9 10
}

TEST(TwoPeaks, BaseCases) {
	FitnessTwoPeaks x;

	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,1,1,1 }), 0, 6), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,1,0,1,0,1 }), 0, 6), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,1,1,1,1 }), 0, 6), 6);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,0,0,0 }), 0, 6), 6);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,1,1,0,0 }), 0, 6), 4);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,0,0,0,0 }), 0, 6), 4);
}

TEST(Knapsack, BaseCases) {
	FitnessKnapsack x({1,2,4,8,16},30);

	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,0,0 }), 0, 5), 0);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,0,0,0,0 }), 0, 5), 1);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,1,0,0,0 }), 0, 5), 2);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,1,0,0 }), 0, 5), 4);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,1,0 }), 0, 5), 8);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,0,0,0,1 }), 0, 5), 16);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 0,1,1,1,1 }), 0, 5), 30);
	EXPECT_EQ(x.fitness(da<uint16_t>({ 1,1,1,1,1 }), 0, 5), 0);
}

TEST(TestCaseName, TestName) {
	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}

#pragma endregion

#pragma region Update prob tests
TEST(CGATest, UpdateProp) {
	FitnessOneMax x;
	CGA* optimiser = new CGA(4, 1, x);
	uint16_t lower = (0xffff * (1.0 / 4));
	uint16_t mid = (0xffff * (1.0 / 2));
	uint16_t upper = (0xffff * (3.0 / 4));
	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(4, 1, x);
	(*optimiser).update_probability_distribution(sc({
		1,1,1,1,
		1,1,1,1,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(4, 1, x);
	(*optimiser).update_probability_distribution(sc({
		1,0,1,0,
		1,0,1,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);



	optimiser = new CGA(4, 1, x);
	(*optimiser).update_probability_distribution(sc({
		1,1,1,1,
		0,0,0,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], upper);
	EXPECT_EQ((*optimiser).probability_distribution[1], upper);
	EXPECT_EQ((*optimiser).probability_distribution[2], upper);
	EXPECT_EQ((*optimiser).probability_distribution[3], upper);

	optimiser = new CGA(4, 1, x);
	(*optimiser).update_probability_distribution(sc({
		1,0,1,1,
		0,1,0,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], upper);
	EXPECT_EQ((*optimiser).probability_distribution[1], lower);
	EXPECT_EQ((*optimiser).probability_distribution[2], upper);
	EXPECT_EQ((*optimiser).probability_distribution[3], upper);

	optimiser = new CGA(4, 1, x);
	(*optimiser).update_probability_distribution(sc({
		1,0,1,0,
		1,1,1,1,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], upper);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], upper);
}
TEST(CGATest, UpdatePropKVar) {
	FitnessOneMax x;
	CGA* optimiser = new CGA(4, 2, x);
	uint16_t lower = (0xffff * (1.0 / 4));
	uint16_t mid = (0xffff * (1.0 / 2));
	uint16_t upper = (0xffff * (3.0 / 4));
	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(4, 2, x);
	(*optimiser).update_probability_distribution(sc({
		1,1,1,1,
		1,1,1,1,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(4, 2, x);
	(*optimiser).update_probability_distribution(sc({
		1,0,1,0,
		1,0,1,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);



	optimiser = new CGA(4, 2, x);
	(*optimiser).update_probability_distribution(sc({
		1,1,1,1,
		0,0,0,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], upper);
	EXPECT_EQ((*optimiser).probability_distribution[1], upper);
	EXPECT_EQ((*optimiser).probability_distribution[2], upper);
	EXPECT_EQ((*optimiser).probability_distribution[3], upper);

	optimiser = new CGA(4, 2, x);
	(*optimiser).update_probability_distribution(sc({
		1,0,1,1,
		0,1,0,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], upper);
	EXPECT_EQ((*optimiser).probability_distribution[1], lower);
	EXPECT_EQ((*optimiser).probability_distribution[2], upper);
	EXPECT_EQ((*optimiser).probability_distribution[3], upper);

	optimiser = new CGA(4, 2, x);
	(*optimiser).update_probability_distribution(sc({
		1,0,1,0,
		1,1,1,1,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], upper);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], upper);
}


TEST(CGATest, UpdatePropKVar2) {
	FitnessOneMax x;
	CGA* optimiser = new CGA(4, 10, x);
	uint16_t lower = (0xffff * (1.0 / 4));
	uint16_t mid = (0xffff * (1.0 / 2));
	uint16_t midP10th = mid + (0xffff * (1 / 10.0)); // mid plus 1/10
	uint16_t utenth = 0xffff * (1 / 10.0);
	uint16_t midM10th = mid - utenth; // mid minus 1/10
	uint16_t upper = (0xffff * (3.0 / 4));
	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(4, 10, x);
	(*optimiser).update_probability_distribution(sc({
		1,1,1,1,
		1,1,1,1,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(4, 10, x);
	(*optimiser).update_probability_distribution(sc({
		1,0,1,0,
		1,0,1,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(4, 10, x);


	(*optimiser).update_probability_distribution(sc({
		1,1,1,1,
		0,0,0,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[1], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[2], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP10th);

	optimiser = new CGA(4, 10, x);
	(*optimiser).update_probability_distribution(sc({
		1,0,1,1,
		0,1,0,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[1], midM10th);
	EXPECT_EQ((*optimiser).probability_distribution[2], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP10th);

	optimiser = new CGA(4, 10, x);
	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		1,1,1,1,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[1], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[2], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP10th);

	optimiser = new CGA(4, 10, x);
	(*optimiser).update_probability_distribution(sc({
		1,0,1,0,
		1,1,1,1,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP10th);
}

TEST(CGATest, UpdatePropKVar3) {
	FitnessOneMax x;
	CGA* optimiser = new CGA(4, 10, x);
	uint16_t lower = (0xffff * (1.0 / 4));
	uint16_t mid = (0xffff * (1.0 / 2));
	uint16_t midP10th = mid + (0xffff * (1.0 / 10)); // mid plus 1/10
	uint16_t utenth = 0xffff * (1 / 10.0);
	uint16_t midM10th = mid - utenth; // mid minus 1/10
	uint16_t upper = (0xffff * (3.0 / 4));
	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		}, x));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(4, 10, x);
	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		1,1,1,1,
		1,1,1,1,
		0,0,0,0,
		0,0,0,0,
		}, x, 4, 6));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(4, 10, x);
	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		1,0,1,0,
		1,0,1,0,
		0,0,0,0,
		0,0,0,0,
		}, x, 4, 6));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(4, 10, x);


	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		1,1,1,1,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		}, x, 4, 6));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[1], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[2], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP10th);

	optimiser = new CGA(4, 10, x);
	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		1,0,1,1,
		0,1,0,0,
		0,0,0,0,
		0,0,0,0,
		}, x, 4, 6));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[1], midM10th);
	EXPECT_EQ((*optimiser).probability_distribution[2], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP10th);

	optimiser = new CGA(4, 10, x);
	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		1,1,1,1,
		0,0,0,0,
		0,0,0,0,
		}, x, 4, 6));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[1], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[2], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP10th);

	optimiser = new CGA(4, 10, x);
	(*optimiser).update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		1,0,1,0,
		1,1,1,1,
		0,0,0,0,
		0,0,0,0,
		}, x, 4, 6));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], midP10th);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP10th);
}

TEST(CGATest, ForceUpdate) {
	FitnessOneMax x;
	int n = 4;
	int k = 10;
	uint16_t lower = (0xffff / n);
	uint16_t mid = (0xffff / 2);
	uint16_t kth = 0xffff / k;
	uint16_t kSamplePairth = kth / 5;
	uint16_t midP1 = mid + kSamplePairth;
	uint16_t midP2 = midP1 + kSamplePairth;
	uint16_t midP3 = midP2 + kSamplePairth;
	uint16_t midP4 = midP3 + kSamplePairth;
	uint16_t midP5 = midP4 + kSamplePairth;
	uint16_t midN1 = mid - kSamplePairth;
	uint16_t midN2 = midN1 - kSamplePairth;
	uint16_t midN3 = midN2 - kSamplePairth;
	uint16_t midN4 = midN3 - kSamplePairth;
	uint16_t midN5 = midN4 - kSamplePairth;
	uint16_t upper = (0xffff * ((n-1) / n));
	CGA* optimiser = new CGA(n, k, x);
	(*optimiser).force_update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,//0000
		0,0,0,0,
		0,0,0,0,//0000
		0,0,0,0,
		0,0,0,0,//0000
		0,0,0,0,
		0,0,0,0,//0000
		0,0,0,0,
		0,0,0,0,//0000
		}, x, 4, 10));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(n, k, x);
	(*optimiser).force_update_probability_distribution(sc({
		1,1,1,1,
		1,1,1,1,//0000
		1,1,1,1,
		1,1,1,1,//0000
		1,1,1,1,
		1,1,1,1,//0000
		1,1,1,1,
		1,1,1,1,//0000
		1,1,1,1,
		1,1,1,1,//0000
		}, x, 4, 10));
	EXPECT_EQ((*optimiser).probability_distribution[0], mid);
	EXPECT_EQ((*optimiser).probability_distribution[1], mid);
	EXPECT_EQ((*optimiser).probability_distribution[2], mid);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(n, k, x);
	(*optimiser).force_update_probability_distribution(sc({
		0,0,0,0,
		1,1,1,1,//1111
		0,0,0,0,
		1,1,1,1,//1111
		0,0,0,0,
		1,1,1,1,//1111
		0,0,0,0,
		1,1,1,1,//1111
		0,0,0,0,
		1,1,1,1,//1111
		}, x, 4, 10));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP5);
	EXPECT_EQ((*optimiser).probability_distribution[1], midP5);
	EXPECT_EQ((*optimiser).probability_distribution[2], midP5);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP5);

	optimiser = new CGA(n, k, x);
	(*optimiser).force_update_probability_distribution(sc({
		0,0,0,1,
		1,1,1,1,//+++0
		0,0,0,1,
		1,1,1,1,//+++0
		0,0,0,1,
		1,1,1,1,//+++0
		0,0,0,1,
		1,1,1,1,//+++0
		0,0,0,1,
		1,1,1,1,//+++0
		}, x, 4, 10));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP5);
	EXPECT_EQ((*optimiser).probability_distribution[1], midP5);
	EXPECT_EQ((*optimiser).probability_distribution[2], midP5);
	EXPECT_EQ((*optimiser).probability_distribution[3], mid);

	optimiser = new CGA(n, k, x);
	(*optimiser).force_update_probability_distribution(sc({
		0,1,0,0,
		1,0,1,1,//+-++
		0,1,0,0,
		1,0,1,1,//+-++
		0,1,0,0,
		1,0,1,1,//+-++
		0,1,0,0,
		1,0,1,1,//+-++
		0,1,0,0,
		1,0,1,1,//+-++
		}, x, 4, 10));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP5);
	EXPECT_EQ((*optimiser).probability_distribution[1], midN5);
	EXPECT_EQ((*optimiser).probability_distribution[2], midP5);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP5);

	optimiser = new CGA(n, k, x);
	(*optimiser).force_update_probability_distribution(sc({
		1,1,0,1,
		0,0,1,0,//++-+
		1,1,0,1,
		0,0,1,0,//++-+
		1,1,0,1,
		0,0,1,0,//++-+
		0,0,1,0,
		1,1,0,1,//++-+
		1,1,0,1,
		0,0,1,0,//++-+
		}, x, 4, 10));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP5);
	EXPECT_EQ((*optimiser).probability_distribution[1], midP5);
	EXPECT_EQ((*optimiser).probability_distribution[2], midN5);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP5);

	optimiser = new CGA(n, k, x);
	(*optimiser).force_update_probability_distribution(sc({
		1,1,1,1,
		0,1,1,1,//+000
		1,1,1,1,
		1,1,1,0,//000+
		1,1,1,1,
		1,1,1,0,//000+
		1,1,1,1,
		1,0,1,1,//0+00
		0,1,0,1,
		0,0,1,0,//0+-+
		}, x, 4, 10));
	EXPECT_EQ((*optimiser).probability_distribution[0], midP1);
	EXPECT_EQ((*optimiser).probability_distribution[1], midP2);
	EXPECT_EQ((*optimiser).probability_distribution[2], midN1);
	EXPECT_EQ((*optimiser).probability_distribution[3], midP3);
}

TEST(CGATest, StressTestUpdate) {
	int n = 1000; FitnessOneMax x;

	for (int k = 1; k <= 20; k++) {
		uint16_t mid = 0xffff / 2;
		uint16_t kth = 0xffff / k;
		uint16_t lower = 0xffff / n;
		uint16_t upper = 0xffff * (n - 1) / n;
		uint16_t midUpper = (uint16_t)(mid + (kth)); // what should value be if positive impact
		uint16_t midLower = (uint16_t)(mid - (kth)); // What should value be if negative impact
		if (kth >= mid - lower) {
			midUpper = upper;
			midLower = lower;
		}
		for (int i = 0; i < 1000; i++) {
			CGA optimiser(n, k, x);
			optimiser.sample();
			auto e1 = 0;
			auto e2 = n;
			optimiser.update_probability_distribution(optimiser.sample_container);
			if (x.fitness(optimiser.sample_container.samples, e1, n) < x.fitness(optimiser.sample_container.samples, e2, n)) {
				e1 = n;
				e2 = 0;
			}
			for (int index = 0; index < n; index++) {
				int v1 = optimiser.sample_container.samples[e1 + index];
				int v2 = optimiser.sample_container.samples[e2 + index];
				if (v1 == v2) {
					EXPECT_EQ(optimiser.probability_distribution[index], mid);
				}
				else if (v1 > v2) {
					EXPECT_EQ(optimiser.probability_distribution[index], midUpper);
				}
				else {
					EXPECT_EQ(optimiser.probability_distribution[index], midLower);
				}
			}
			EXPECT_EQ(optimiser.sample_container.sample_count, 2);
			EXPECT_EQ(optimiser.sample_container.sample_size, n);
			EXPECT_EQ(optimiser.sample_container.sample_fitnesses.size(), 2);
			EXPECT_EQ(optimiser.sample_container.samples.size(), n * 2);

		}
	}

}

TEST(CGATest, StressTestForceUpdate) {
	const int n = 1000; FitnessOneMax x;
	const int samplePairCount = 10;

	for (int k = 1; k <= 20; k++) {
		uint16_t mid = 0xffff / 2;
		uint16_t kth = 0xffff / k;
		uint16_t kSamplePairth = kth / samplePairCount;
		int lower = 0xffff / n;
		int upper = 0xffff * (n - 1) / n;
		for (int i = 0; i < 1000; i++) {
			CGA optimiser(n, k, x);
			int expectedValues[n];
			for (int index = 0; index < n; index++) {
				expectedValues[index] = mid;
			}
			SampleContainer sc(0, n);
			for (int samplePairi = 0; samplePairi < samplePairCount; samplePairi++) {
				optimiser.sample();
				sc.concat(optimiser.sample_container);
				auto e1 = 0;
				auto e2 = n;
				if (optimiser.sample_container.sample_fitnesses[0] < optimiser.sample_container.sample_fitnesses[1]) {
					e1 = n;
					e2 = 0;
				}
				for (int index = 0; index < n; index++) {
					int v1 = optimiser.sample_container.samples[e1 + index];
					int v2 = optimiser.sample_container.samples[e2 + index];
					if (v1 > v2) {
						expectedValues[index] = (expectedValues[index] + (kth / samplePairCount));
					}
					else if (v1 < v2) {
						expectedValues[index] = (expectedValues[index] - (kth / samplePairCount));
					}
				}
			}
			for (int index = 0; index < n; index++) {
				expectedValues[index] = (std::max((int)(lower), std::min((int)(upper), expectedValues[index])));
			}

			optimiser.force_update_probability_distribution(sc);
			for (int index = 0; index < n; index++) {
				//FOR DEBUGGING PURPOSES
				if (optimiser.probability_distribution[index] != expectedValues[index]) {

					int prob = optimiser.probability_distribution[index];
					int expected = expectedValues[index];

					std::vector<int> bests(samplePairCount);
					std::vector<int> worsts(samplePairCount);
					std::vector<int> change(samplePairCount);
					int totalChange = 0;
					for (int i = 0; i < samplePairCount; i++) {
						int index1 = i * 2 * n + index;
						int index2 = index1 + n;
						if (sc.sample_fitnesses[i] < sc.sample_fitnesses[i + 1]) {
							index2 = index1;
							index1 = index2 + n;
						}
						bests[i] = sc.samples[index1];
						worsts[i] = sc.samples[index2];
						change[i] = bests[i] - worsts[i];
						totalChange += change[i];
					}
					std::cout << "WERE HALTING" << std::endl;
					volatile int STOP = 13;
				}
				EXPECT_EQ(optimiser.probability_distribution[index], expectedValues[index]);
			}
			EXPECT_EQ(optimiser.sample_container.sample_count, 2);
			EXPECT_EQ(optimiser.sample_container.sample_size, n);
			EXPECT_EQ(optimiser.sample_container.sample_fitnesses.size(), 2);
			EXPECT_EQ(optimiser.sample_container.samples.size(), n * 2);

		}
	}
}


TEST(CGATest, StressTestUpdateBest) {
	const int n = 1000; FitnessOneMax x;
	const int samplePairCount = 10;

	for (int k = 1; k <= 20; k++) {
		uint16_t mid = 0xffff / 2;
		uint16_t kth = 0xffff / k;
		int lower = 0xffff / n;
		int upper = 0xffff * (n - 1) / n;
		for (int i = 0; i < 1000; i++) {
			CGA optimiser(n, k, x);
			int expectedValues[n];
			for (int index = 0; index < n; index++) {
				expectedValues[index] = mid;
			}
			SampleContainer sc(0, n);
			for (int samplePairi = 0; samplePairi < samplePairCount; samplePairi++) {
				optimiser.sample();
				sc.concat(optimiser.sample_container);
			}
			int bestFitnessIndex = 0;
			for (int samplei = 1; samplei < samplePairCount*2; samplei++) {
				if (sc.sample_fitnesses[samplei]>sc.sample_fitnesses[bestFitnessIndex]) {
					bestFitnessIndex = samplei;
				}
			}
			int worstFitnessindex = bestFitnessIndex%2==0?bestFitnessIndex+1 : bestFitnessIndex-1;

			optimiser.update_probability_distribution(sc);
			for (int index = 0; index < n; index++) {
				int best = sc.samples[bestFitnessIndex * n + index];
				int worst = sc.samples[worstFitnessindex * n + index];
				expectedValues[index] +=  (best-worst)* (kth);
				expectedValues[index] = (std::max((int)(lower), std::min((int)(upper), expectedValues[index])));
				int probDist = optimiser.probability_distribution[index];
				int expectedProbDist = expectedValues[index];
				EXPECT_EQ(probDist, expectedProbDist);
			}
			EXPECT_EQ(optimiser.sample_container.sample_count, 2);
			EXPECT_EQ(optimiser.sample_container.sample_size, n);
			EXPECT_EQ(optimiser.sample_container.sample_fitnesses.size(), 2);
			EXPECT_EQ(optimiser.sample_container.samples.size(), n * 2);

		}
	}
}


TEST(UMDA, UpdateProp) {
	FitnessOneMax x;
	umda optimiser(4, 4, 4, x);
	uint16_t lower = (0xffff *  (1.0 / 4));
	uint16_t mid = (0xffff * (1.0 / 2));
	uint16_t upper = (0xffff * (3.0 / 4));
	optimiser.update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0
		}, x));
	EXPECT_EQ(optimiser.probability_distribution[0], lower);
	EXPECT_EQ(optimiser.probability_distribution[1], lower);
	EXPECT_EQ(optimiser.probability_distribution[2], lower);
	EXPECT_EQ(optimiser.probability_distribution[3], lower);
	optimiser.update_probability_distribution(sc({
		1,0,0,0,
		1,0,0,0,
		0,0,0,0,
		0,0,0,0
		}, x));
	EXPECT_EQ(optimiser.probability_distribution[0], mid);
	EXPECT_EQ(optimiser.probability_distribution[1], lower);
	EXPECT_EQ(optimiser.probability_distribution[2], lower);
	EXPECT_EQ(optimiser.probability_distribution[3], lower);
	optimiser.update_probability_distribution(sc({
		1,0,1,0,
		1,0,1,0,
		1,0,1,0,
		1,0,1,0
		}, x));
	EXPECT_EQ(optimiser.probability_distribution[0], upper);
	EXPECT_EQ(optimiser.probability_distribution[1], lower);
	EXPECT_EQ(optimiser.probability_distribution[2], upper);
	EXPECT_EQ(optimiser.probability_distribution[3], lower);

}

TEST(UMDA, UpdateProp2) {
	FitnessOneMax x;
	umda optimiser(2, 4, 4, x);
	uint16_t lower = (0xffff * (1.0 / 4));
	uint16_t mid = (0xffff * (1.0 / 2));
	uint16_t upper = (0xffff * (3.0 / 4));
	optimiser.update_probability_distribution(sc({
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0
		}, x));
	EXPECT_EQ(optimiser.probability_distribution[0], lower);
	EXPECT_EQ(optimiser.probability_distribution[1], lower);
	EXPECT_EQ(optimiser.probability_distribution[2], lower);
	EXPECT_EQ(optimiser.probability_distribution[3], lower);
	optimiser.update_probability_distribution(sc({
		1,0,0,0,
		0,0,0,0,
		0,0,1,0,
		0,0,0,0
		}, x));
	EXPECT_EQ(optimiser.probability_distribution[0], mid);
	EXPECT_EQ(optimiser.probability_distribution[1], lower);
	EXPECT_EQ(optimiser.probability_distribution[2], mid);
	EXPECT_EQ(optimiser.probability_distribution[3], lower);
	optimiser.update_probability_distribution(sc({
		1,0,1,0,
		1,0,0,0,
		1,0,1,0,
		0,0,1,0
		}, x));
	EXPECT_EQ(optimiser.probability_distribution[0], upper);
	EXPECT_EQ(optimiser.probability_distribution[1], lower);
	EXPECT_EQ(optimiser.probability_distribution[2], upper);
	EXPECT_EQ(optimiser.probability_distribution[3], lower);
}

TEST(History, Truncation) {
	std::vector<iteration_snapshot> history;
	std::vector<iteration_snapshot> expectedResult;
	for (int i = 0; i < 100; i++) {
		history.push_back(iteration_snapshot(i, i, i, i, i));
		if (i % 5 == 0 || i == 3 || i == 99) {
			expectedResult.push_back(iteration_snapshot(i, i, i, i, i));
		}
	}

	std::vector<iteration_snapshot> truncHistory = DataTruncater::truncate_history(history, 5, 3, da<int>({}));

	EXPECT_EQ(expectedResult.size(), truncHistory.size());
	for (int i = 0; i < expectedResult.size(); i++) {
		EXPECT_EQ(
			expectedResult[i].iteration,
			truncHistory[i].iteration
		);
	}

}

TEST(History, Truncation2) {
	std::vector<iteration_snapshot> history;
	std::vector<iteration_snapshot> expectedResult;
	for (int i = 0; i < 100; i++) {
		history.push_back(iteration_snapshot(i, i, i, i, i));
		if (i % 5 == 0 || i == 3 || i == 99||i==7) {
			expectedResult.push_back(iteration_snapshot(i, i, i, i, i));
		}
	}

	std::vector<iteration_snapshot> truncHistory = DataTruncater::truncate_history(history, 5, 3, da<int>({3,15,7,7}));

	EXPECT_EQ(expectedResult.size(), truncHistory.size());
	for (int i = 0; i < expectedResult.size(); i++) {
		EXPECT_EQ(
			expectedResult[i].iteration,
			truncHistory[i].iteration
		);
	}

}

#pragma endregion
