#include <random>
#include <algorithm>
#include <vector>
#include <iostream>
#include "encodings/Enc_GTECluster.h"

#define NUM_CLUSTERS 10
#define MAX_PER_CLUSTER 20

void test_encoding()
{

	std::random_device rd;
	std::mt19937 g(rd());
	std::uniform_int_distribution<unsigned int> dis(1, MAX_PER_CLUSTER);
	std::uniform_int_distribution<uint64_t> dis64(1, 10000);

	std::vector<uint64_t> weights;
	for (int i=0; i<NUM_CLUSTERS; i++) {
		int n = dis(g);
		uint64_t w = dis64(g);
		for (int j=0; j<n; j++) {
			weights.push_back(w);
		}
	}

	Solver *s = new Solver();

	vec<Lit> literals;
	vec<uint64_t> weights_vec;
	std::shuffle(weights.begin(), weights.end(), g);
	for (unsigned i=0; i<weights.size(); i++) {
		literals.push(mkLit(s->newVar(), false));
		weights_vec.push(weights[i]);
	}

	std::uniform_int_distribution<unsigned> dis_unit(1, weights.size()/4);
	unsigned num_unit_clauses = dis_unit(g);

	uint64_t sum = 0;
	for (unsigned i=0; i<num_unit_clauses; i++) {
		sum += weights[i];
		s->addClause(literals[i]);
	}

	openwbo::GTECluster gte;
	uint64_t rhs = sum - 1;

	std::cout << "Number of lits: " << literals.size() << std::endl
		<< "Number of unit clauses: " << num_unit_clauses << std::endl;
	std::cout << "RHS: " << rhs << std::endl;

	gte.encode(s, literals, weights_vec, rhs);

	std::cout << "Encoded" << std::endl;
	//return;

	bool solved = s->solve();

	if (solved) {
		std::cout << "SATISFIED" << std::endl;
		for (unsigned i=0; i<weights_vec.size(); i++) {
			std::cout << weights_vec[i] << std::endl;
		}
	} else {
		std::cout << "UNSAT" << std::endl;
	}

}