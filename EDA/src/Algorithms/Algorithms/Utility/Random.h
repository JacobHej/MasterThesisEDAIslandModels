#pragma once
#include <cstdint>
#include <memory>
#include <random>
//The code and implementatin of this class was based on https://en.wikipedia.org/wiki/Xorshift

class Random {
	private: 
		uint64_t x[8];
	public:
		Random();
		void next_x_shorts(int r_count, uint16_t* result);
};

