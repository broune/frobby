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

  RawSquareFreeIdeal* ideal;
  Word* eliminated;

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

  bool negateValue;
};

void PivotEulerAlg::rec(EulerState& state) {
  const size_t varCount = state.ideal->getVarCount();

  if (state.baseCase(_euler)) {
	return;
  }

  if (state.optimize()) {
	rec(state);
	return;
  }

  getPivot(state, _pivot);
  ASSERT(!Ops::isIdentity(_pivot, varCount));
  ASSERT(Ops::isRelativelyPrime(_pivot, state.eliminated, varCount));

  EulerState sumSubState;
  sumSubState.clone(state);
  state.toColonSubState(_pivot);

  sumSubState.toSumSubState(_pivot);

  rec(state);
  rec(sumSubState);
}

void PivotEulerAlg::getPivot(const EulerState& state, Word* pivot) {
  const size_t varCount = state.ideal->getVarCount();
  state.ideal->getVarDividesCounts(_counts);
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
  RawSquareFreeIdeal* packed = newRawSquareFreeIdeal(ideal.getVarCount(),
													 ideal.getGeneratorCount());
  packed->insert(ideal);

  _lcm = Ops::newTerm(ideal.getVarCount());
  _pivot = Ops::newTerm(ideal.getVarCount());
  Word* eliminated = Ops::newTerm(ideal.getVarCount());
  _euler = 0;

  EulerState state;
  state.ideal = packed;
  state.eliminated = eliminated;

  rec(state);
  Ops::deleteTerm(_lcm);
  Ops::deleteTerm(_pivot);
  _lcm = 0;
}

mpz_class PivotEulerAlg::getEuler() {
  return _euler;
}
