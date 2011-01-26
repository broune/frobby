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
#include "ElementDeleter.h"

#include <vector>

namespace Ops = SquareFreeTermOps;

typedef vector<size_t> DivCounts;

class EulerState {
public:
  EulerState(const Ideal& idealParam) {
	const size_t varCount = idealParam.getVarCount();

	toZero();
	allocateIdealAndEliminated(varCount, idealParam.getGeneratorCount());

	ideal->insert(idealParam);
	Ops::setToIdentity(eliminated, varCount);
	sign = 1;
	ASSERT(debugIsValid());
  }

  EulerState() {
	toZero();
  }

  ~EulerState() {
	deallocate();
  }

  void cloneWithCapacityForOneMoreGenerator(const EulerState& state) {
	ASSERT(state.debugIsValid());
	if (this == &state)
	  return;

	// Add one to capacity to support adding a single additional
	// generator later.
	const size_t capacity = state.ideal->getGeneratorCount() + 1;
	const size_t varCount = state.ideal->getVarCount();

	allocateIdealAndEliminated(varCount, capacity);

	*ideal = *state.ideal;
	Ops::assign(eliminated, state.eliminated, varCount);
	sign = state.sign;

	ASSERT(debugIsValid());
  }

  void toColonSubState(const Word* pivot) {
	ASSERT(pivot != 0);

	ideal->colonReminimize(pivot);
	Ops::lcmInPlace(eliminated, pivot, ideal->getVarCount());
	ASSERT(debugIsValid());
  }

  void toColonSubState(size_t pivotVar) {
	ASSERT(pivotVar < getVarCount());

	ideal->colonReminimize(pivotVar);
	Ops::setExponent(eliminated, pivotVar, true);
	ASSERT(debugIsValid());
  }

  void toSumSubState(const Word* pivot) {
 	ideal->insertReminimize(pivot);
	ASSERT(debugIsValid());
  }

  void reset() {
	deallocate();
	toZero();
  }

  void flipSign() {
	ASSERT(sign == 1 || sign == -1);
	sign = -sign;
  }

  int getSign() const {
	ASSERT(sign == 1 || sign == -1);
	return sign;
  }

  const RawSquareFreeIdeal& getIdeal() const {
	return *ideal;
  }

  const Word* getEliminatedVars() const {
	return eliminated;
  }

  size_t getVarCount() const {
	return getIdeal().getVarCount();
  }

  size_t getNonEliminatedVarCount() const {
	const size_t eliminatedVarCount =
	  Ops::getSizeOfSupport(getEliminatedVars(), getVarCount());
	ASSERT(getVarCount() >= eliminatedVarCount);
	return getVarCount() - eliminatedVarCount;
  }

  void removeGenerator(size_t index) {
	ASSERT(index < getIdeal().getGeneratorCount());
	ideal->removeGenerator(index);
  }

#ifdef DEBUG
  bool debugIsValid() const {
	if (ideal == 0 || eliminated == 0)
	  return false;
	if (sign != 1 && sign != -1)
	  return false;
	if (!ideal->isMinimallyGenerated())
	  return false;
	if (ideal->getNotRelativelyPrime(eliminated) !=
		ideal->getGeneratorCount())
	  return false;
	if (eliminated != ideal->getGeneratorUnsafe(idealCapacity))
	  return false;
	return true;
  }
#endif

private:
  EulerState(const EulerState&); // unavailable
  EulerState& operator=(const EulerState&); // unavailable

  void toZero() {
	ideal = 0;
	eliminated = 0;
	sign = 1;
	idealCapacity = 0;
  }

  void allocateIdealAndEliminated(size_t varCount, size_t capacity) {
	if (capacity > idealCapacity) {
	  // Add 1 to the capacity to leave room for eliminated at the end.
	  size_t byteCount =
		RawSquareFreeIdeal::getBytesOfMemoryFor(varCount, capacity + 1);
	  void* buffer = new char[byteCount];
	  deallocate();

	  ideal = RawSquareFreeIdeal::construct(buffer, varCount);
	  idealCapacity = capacity;
	  eliminated = ideal->getGeneratorUnsafe(capacity);
	} else
	  RawSquareFreeIdeal::construct(ideal, varCount);
  }

  void deallocate() {
	deleteRawSquareFreeIdeal(ideal);
	idealCapacity = 0;
	// eliminated does not need to be deallocated as it is placed
	// inside the memory allocated for ideal.
  }

  RawSquareFreeIdeal* ideal;
  Word* eliminated;
  int sign;
  size_t idealCapacity;
};

bool baseCaseSimple1(mpz_class& accumulator,
					 const EulerState& state) {
  const size_t varCount = state.getVarCount();
  const RawSquareFreeIdeal& ideal = state.getIdeal();
  const Word* eliminated = state.getEliminatedVars();

  if (Ops::hasFullSupport(eliminated, varCount)) {
	// The ideal is either <1> or <0> in a ring with no variables.
	ASSERT(ideal.getGeneratorCount() <= 1);
	if (ideal.getGeneratorCount() == 0)
	  accumulator += state.getSign();
	return true;
  }

  if (!ideal.hasFullSupport(eliminated))
	return true;

  if (ideal.getGeneratorCount() == 2) {
	accumulator += state.getSign();
	return true;
  }

  return false;
}

bool baseCaseSimple2(mpz_class& accumulator,
					 const EulerState& state,
					 const DivCounts& divCounts) {
  const size_t varCount = state.getVarCount();
  const RawSquareFreeIdeal& ideal = state.getIdeal();

  for (size_t var = 0; var < varCount; ++var)
	if (divCounts[var] > 1)
	  return false;

  if (ideal.getGeneratorCount() % 2 == 1)
	accumulator -= state.getSign();
  else
	accumulator += state.getSign();
  return true;
}

bool baseCasePreconditionSimplified(mpz_class& accumulator,
									const EulerState& state,
									const DivCounts& divCounts) {
  const size_t varCount = state.getVarCount();
  const RawSquareFreeIdeal& ideal = state.getIdeal();

  if (ideal.getGeneratorCount() == 3) {
	accumulator += state.getSign() + state.getSign();
	return true;
  }

  if (ideal.getGeneratorCount() != 4 ||
	  state.getNonEliminatedVarCount() != 4)
	return false;
  for (size_t var = 0; var < varCount; ++var)
	if (divCounts[var] > 2)
	  return false;

  accumulator -= state.getSign();
  return true;
}

bool optimizeOneDivCounts(EulerState& state,
						  const DivCounts& divCounts,
						  Word* tmp) {
  const size_t varCount = state.getVarCount();
  const RawSquareFreeIdeal& ideal = state.getIdeal();

  Ops::setToIdentity(tmp, varCount);
  for (size_t var = 0; var < varCount; ++var) {
	if (divCounts[var] != 1 || Ops::getExponent(tmp, var) == 1)
	  continue;
	size_t index = ideal.getMultiple(var);
	ASSERT(ideal.getGeneratorCount() > index);
	Ops::lcmInPlace(tmp, ideal.getGenerator(index), varCount);
	state.removeGenerator(index);
	state.flipSign();
  }

  if (Ops::isIdentity(tmp, varCount))
	return false;

  state.toColonSubState(tmp);
  return true;
}

bool optimizeVarPairs(EulerState& state, Word* tmp) {
  const size_t varCount = state.getVarCount();
  const RawSquareFreeIdeal& ideal = state.getIdeal();

  for (size_t var = 0; var < varCount; ++var) {
	ideal.getLcmOfNonMultiples(tmp, var);
	Ops::lcmInPlace(tmp, state.getEliminatedVars(), varCount);
	Ops::setExponent(tmp, var, true);
	if (!Ops::hasFullSupport(tmp, varCount)) {
	  state.toColonSubState(var);
	  return true;
	}
  }
  return false;
}

bool PivotEulerAlg::processState(EulerState& state, EulerState& newState) {
  // ** First optimize state and return false if a base case is detected.
  while (true) {
	ASSERT(state.debugIsValid());

	state.getIdeal().getVarDividesCounts(_divCountsTmp);
	ASSERT(_divCountsTmp.size() == state.getVarCount());

	if (baseCaseSimple1(_euler, state))
	  return false;
	if (baseCaseSimple2(_euler, state, _divCountsTmp))
	  return false;

	if (optimizeOneDivCounts(state, _divCountsTmp, _termTmp))
	  continue;
	if (optimizeVarPairs(state, _termTmp))
	  continue;
	break;
  }

  if (baseCasePreconditionSimplified(_euler, state, _divCountsTmp))
	return false;

  // ** State is not a base case so perform a split while putting the
  // two sub-states into state and newState.
  getPivot(state, _termTmp);
  newState.cloneWithCapacityForOneMoreGenerator(state);
  newState.toSumSubState(_termTmp);
  state.toColonSubState(_termTmp);
  
  return true;
}

void PivotEulerAlg::getPivot(const EulerState& state, Word* pivot) {
  const size_t varCount = state.getVarCount();
  state.getIdeal().getVarDividesCounts(_divCountsTmp);
  Ops::setToIdentity(pivot, varCount);
  for (size_t var = 0; var < varCount; ++var) {
	if (_divCountsTmp[var] != 0) {
	  Ops::setExponent(_termTmp, var, true);
	  return;
	}
  }
  ASSERT(false);
}

PivotEulerAlg::PivotEulerAlg(const Ideal& ideal) {
  _termTmp = Ops::newTerm(ideal.getVarCount());
  try {
	_euler = 0;

	vector<EulerState*> todo;
	ElementDeleter<vector<EulerState*> > todoDeleter(todo);
	{
	  auto_ptr<EulerState> initialState(new EulerState(ideal));
	  exceptionSafePushBack(todo, initialState);
	}

	/*
	while (!todo.empty()) {
	  auto_ptr<EulerState> state(todo.back());
	  todo.pop_back();

	  auto_ptr<EulerState> newState(new EulerState());
	  while (processState(*state, *newState)) {
		exceptionSafePushBack(todo, newState);
		newState = auto_ptr<EulerState>(new EulerState());
	  }
	}
	//*/

	//*
	size_t todoCount = 1;
	while (todoCount > 0) {
	  ASSERT(todoCount - 1 < todo.size());
	  if (todoCount == todo.size()) {
		auto_ptr<EulerState> newState(new EulerState);
		exceptionSafePushBack(todo, newState);
	  }
	  ASSERT(todoCount < todo.size());

	  EulerState& state = *todo[todoCount - 1];
	  EulerState& nextState = *todo[todoCount];
	  if (processState(state, nextState))
		++todoCount;
	  else
		--todoCount;
	}
	//*/
  } catch (...) {
	Ops::deleteTerm(_termTmp);
	throw;
  }
  Ops::deleteTerm(_termTmp);
}

mpz_class PivotEulerAlg::getEuler() {
  return _euler;
}
