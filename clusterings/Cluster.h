#ifndef Cluster_h
#define Cluster_h

#ifdef SIMP
#include "simp/SimpSolver.h"
#else
#include "core/Solver.h"
#endif

#include "SequenceStatistic.h"
#include "../MaxSATFormulaExtended.h"
#include <vector>

using NSPACE::vec;
using std::vector;

namespace openwbo {
  
class Cluster {
  
public:
  vec<uint64_t> original_weights; // store the original weights
  Statistics cluster_statistic;
  SequenceStatistic statistic_finder;
  uint64_t num_clauses;
  
  Cluster(MaxSATFormulaExtended *formula, Statistics cluster_statistic);
  virtual void clusterWeights(MaxSATFormulaExtended *formula, uint64_t c) = 0;
  void saveWeights(MaxSATFormulaExtended *formula);
  void restoreWeights(MaxSATFormulaExtended *formula);
  void replaceWeights(MaxSATFormulaExtended *formula, vector<uint64_t> &clusters);
  uint64_t getOriginalWeight(int index);
  
}; // class Cluster
  
} // namespace openwbo


#endif
