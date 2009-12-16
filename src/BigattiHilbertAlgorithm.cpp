/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "BigattiHilbertAlgorithm.h"

#include "Ideal.h"
#include "CoefTermConsumer.h"
#include "BigattiState.h"

BigattiHilbertAlgorithm::BigattiHilbertAlgorithm(CoefTermConsumer* consumer):
	_consumer(consumer) {
}

void BigattiHilbertAlgorithm::run(const Ideal& ideal) {
  ASSERT(ideal.isMinimallyGenerated());
  _varCount = ideal.getVarCount();
  _output.clearAndSetVarCount(_varCount);
  _tmp_getPivot_counts.reset(_varCount);
  _tmp_simplify_gcd.reset(_varCount);
  _tmp_baseCase_lcm.reset(_varCount);
  _tmp_baseCase_maxCount.reset(_varCount);

  _tmp_allCombinationsBaseCase_lcms.clear();
  _tmp_allCombinationsBaseCase_lcms.reserve(_varCount);
  for (size_t i = 0; i < _varCount; ++i)
	_tmp_allCombinationsBaseCase_lcms.push_back(Term(_varCount));
  _tmp_allCombinationsBaseCase_included.resize(_varCount);
  _tmp_allCombinationsBaseCase_term.reset(_varCount);

  _tasks.addTask(new BigattiState(this, ideal, Term(_varCount)));
  _tasks.runTasks();

  //_output.sortTermsReverseLex(true);
  //_consumer->consume(_output);
  _output.feedTo(*_consumer);
}

void BigattiHilbertAlgorithm::processState(auto_ptr<BigattiState> state) {
  simplify(*state);

  if (baseCase(*state)) {
	freeState(state);
	return;
  }

  Term& pivot = _tmp_processState_pivot;
  getPivot(*state, pivot);

  auto_ptr<BigattiState> colonState(_stateCache.newObjectCopy(*state));
  colonState->colonStep(pivot);
  _tasks.addTask(colonState.release());

  state->addStep(pivot);
  _tasks.addTask(state.release());
}

bool BigattiHilbertAlgorithm::baseCase(const BigattiState& state) {
  Term& lcm = _tmp_baseCase_lcm;
  Term& maxCount = _tmp_baseCase_maxCount;

  ASSERT(state.getIdeal().getVarCount() == _varCount);
  ASSERT(lcm.getVarCount() == _varCount);
  ASSERT(maxCount.getVarCount() == _varCount);

  if (state.getIdeal().isZeroIdeal()) {
    _output.add(1, state.getMultiply());
    return true;
  }
  if (state.getIdeal().getGeneratorCount() == 1 &&
    state.getIdeal().containsIdentity()) {
    return true;
  }
  
  if (state.getIdeal().getGeneratorCount() > _varCount)
	return false;

  state.getIdeal().getLcm(lcm);
  if (state.getIdeal().getGeneratorCount() > lcm.getSizeOfSupport())
	return false;

  maxCount.setToIdentity();
  Ideal::const_iterator end = state.getIdeal().end();
  Ideal::const_iterator it = state.getIdeal().begin();
  for (; it != end; ++it) {
	bool hasMax = false;
	for (size_t var = 0; var < _varCount; ++var) {
	  ASSERT((*it)[var] <= lcm[var]);
	  if ((*it)[var] == lcm[var] && lcm[var] > 0) {
		hasMax = true;
		maxCount[var] += 1;
		if (maxCount[var] > 1)
		  return false;
	  }
	}
	if (!hasMax)
	  return false;
  }

  allCombinationsBaseCase(state);
  /*
  Term lcm2(_varCount);
  lcm2.setToIdentity();
  basecase(state.getIdeal().begin(),
		   state.getIdeal().end(),
		   true,
		   lcm2,
		   state.getMultiply());//*/
  return true;
}

void BigattiHilbertAlgorithm::allCombinationsBaseCase(const BigattiState& state) {
  vector<Term>& lcms = _tmp_allCombinationsBaseCase_lcms;
  vector<int>& included = _tmp_allCombinationsBaseCase_included;
  Term& term = _tmp_allCombinationsBaseCase_term;

  ASSERT(state.getIdeal().getGeneratorCount() <= _varCount);
  ASSERT(included.size() == _varCount);
  ASSERT(lcms.size() == _varCount);
  ASSERT(lcms.empty() || lcms[0].getVarCount() == _varCount);
  ASSERT(term.getVarCount() == _varCount);

  size_t genCount = state.getIdeal().getGeneratorCount();

  for (size_t i = 0; i < genCount; ++i) {
	ASSERT(lcms[i].getVarCount() == _varCount);
	lcms[i].setToIdentity();
	included[i] = 0;
  }

  bool plus = true;
  while (true) {
    term.product(lcms[0], state.getMultiply());
	_output.add(plus ? 1 : -1, term);

	size_t gen = 0;
    while (true) {
	  if (gen == genCount)
		return;
      if (!included[gen])
        break;
	  ++gen;
	}
	plus = (gen % 2 == 0 ? !plus : plus);

	ASSERT(!included[gen]);
	included[gen] = true;
	if (gen == genCount - 1)
	  lcms[gen] = state.getIdeal()[gen];
	else
	  lcms[gen].lcm(state.getIdeal()[gen], lcms[gen + 1]);

	while (gen > 0) {
	  --gen;
	  ASSERT(included[gen]);
	  included[gen] = false;
	  lcms[gen] = lcms[gen + 1];
	}
  }
}

void BigattiHilbertAlgorithm::basecase(Ideal::const_iterator begin, Ideal::const_iterator end, bool plus, const Term& term, const Term& multiply) {
  if (begin == end) {
	Term& product = _tmp_allCombinationsBaseCase_term;
    ASSERT(product.getVarCount() == _varCount);
	product.product(term, multiply);
    _output.add(plus ? 1 : -1, product);
  }
  else {
    Term& lcm = _tmp_allCombinationsBaseCase_lcms[end - begin - 1];
    ASSERT(lcm.getVarCount() == _varCount);

    basecase(begin + 1, end, plus, term, multiply);
    lcm.lcm(term, *begin);
    basecase(begin + 1, end, !plus, lcm, multiply);
  }
}

void BigattiHilbertAlgorithm::getPivot(BigattiState& state, Term& pivot) {
  Term& counts = _tmp_getPivot_counts;
  ASSERT(counts.getVarCount() == _varCount);

  state.getIdeal().getSupportCounts(counts);
  size_t var = counts.getFirstMaxExponent();

  pivot.reset(_varCount);
  pivot[var] = 1;
}

void BigattiHilbertAlgorithm::simplify(BigattiState& state) {
  Term& gcd = _tmp_simplify_gcd;
  ASSERT(gcd.getVarCount() == _varCount);

  state.getIdeal().getGcd(gcd);
  if (!gcd.isIdentity()) {
    // Do colon and output multiply-gcd*multiply.
    _output.add(1, state.getMultiply());
	state.colonStep(gcd);
	_output.add(-1, state.getMultiply());
  }

  IF_DEBUG(state.getIdeal().getGcd(gcd));
  ASSERT(gcd.isIdentity());
}

void BigattiHilbertAlgorithm::freeState(auto_ptr<BigattiState> state) {
  state->getIdeal().clear(); // To preserve memory
  _stateCache.freeObject(state);
}
