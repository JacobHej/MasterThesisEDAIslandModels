#include <vector>
#include <numeric>
#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <cmath>
#include "../Algorithms/Utility/Random.h"
#include "../Coordination/Communocation/DynamicArray.h"
#pragma once

inline int leading(int x, DynamicArray<uint16_t>& samples, size_t start, size_t size) {
    int i = 0;
    while (i < size && samples[start + i] == x) {
        i++;
    }
    return i;
}

inline int trailing(int x, DynamicArray<uint16_t>& samples, size_t start, size_t size) {
    int i = 0;
    while (i < size && samples[start+size-1-i] == x) {
        i++;
    }
    return i;
}


class FitnessClass {
public:
    virtual int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) = 0;
};

class FitnessOneMax : public FitnessClass {
public:
    int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) override {
        return std::accumulate(samples.get() + start, samples.get() + start + size, 1);
    }
};

class FitnessZeroMax : public FitnessClass {
public:
    int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) override {
        return size - std::accumulate(samples.get() + start, samples.get() + start + size, 0);
    }
};

class FitnessLeadingOnes : public FitnessClass {
public:
    int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) override {
        return leading(1, samples, start, size);
    }
};

class FitnessLeadingZeros : public FitnessClass {
public:
    int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) override {
        return leading(0, samples, start, size);
    }
};

class FitnessFlipFlop : public FitnessClass {
public:
    int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) override {
        int sum = 0;
        for (int i = 0; i < size-1; i++) {
            if (samples[start + i] != samples[start + i + 1]) {
                sum++;
            }
        }
        return sum;
    }
};

class FitnessNeedle : public FitnessClass {
private:
    std::unique_ptr<uint16_t[]> _solution;
public:
    FitnessNeedle(std::unique_ptr<uint16_t[]>&& solution) : _solution(std::move(solution)) { }

    int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) override {
        for (int i = 0; i < size; i++) {
            if (_solution[i]!=samples[i+start]) {
                return 0;
            }
        }
        return size;
    }
};

class FitnessJumpk : public FitnessClass {
private:
    uint16_t k;
public:
    FitnessJumpk(uint16_t k) {
        this->k = k;
    }
    int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) override {
        int i = std::accumulate(samples.get() + start, samples.get() + start + size, 1);
        if (i > size - k && i < size) {
            return size-i;
        }
        return i+k;
    }
};

class FitnessKSAT : public FitnessClass {
public:
    std::vector<std::pair<std::vector<uint16_t>, std::vector<uint16_t>>> sat_clauses;

    FitnessKSAT(const std::vector<std::pair<std::vector<uint16_t>, std::vector<uint16_t>>>& sat_clauses) {
        this->sat_clauses = sat_clauses;
    }

    int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) override {
        int sum = 0;
        for (int clause_nr = 0; clause_nr < sat_clauses.size(); clause_nr++) {
            int clause_size = sat_clauses[clause_nr].first.size();
            for (int litteral_index = 0; litteral_index < clause_size; litteral_index++) {
                int litteral = sat_clauses[clause_nr].first[litteral_index];
                if ((samples[litteral + start] ^ sat_clauses[clause_nr].second[litteral_index])) {
                    sum++;
                    break;
                }
            }
        }

        return sum;
    }
};

class FitnessTwoPeaks : public FitnessClass {
private:
public:
    int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) override {
        auto tail = leading(1, samples, start, size);
        auto head = trailing(0, samples, start, size);
        return std::max(tail, head);
    }
};

/*
class FitnessFourPeaks : public FitnessClass {
private:
    uint16_t T;
public:
    FitnessFourPeaks(uint16_t T) {
        this->T = T;
    }
    int fitness(const std::vector<uint16_t>& sample) override {
        auto tail = leading(1, sample);
        auto head = trailing(0, sample);

        return std::max(tail, head) + (tail > T && head > T ? sample.size() : 0);
    }
};
*/

class FitnessKnapsack : public FitnessClass {
private:
    std::vector<uint16_t> _weights;
    int _capacity;
public:
    FitnessKnapsack(const std::vector<uint16_t>& weights, int capacity) {
        _weights = weights;
        _capacity = capacity;
    }
    int fitness(DynamicArray<uint16_t>& samples, size_t start, size_t size) override {
        uint32_t weight = 0;
        for (int i = 0; i < size; i++) {
            if (samples[i+start] == 1) {
                weight += _weights[i];
            }
        }
        if (weight <= _capacity) {
            return weight;
        }
        return 0;
    }
};