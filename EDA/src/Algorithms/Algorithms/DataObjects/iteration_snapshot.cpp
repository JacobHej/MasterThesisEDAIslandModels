#include "iteration_snapshot.h"

iteration_snapshot::iteration_snapshot(uint16_t bf, uint16_t wf, uint16_t mf, uint32_t i, uint32_t fe) {
	this->best_fitness = bf;
	this->worst_fitness = wf;
	this->median_fitness = mf;
	this->iteration = i;
	this->fitness_evals = fe;
	this->lower_bound_hits = 0;
	this->upper_bound_hits = 0;
}

iteration_snapshot::iteration_snapshot(uint16_t bf, uint16_t wf, uint16_t mf, uint32_t i, uint32_t fe, int lbh, int ubh) : iteration_snapshot(bf, wf, mf, i, fe)
{
	this->lower_bound_hits = lbh;
	this->upper_bound_hits = ubh;
}

iteration_snapshot::iteration_snapshot(const iteration_snapshot& ref) : iteration_snapshot(
	ref.best_fitness,
	ref.worst_fitness,
	ref.median_fitness,
	ref.iteration,
	ref.fitness_evals,
	ref.lower_bound_hits,
	ref.upper_bound_hits
){}