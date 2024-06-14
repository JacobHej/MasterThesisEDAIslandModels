#pragma once
#include <vector>
#include <string>

//Paradigms
#define LOGN 0
#define SQRTNLOGN 1
#define NLOGN 2
static std::vector<std::string> args;
const int bigAmount = 16;


static bool first = true;
int truncation = 1000000;
int runAmount;
int runStart;

//GENERALISATION ----------------------------------------------------------------

struct CGAEnumText {
	static const char* enumtext[];
};

const char* CGAEnumText::enumtext[] = { "Sample","MuSample","Prob" , "NoMigration" };


enum MigrationStrategyEnum
{
	sample, musample, prop, nomigration
};
struct EnumText {
	static const char* enumtext[];
};

const char* EnumText::enumtext[] = { "Sample","MuSample","Prob" , "NoMigration" };