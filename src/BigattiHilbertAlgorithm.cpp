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
  if (!state.getIdeal().disjointSupport())
	return false;

  basecase(state.getIdeal().begin(),
		   state.getIdeal().end(),
		   true,
		   state.getMultiply());
  return true;
}

void BigattiHilbertAlgorithm::basecase(Ideal::const_iterator begin, Ideal::const_iterator end, bool plus, const Term& term) {
  if (begin == end) {
    _output.add(plus ? 1 : -1, term);
  }
  else {
    basecase(begin + 1, end, plus, term);
    Term product(_varCount);
    product.product(term, *begin);
    basecase(begin + 1, end, !plus, product);
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
	state.colonStep(gcd);
	_output.add(-1, gcd);
	gcd.setToIdentity();
	_output.add(1, gcd);
  }

  IF_DEBUG(state.getIdeal().getGcd(gcd));
  ASSERT(gcd.isIdentity());
}

void BigattiHilbertAlgorithm::freeState(auto_ptr<BigattiState> state) {
  state->getIdeal().clear(); // To preserve memory
  _stateCache.freeObject(state);
}
