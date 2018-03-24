#include "Enc_GTEIncremental.h"
#include <algorithm>
#include <numeric>
using namespace openwbo;

struct less_than_wlitt {
  inline bool operator()(const wlitt &wl1, const wlitt &wl2) {
    return (wl1.weight < wl2.weight);
  }
};

Lit GTEIncremental::getNewLit(Solver *S) {
  Lit p = mkLit(S->nVars(), false);
  newSATVariable(S);
  nb_variables++;
  return p;
}

Lit GTEIncremental::get_var(Solver *S, wlit_mapt &oliterals, uint64_t weight) {
  wlit_mapt::iterator it = oliterals.find(weight);
  if (it == oliterals.end()) {
    Lit v = getNewLit(S);
    oliterals[weight] = v;
  }
  return oliterals[weight];
}

bool GTEIncremental::encodeLeq(uint64_t k, Solver *S, const weightedlitst &iliterals,
                    wlit_mapt &oliterals) {

  if (iliterals.size() == 0 || k == 0)
    return false;

  if (iliterals.size() == 1) {

    oliterals.insert(
        wlit_pairt(iliterals.front().weight, iliterals.front().lit));
    return true;
  }

  unsigned int size = iliterals.size();

  // formulat lformula,rformula;
  weightedlitst linputs, rinputs;
  wlit_mapt loutputs, routputs;

  unsigned int lsize = size >> 1;
  // unsigned int rsize=size-lsize;
  weightedlitst::const_iterator myit = iliterals.begin();
  weightedlitst::const_iterator myit1 = myit + lsize;
  weightedlitst::const_iterator myit2 = iliterals.end();

  linputs.insert(linputs.begin(), myit, myit1);
  rinputs.insert(rinputs.begin(), myit1, myit2);

  /*wlitt init_wlit;
  init_wlit.lit = lit_Undef;
  init_wlit.weight=0;*/
  wlit_sumt wlit_sum;
  uint64_t lk = std::accumulate(linputs.begin(), linputs.end(), 0, wlit_sum);
  uint64_t rk = std::accumulate(rinputs.begin(), rinputs.end(), 0, wlit_sum);

  lk = k >= lk ? lk : k;
  rk = k >= rk ? rk : k;

  bool result = encodeLeq(lk, S, linputs, loutputs);
  if (!result)
    return result;
  result = result && encodeLeq(rk, S, rinputs, routputs);
  if (!result)
    return result;

  {
    assert(!loutputs.empty());

    for (wlit_mapt::iterator mit = loutputs.begin(); mit != loutputs.end();
         mit++) {

      if (mit->first > k) {
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, k));
        nb_clauses++;
      } else {
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, mit->first));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,l.first));
      }

      // formula.push_back(std::move(clause));
    }
  }

  {
    assert(!routputs.empty());
    for (wlit_mapt::iterator mit = routputs.begin(); mit != routputs.end();
         mit++) {

      if (mit->first > k) {
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, k));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,k));
      } else {
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, mit->first));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,r.first));
      }

      // formula.push_back(std::move(clause));
    }
  }

  // if(!lformula.empty() && !rformula.empty())
  {
    for (wlit_mapt::iterator lit = loutputs.begin(); lit != loutputs.end();
         lit++) {
      for (wlit_mapt::iterator rit = routputs.begin(); rit != routputs.end();
           rit++) {
        /*clauset clause;
        clause.push_back(-l.second);
        clause.push_back(-r.second);*/
        uint64_t tw = lit->first + rit->first;
        if (tw > k) {
          addTernaryClause(S, ~lit->second, ~rit->second,
                           get_var(S, oliterals, k));
          nb_clauses++;
          // clause.push_back(get_var(auxvars,oliterals,k));
        } else {
          addTernaryClause(S, ~lit->second, ~rit->second,
                           get_var(S, oliterals, tw));
          nb_clauses++;
          // clause.push_back(get_var(auxvars,oliterals,tw));
        }

        // formula.push_back(std::move(clause));
      }
    }
  }
  
  return true;
}

bool GTEIncremental::encodeLeqIncremental(uint64_t k, Solver *S, const weightedlitst &iliterals,
                    wlit_mapt &oliterals) {

  if (iliterals.size() == 0 || k == 0)
    return false;

  if (iliterals.size() == 1) {
    
    oliterals.insert(
        wlit_pairt(iliterals.front().weight, iliterals.front().lit));
    return true;
  }

  unsigned int size = iliterals.size();

  // formulat lformula,rformula;
  weightedlitst linputs, rinputs;
  wlit_mapt loutputs, routputs;

  unsigned int lsize = size >> 1;
  // unsigned int rsize=size-lsize;
  weightedlitst::const_iterator myit = iliterals.begin();
  weightedlitst::const_iterator myit1 = myit + lsize;
  weightedlitst::const_iterator myit2 = iliterals.end();

  linputs.insert(linputs.begin(), myit, myit1);
  rinputs.insert(rinputs.begin(), myit1, myit2);

  /*wlitt init_wlit;
  init_wlit.lit = lit_Undef;
  init_wlit.weight=0;*/
  wlit_sumt wlit_sum;
  uint64_t lk = std::accumulate(linputs.begin(), linputs.end(), 0, wlit_sum);
  uint64_t rk = std::accumulate(rinputs.begin(), rinputs.end(), 0, wlit_sum);

  lk = k >= lk ? lk : k;
  rk = k >= rk ? rk : k;

  bool result = encodeLeqIncremental(lk, S, linputs, loutputs);
  if (!result)
    return result;
  result = result && encodeLeqIncremental(rk, S, rinputs, routputs);
  if (!result)
    return result;
    
  bool added_first_above_k = false;
  uint64_t least_weight_above_k = 0;
  
  assert(!loutputs.empty());
  assert(!routputs.empty());
  
  uint64_t left_largest_weight = loutputs.back().first;
  uint64_t right_largest_weight = routputs.back().first;
  
  // find last element for current level, if it directly comes from the 
  // level immediately below
  if(left_largest_weight > k) {
    added_first_above_k = true;
    if(right_largest_weight > k && 
          right_largest_weight < left_largest_weight) {
      least_weight_above_k = right_largest_weight;
    } else {
      least_weight_above_k = left_largest_weight;
    }
  } else if(right_largest_weight > k) {
    added_first_above_k = true;
    least_weight_above_k = right_largest_weight;
  }

  {
    
    for (wlit_mapt::iterator mit = loutputs.begin(); mit != loutputs.end();
         mit++) {

      if (mit->first > k) {
        addBinaryClause(S, ~mit->second, 
          get_var(S, oliterals, least_weight_above_k));
        nb_clauses++;
      } else {
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, mit->first));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,l.first));
      }

      // formula.push_back(std::move(clause));
    }
  }

  {
    
    for (wlit_mapt::iterator mit = routputs.begin(); mit != routputs.end();
         mit++) {

      if (mit->first > k) {
        addBinaryClause(S, ~mit->second,
          get_var(S, oliterals, least_weight_above_k));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,k));
      } else {
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, mit->first));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,r.first));
      }

      // formula.push_back(std::move(clause));
    }
  }
  
  // finding least weight above k among pairwise sums, since we already 
  // know that no single weight is above k
  if(!added_first_above_k) {
    least_weight_above_k = loutputs.back().first + routputs.back().first;
    if(least_weight_above_k > k) {
      added_first_above_k = true;
      for (wlit_mapt::iterator lit = loutputs.begin(); lit != loutputs.end();
         lit++) {
        for (wlit_mapt::iterator rit = routputs.begin(); rit != routputs.end();
           rit++) {
          uint64_t tw = lit->first + rit->first;
          if(tw > k && tw < least_weight_above_k) {
            least_weight_above_k = tw;
          }
        }
      }
    }
  }

  // if(!lformula.empty() && !rformula.empty())
  {
    // sending pairwise sums to parent
    for (wlit_mapt::iterator lit = loutputs.begin(); lit != loutputs.end();
         lit++) {
      for (wlit_mapt::iterator rit = routputs.begin(); rit != routputs.end();
           rit++) {
        /*clauset clause;
        clause.push_back(-l.second);
        clause.push_back(-r.second);*/
        uint64_t tw = lit->first + rit->first;
        if (tw > k) {
          addTernaryClause(S, ~lit->second, ~rit->second,
                           get_var(S, oliterals, least_weight_above_k)); // TODO - check
          nb_clauses++;
          // clause.push_back(get_var(auxvars,oliterals,k));
        } else {
          addTernaryClause(S, ~lit->second, ~rit->second,
                           get_var(S, oliterals, tw));
          nb_clauses++;
          // clause.push_back(get_var(auxvars,oliterals,tw));
        }

        // formula.push_back(std::move(clause));
      }
    }
  }
  
  for(wlit_mapt::reverse_iterator oit = ++(oliterals.rbegin());
      oit != oliterals.rend(); oit++) {
    wlit_mapt::reverse_iterator implied_lit = oit;
    --implied_lit;
    addBinaryClause(S, ~oit->second, implied_lit->second);
    nb_clauses++;
  }

  return true;
}

void GTEIncremental::build(Solver *S, vec<Lit> &lits, vec<uint64_t> &coeffs,
				uint64_t rhs) {
	vec<Lit> simp_lits;
  vec<uint64_t> simp_coeffs;
  lits.copyTo(simp_lits);
  coeffs.copyTo(simp_coeffs);

  lits.clear();
  coeffs.clear();

  // Fix literals that have a coeff larger than rhs.
  for (int i = 0; i < simp_lits.size(); i++) {
    if (simp_coeffs[i] == 0)
      continue;
	
    // CHANGED TO INT64_MAX for now - Sukrut
    if (simp_coeffs[i] >= INT64_MAX) {
      printf("c Overflow in the Encoding\n");
      printf("s UNKNOWN\n");
      exit(_ERROR_);
    }

    if (simp_coeffs[i] <= (unsigned)rhs) {
      lits.push(simp_lits[i]);
      coeffs.push(simp_coeffs[i]);
    } else if(incremental_strategy == _INCREMENTAL_ITERATIVE) {
      lits.push(simp_lits[i]);
      coeffs.push(simp_coeffs[i]);
      addUnitClause(S, ~simp_lits[i]);
    } else
      addUnitClause(S, ~simp_lits[i]);
  }

  if (lits.size() == 1) { // TODO - how is this sound? - Sukrut
    // addUnitClause(S, ~lits[0]);
    return;
  }

  if (lits.size() == 0)
    return;

  weightedlitst iliterals;
  for (int i = 0; i < lits.size(); i++) {
    wlitt wl;
    wl.lit = lits[i];
    wl.weight = coeffs[i];
    iliterals.push_back(wl);
  }
  less_than_wlitt lt_wlit;
  std::sort(iliterals.begin(), iliterals.end(), lt_wlit);
  if(incremental_strategy == _INCREMENTAL_ITERATIVE_) {
  	// this is because iliterals may contain weights > k
  	auto index_above_k = 
  				std::upper_bound(iliterals.begin(), iliterals.end(), rhs);
  	// if not, all elements are above k, so previous loop has taken care of this
  	if(index_above_k != iliterals.begin()) {
  	  // find last index to use for encoding so far
  	  auto last_index = index_above_k;
  	  --last_index;
  	  // get lits up to that index \
  	  // we are using separate vector because original lits will be 
  	  // needed later when incremental step is called
  	  weightedlitst iliterals_less_k(iliterals.begin(),last_index);
  		encodeLeqIncremental(rhs + 1, S, iliterals_less_k, pb_oliterals);
  	}
  } else {
    encodeLeq(rhs + 1, S, iliterals, pb_oliterals);
  }
  
  // fill up assumptions outside, like in totalizer encoding 
  // this function need not handle it
  if(incremental_strategy == _INCREMENTAL_NONE_) {
    for (wlit_mapt::reverse_iterator rit = pb_oliterals.rbegin();
      rit != pb_oliterals.rend(); rit++) {
      if (rit->first > rhs) {
      //  assumptions.push(~rit->second);
        addUnitClause(S, ~rit->second);
      } else {
        break;
      }
    } 
  }	
}

void GTEIncremental::encode(Solver *S, vec<Lit> &lits, vec<uint64_t> &coeffs,
                 uint64_t rhs) {
  // FIXME: do not change coeffs in this method. Make coeffs const.

  // If the rhs is larger than INT32_MAX is not feasible to encode this
  // pseudo-Boolean constraint to CNF.
  // CHANGED TO INT64_MAX for now - Sukrut
  if (rhs >= INT64_MAX) {
    printf("c Overflow in the Encoding\n");
    printf("s UNKNOWN\n");
    exit(_ERROR_);
  }

  hasEncoding = false; // TODO, Sukrut - what is the purpose of this?
  nb_variables = 0;
  nb_clauses = 0;
  
  build(S, lits, coeffs, rhs);
  
  for (int i = 0; i < lits.size(); i++) {
    wlitt wl;
    wl.lit = lits[i];
    wl.weight = coeffs[i];
    enc_literals.push_back(wl);
  }

  // addUnitClause(S,~pb_oliterals.rbegin()->second);
  /*
  if (pb_oliterals.rbegin()->first != rhs+1){
        printf("%d - %d\n",pb_oliterals.rbegin()->first,rhs);
        for(wlit_mapt::reverse_iterator
  rit=pb_oliterals.rbegin();rit!=pb_oliterals.rend();rit++)
  {
        printf("rit->first %d\n",rit->first);
  }
  }
  */
  // assert (pb_oliterals.rbegin()->first == rhs+1);
  // printLit(~pb_oliterals.rbegin()->second);
  /* ... PUT CODE HERE FOR CREATING THE ENCODING ... */
  /* ... do not forget to sort the coefficients so that GTE is more efficient
   * ... */

  current_pb_rhs = rhs;
  hasEncoding = true;
}

void GTEIncremental::update(Solver *S, uint64_t rhs) {

  // TODO - for now, I am assuming that RHS does not increase in a given tree
  assert(hasEncoding);
  for (wlit_mapt::reverse_iterator rit = pb_oliterals.rbegin();
       rit != pb_oliterals.rend(); rit++) {
    if (rit->first > current_pb_rhs)
      continue;
    if (rit->first > rhs) {
      addUnitClause(S, ~rit->second);
    } else {
      break;
    }
  }
  /* ... PUT CODE HERE TO UPDATE THE RHS OF AN ALREADY EXISTING ENCODING ... */
  
  // add missing literals and clauses
  current_pb_rhs = rhs;
}

void GTEIncremental::join(Solver *S, vec<Lit> &lits, vec<uint64_t> &coeffs,
                 uint64_t rhs) {
	
  assert(incremental_strategy == _INCREMENTAL_ITERATIVE_);
  wlit_mapt old_pb_oliterals;
  old_pb_oliterals.insert(pb_oliterals.begin(), pb_oliterals.end());
  pb_oliterals.clear();
  build(S, lits, coeffs, rhs);

  uint64_t old_pb = current_pb_rhs;

  encode(S, lits, coeffs, rhs);
 
  wlit_mapt right_pb_oliterals;
  right_pb_oliterals.insert(pb_oliterals.begin(), pb_oliterals.end());
  pb_oliterals.clear();
  
  
  
  
}

void GTEIncremental::
