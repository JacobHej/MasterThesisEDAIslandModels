#include "Random.h"
#include <stdlib.h>
#include <chrono>
#include <memory>
#include <cmath>
#include <immintrin.h>
#include <iostream>
#include <random>

// The code and implementatin of this class was based on https://en.wikipedia.org/wiki/Xorshift
Random::Random() {
    srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    for (int i = 0; i < 8; i++) {
        x[i] = rand();
        x[i] = x[i] << 32;
        x[i] |= rand();
    }
}

void Random::next_x_shorts(int r_count, uint16_t* result) {
    int sixteen_count = std::ceil(r_count / 16.0);

    for (int i = 0; i < sixteen_count; i++) {
        __m256i t = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(x)); // uint64_t t = x[0];
        __m256i s = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(x + 4)); // uint64_t const s = x[1];

        _mm256_storeu_si256(reinterpret_cast<__m256i*>(x), s); // x[0] = s;

        t = _mm256_xor_si256(t, _mm256_slli_epi64(t, 23)); // t ^= t << 23
        t = _mm256_xor_si256(t, _mm256_srli_epi64(t, 18)); // t ^= t >> 18
        t = _mm256_xor_si256(t, _mm256_xor_si256(s, _mm256_srli_epi64(s, 5))); // t ^= s ^ (s >> 5)

        _mm256_storeu_si256(reinterpret_cast<__m256i*>(x + 4), t); // x[1] = t;

        _mm256_storeu_si256(reinterpret_cast<__m256i*>(result + 16 * i), _mm256_add_epi64(t, s) /*uint64_t r = (t + s)*/);
    }
}

