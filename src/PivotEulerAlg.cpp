/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
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
#include "PivotEulerAlg.h"

#include "Ideal.h"
#include "RawSquareFreeTerm.h"
#include "RawSquareFreeIdeal.h"
#include "Task.h"
#include "TaskEngine.h"

#include <vector>

namespace Ops = SquareFreeTermOps;

class EulerState {
public:
  EulerState(const Ideal& idealParam) {
	const size_t varCount = idealParam.getVarCount();
	ideal = newRawSquareFreeIdeal(varCount, idealParam.getGeneratorCount());
	ideal->insert(idealParam);
	eliminated = Ops::newTerm(varCount);
	negateValue = false;
  }

  EulerState() {
	toZero();
  }
  
  ~EulerState() {
	deallocate();
  }

  void clone(const EulerState& state) {
	if (this == &state)
	  return;
	deallocate();

	// capacity +1 to support adding an element later
	const size_t capacity = state.ideal->getGeneratorCount() + 1;
	const size_t varCount = state.ideal->getVarCount();

	ideal = newRawSquareFreeIdeal(varCount, capacity);
	*ideal = *state.ideal;

	// todo: exception memory leak
	eliminated = Ops::newTerm(varCount);
	Ops::assign(eliminated, state.eliminated, varCount);

	negateValue = state.negateValue;
  }

  void toColonSubState(const Word* pivot) {
	ideal->colonReminimize(pivot);
	Ops::lcmInPlace(eliminated, pivot, ideal->getVarCount());
  }

  void toSumSubState(const Word* pivot) {
	ideal->insertReminimize(pivot);
  }

  bool optimize() {
	size_t pivot = ideal->getExclusiveVarGenerator();
	if (pivot != ideal->getGeneratorCount()) {
	  Ops::lcmInPlace(eliminated,
					  ideal->getGenerator(pivot), ideal->getVarCount());
	  flipValueSign();
	  ideal->colonReminimizeAndRemove(pivot);
	  return true;
	}
	return false;
  }

  bool baseCase(mpz_class& accumulator) {
	const size_t varCount = ideal->getVarCount();

	if (Ops::hasFullSupport(eliminated, varCount)) {
	  // The ideal is either <1> or <0> in a ring with no variables.
	  ASSERT(ideal->getGeneratorCount() <= 1);
	  if (ideal->getGeneratorCount() == 0) {
		if (negateValue)
		  --accumulator;
		else
		  ++accumulator;
	  }
	  return true;
	}

	return !ideal->hasFullSupport(eliminated);
  }

  void reset() {
	deallocate();
	toZero();
  }

  bool getNegateValue() const {
	return negateValue;
  }

  void flipValueSign() {
	negateValue = !negateValue;
  }

  const RawSquareFreeIdeal& getIdeal() const {
	return *ideal;
  }

  size_t getVarCount() const {
	return getIdeal().getVarCount();
  }

private:
  void toZero() {
	ideal = 0;
	eliminated = 0;
	negateValue = false;
  }

  void deallocate() {
	deleteRawSquareFreeIdeal(ideal);
	Ops::deleteTerm(eliminated);
  }

  RawSquareFreeIdeal* ideal;
  Word* eliminated;
  bool negateValue;
};

bool PivotEulerAlg::rec(EulerState& state, EulerState& newState) {
  while (true) {
	if (state.baseCase(_euler))
	  return false;
	if (state.optimize())
	  continue;

	getPivot(state, _pivot);

	newState.clone(state);
	newState.toSumSubState(_pivot);
	state.toColonSubState(_pivot);

	return true;
  }
}

void PivotEulerAlg::getPivot(const EulerState& state, Word* pivot) {
  const size_t varCount = state.getVarCount();
  state.getIdeal().getVarDividesCounts(_counts);
  Ops::setToIdentity(pivot, varCount);
  for (size_t var = 0; var < varCount; ++var) {
	if (_counts[var] != 0) {
	  Ops::setExponent(_pivot, var, true);
	  return;
	}
  }
  ASSERT(false);
}

void PivotEulerAlg::taskRun(RawSquareFreeIdeal& ideal) {
  
}

PivotEulerAlg::PivotEulerAlg(const Ideal& ideal) {
  _pivot = Ops::newTerm(ideal.getVarCount());
  _euler = 0;

  vector<EulerState*> todo;
  todo.push_back(new EulerState(ideal));
  while (!todo.empty()) {
	EulerState* state = todo.back();
	todo.pop_back();

	EulerState* newState = new EulerState();
	while (rec(*state, *newState)) {
	  todo.push_back(newState);
	  newState = new EulerState();
	}
	delete state;
	delete newState;
  }

  Ops::deleteTerm(_pivot);
}

mpz_class PivotEulerAlg::getEuler() {
  return _euler;
}
