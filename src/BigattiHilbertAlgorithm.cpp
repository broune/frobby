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

BigattiHilbertAlgorithm::BigattiHilbertAlgorithm(const Ideal& ideal, CoefTermConsumer* consumer):
 _consumer(consumer),
 _baseCase(ideal.getVarCount()) {

   ASSERT(ideal.isMinimallyGenerated());
  _varCount = ideal.getVarCount();
  _tmp_getPivot_counts.reset(_varCount);
  _tmp_simplify_gcd.reset(_varCount);

  _tasks.addTask(new BigattiState(this, ideal, Term(_varCount)));
}

void BigattiHilbertAlgorithm::run() {
  _tasks.runTasks();
  _baseCase.feedOutputTo(*_consumer);
  //_output.feedTo(*_consumer);
}

void BigattiHilbertAlgorithm::processState(auto_ptr<BigattiState> state) {
  simplify(*state);

  if (_baseCase.baseCase(*state)) {
	freeState(state);
	return;
  }

  size_t pivotVar;
  Exponent pivotExponent;
  getPivot(*state, pivotVar, pivotExponent);

  auto_ptr<BigattiState> colonState(_stateCache.newObjectCopy(*state));
  colonState->colonStep(pivotVar, pivotExponent);
  _tasks.addTask(colonState.release());

  state->addStep(pivotVar, pivotExponent);
  _tasks.addTask(state.release());
}


void BigattiHilbertAlgorithm::getPivot(BigattiState& state, size_t& var, Exponent& e) {
  Term& counts = _tmp_getPivot_counts;
  ASSERT(counts.getVarCount() == _varCount);

  state.getIdeal().getSupportCounts(counts);
  var = counts.getFirstMaxExponent();
  e = state.getMedianPositiveExponentOf(var);
}

void BigattiHilbertAlgorithm::simplify(BigattiState& state) {
  Term& gcd = _tmp_simplify_gcd;
  ASSERT(gcd.getVarCount() == _varCount);

  state.getIdeal().getGcd(gcd);
  if (!gcd.isIdentity()) {
    // Do colon and output multiply-gcd*multiply.
    //_output.add(1, state.getMultiply());
    _baseCase.outputPlus(state.getMultiply());
	state.colonStep(gcd);
	//_output.add(-1, state.getMultiply());
    _baseCase.outputMinus(state.getMultiply());
  }

  IF_DEBUG(state.getIdeal().getGcd(gcd));
  ASSERT(gcd.isIdentity());
}

void BigattiHilbertAlgorithm::freeState(auto_ptr<BigattiState> state) {
  state->getIdeal().clear(); // To preserve memory
  _stateCache.freeObject(state);
}
