#include "EDABase.h"
#include "Utility/Random.h"
#include "immintrin.h"
#include <iostream>
#include <algorithm>

EDABase::EDABase(int lambda, int n, FitnessClass& fitness_class) 
    : sample_container(lambda, n), 
    probability_distribution(n, 0xffff / 2),
    _rng_buffer(std::ceil((n * lambda) / 16.0) * 16) {

    _random = Random();
    _fitnessclass = &fitness_class;

    _n = n;
    _lambda = lambda;
    best_fitness = -1;
    _itr_count = 0;
    _fe_count = 0;

    _lower = 1.0 / n;
    _upper = 1 - _lower;
}


DynamicArray<uint16_t> EDABase::get_x_best(SampleContainer& samples, int x) {
    DynamicArray<uint16_t> indices = index_sort(samples);
    
    DynamicArray<uint16_t> x_best(x * samples.sample_size);

    for(int i = 0; i < x; i++){
        int start = indices[i]*_n;
        int end = start + _n;
        std::copy(samples.samples.get() + start, samples.samples.get() + end, x_best.get() + i*_n);
    }

    return x_best;
}

void EDABase::log_state(SampleContainer& sample_container, DynamicArray<uint16_t>& indices) {
   if (best_fitness < sample_container.sample_fitnesses[indices[0]]) {
       best_fitness = sample_container.sample_fitnesses[indices[0]];
       best_fitness_itr = _itr_count;
   }

  /* history.push_back(
       iteration_snapshot( 
           sample_container.sample_fitnesses[indices[0]],
           sample_container.sample_fitnesses[indices[_lambda - 1]],
           sample_container.sample_fitnesses[indices[sample_container.sample_count / 2]],
           _itr_count, _fe_count));*/

   int lower_bounds_hit = 0;
   int upper_bounds_hit = 0;
   int tmp = _upper * 0xffff;
   for (int i = 0; i < probability_distribution.size(); i++) {
       int tmp1 = _lower * 0xffff;
       int tmp = _upper * 0xffff;
       int val = probability_distribution[i];
       if (probability_distribution[i] == tmp1) {
           lower_bounds_hit += 1;
       }
       else if (probability_distribution[i] == tmp) {
           upper_bounds_hit += 1;
       }
   }

   history.push_back(
       iteration_snapshot(
           sample_container.sample_fitnesses[indices[0]],
           sample_container.sample_fitnesses[indices[_lambda - 1]],
           sample_container.sample_fitnesses[indices[sample_container.sample_count / 2]],
           _itr_count, _fe_count, lower_bounds_hit, upper_bounds_hit));
}

DynamicArray<uint16_t> EDABase::index_sort(SampleContainer& samples) {
    DynamicArray<uint16_t> indices(samples.sample_count);

    for (int i = 0; i < samples.sample_count; i++) indices[i] = i;

    std::sort(indices.get(), indices.get() + indices.size(),
        [&](const int& a, const int& b) {
            return samples.sample_fitnesses[a] > samples.sample_fitnesses[b];
        });

    return indices;
}


void EDABase::sample()
{
    int start = (_n / 16) * 16;
    int rest = _n - start;
    __m256i sign_flip = _mm256_set1_epi16(0x8000);
    __m256i lsb_mask = _mm256_set1_epi16(1);

    _random.next_x_shorts(_n * _lambda, _rng_buffer.get());

    for (int j = 0; j < _lambda; j++) {
        for (int k = 0; k < _n - 15; k += 16) {
            int index = j * _n + k;
            _mm256_storeu_si256(
                reinterpret_cast<__m256i*>(sample_container.samples.get() + index), 
                _mm256_and_si256(
                    lsb_mask,
                    _mm256_cmpgt_epi16(
                        _mm256_xor_si256(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(probability_distribution.get() + k)), sign_flip),
                        _mm256_xor_si256(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(_rng_buffer.get() + index)), sign_flip))));
        }

        for (int k = start; k < start + rest; k++) {
            sample_container.samples[j * _n + k] = _rng_buffer[j * _n + k] < probability_distribution[k];
        }
    }

    for (int j = 0; j < sample_container.sample_count; j++) {
        sample_container.sample_fitnesses[j] = _fitnessclass->fitness(sample_container.samples, j * _n, _n);
    }
    _fe_count += sample_container.sample_count;
}
