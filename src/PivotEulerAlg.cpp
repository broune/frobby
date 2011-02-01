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

  bool toColonSubState(const Word* pivot) {
	ASSERT(pivot != 0);
	ASSERT(Ops::isRelativelyPrime(pivot, getEliminatedVars(), getVarCount()));

	const size_t genCountBefore = getIdeal().getGeneratorCount();
	ideal->colonReminimize(pivot);
	Ops::lcmInPlace(eliminated, pivot, ideal->getVarCount());
	ASSERT(debugIsValid());
	return genCountBefore != getIdeal().getGeneratorCount();
  }

  bool toColonSubState(size_t pivotVar) {
	ASSERT(pivotVar < getVarCount());
	ASSERT(Ops::getExponent(getEliminatedVars(), pivotVar) == 0);

	const size_t genCountBefore = getIdeal().getGeneratorCount();
	ideal->colonReminimize(pivotVar);
	Ops::setExponent(eliminated, pivotVar, true);
	ASSERT(debugIsValid());
	return genCountBefore != getIdeal().getGeneratorCount();
  }

  void toColonSubStateNoReminimizeNecessary(size_t pivotVar) {
	ASSERT(pivotVar < getVarCount());
	ASSERT(Ops::getExponent(getEliminatedVars(), pivotVar) == 0);

	ideal->colon(pivotVar);
	Ops::setExponent(eliminated, pivotVar, true);
	ASSERT(debugIsValid());
  }

  void toColonSubStateNoReminimizeNecessary(Word* pivot) {
	ASSERT(pivot != 0);
	ASSERT(Ops::isRelativelyPrime(getEliminatedVars(), pivot, getVarCount()));

	ideal->colon(pivot);
	Ops::lcmInPlace(eliminated, pivot, getVarCount());
	ASSERT(debugIsValid());
  }

  void makeSumSubState(const Word* pivot, EulerState& subState) {
	ASSERT(&subState != this);

	const size_t capacity = ideal->getGeneratorCount();
	const size_t varCount = ideal->getVarCount();
	subState.allocateIdealAndEliminated(varCount, capacity);

	subState.ideal->insertNonMultiples(pivot, *ideal);
	Ops::lcm(subState.eliminated, eliminated, pivot, varCount);
	subState.sign = sign;
	subState.flipSign();

	ASSERT(subState.debugIsValid());
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
  const size_t genCount = ideal.getGeneratorCount();

  if (!ideal.hasFullSupport(eliminated))
	return true;
  if (genCount > 2)
	return false;

  if (genCount == 0)
	accumulator += state.getSign();
  else if (genCount == 2)
	accumulator += state.getSign();
  else {
	ASSERT(genCount == 1);
	if (!Ops::hasFullSupport(eliminated, varCount))
	  accumulator -= state.getSign();
  }
  return true;
}

bool optimizeSimpleFromDivCounts(mpz_class& accumulator,
								 EulerState& state,
								 DivCounts& divCounts,
								 Word* termTmp) {
  const size_t varCount = state.getVarCount();
  const size_t genCount = state.getIdeal().getGeneratorCount();
  ASSERT(genCount > 2);

  for (size_t var = 0; var < varCount; ++var) {
	ASSERT(genCount == state.getIdeal().getGeneratorCount());
	ASSERT(genCount > 2);

	if (divCounts[var] < genCount - 2)
	  continue;

	if (divCounts[var] == genCount - 1) {
	  const Word* nonMultiple =
		state.getIdeal().getGenerator(state.getIdeal().getNonMultiple(var));
	  Ops::lcm(termTmp, nonMultiple, state.getEliminatedVars(), varCount);
	  Ops::setExponent(termTmp, var, 1);
	  if (Ops::hasFullSupport(termTmp, varCount))
		accumulator += state.getSign();

	  if (state.toColonSubState(var))
		return true;
	  divCounts[var] = 0;
	} else if (divCounts[var] == genCount - 2) {
	  state.getIdeal().getLcmOfNonMultiples(termTmp, var);
	  Ops::lcmInPlace(termTmp, state.getEliminatedVars(), varCount);
	  Ops::setExponent(termTmp, var, 1);
	  if (Ops::hasFullSupport(termTmp, varCount))
		accumulator -= state.getSign();

	  if (state.toColonSubState(var))
		return true;
	  divCounts[var] = 0;
	} else {
	  ASSERT(divCounts[var] == genCount);

	  state.toColonSubStateNoReminimizeNecessary(var);
	  divCounts[var] = 0;
	}
  }
  return false;
}

bool baseCaseSimple2(mpz_class& accumulator,
					 const EulerState& state,
					 const DivCounts& divCounts) {
  const size_t varCount = state.getVarCount();
  const RawSquareFreeIdeal& ideal = state.getIdeal();
  const size_t genCount = state.getIdeal().getGeneratorCount();

  for (size_t var = 0; var < varCount; ++var)
	if (divCounts[var] != 1 && divCounts[var] != genCount)
	  return false;

  if ((ideal.getGeneratorCount() & 1) == 0)
	accumulator += state.getSign();
  else
	accumulator -= state.getSign();
  return true;
}

bool baseCasePreconditionSimplified(mpz_class& accumulator,
									const EulerState& state,
									const DivCounts& divCounts) {
  const RawSquareFreeIdeal& ideal = state.getIdeal();

  if (ideal.getGeneratorCount() == 3) {
	accumulator += state.getSign() + state.getSign();
	return true;
  }
  return false;
}

bool optimizeOneDivCounts(EulerState& state,
						  DivCounts& divCounts,
						  Word* tmp) {
  const size_t varCount = state.getVarCount();
  const RawSquareFreeIdeal& ideal = state.getIdeal();

  size_t var = 0;
  for (; var < varCount; ++var) {
	if (divCounts[var] != 1)
	  continue;
	size_t index = ideal.getMultiple(var);
	ASSERT(ideal.getGeneratorCount() > index);
	Ops::assign(tmp, ideal.getGenerator(index), varCount);
	state.removeGenerator(index);
	state.flipSign();
	goto searchForMore;
  }
  return false;

 searchForMore:
  for (++var; var < varCount; ++var) {
	if (divCounts[var] != 1 || Ops::getExponent(tmp, var) == 1)
	  continue;
	size_t index = ideal.getMultiple(var);
	ASSERT(ideal.getGeneratorCount() > index);
	Ops::lcmInPlace(tmp, ideal.getGenerator(index), varCount);

	state.removeGenerator(index);
	state.flipSign();
  }

  if (state.toColonSubState(tmp) || ideal.getGeneratorCount() <= 2)
	return true;

  Ops::toZeroAtSupport(tmp, &divCounts.front(), varCount);
  return false;
}

bool optimizeVarPairs(EulerState& state, Word* tmp, DivCounts& divCounts) {
  const size_t varCount = state.getVarCount();
  const RawSquareFreeIdeal& ideal = state.getIdeal();
  const Word* eliminated = state.getEliminatedVars();

  for (size_t var = 0; var < varCount; ++var) {
	if (Ops::getExponent(eliminated, var) == 1)
	  continue;
	ideal.getLcmOfNonMultiples(tmp, var);
	Ops::lcmInPlace(tmp, state.getEliminatedVars(), varCount);
	Ops::setExponent(tmp, var, true);
	if (!Ops::hasFullSupport(tmp, varCount)) {
	  if (state.toColonSubState(var))
		return true;
	  divCounts[var] = 0;
	}
  }
  return false;
}

bool PivotEulerAlg::processState(EulerState& state, EulerState& newState) {
  //cout << "*** Before simplification:\n" << state.getIdeal();
  ++_stepsPerformed;

  // ** First optimize state and return false if a base case is detected.
  bool doAllPairs = false;

  while (true) {
	ASSERT(state.debugIsValid());

	if (baseCaseSimple1(_euler, state))
	  return false;

	state.getIdeal().getVarDividesCounts(_divCountsTmp);

	if (optimizeOneDivCounts(state, _divCountsTmp, _termTmp))
	  continue;
	if (optimizeSimpleFromDivCounts(_euler, state, _divCountsTmp, _termTmp))
	  continue;
	if (doAllPairs && optimizeVarPairs(state, _termTmp, _divCountsTmp))
	continue;
	break;
  }

  if (doAllPairs && baseCasePreconditionSimplified(_euler, state, _divCountsTmp))
	return false;

  //cout << "+++ After simplification/base case:\n" << state.getIdeal();


  // ** State is not a base case so perform a split while putting the
  // two sub-states into state and newState.

  size_t pivotVar = 0;
  for (size_t var = 1; var < _divCountsTmp.size(); ++var)
	if (_divCountsTmp[var] > _divCountsTmp[pivotVar])
	  pivotVar = var;

  //cout << "Pivot is " << pivotVar;

  Ops::setToIdentity(_termTmp, state.getVarCount());
  Ops::setExponent(_termTmp, pivotVar, 1);

  state.makeSumSubState(_termTmp, newState);
  state.toColonSubState(pivotVar);

  return true;
}

void PivotEulerAlg::getPivot(const EulerState& state, Word* pivot) {
  ASSERT(false);
}

PivotEulerAlg::PivotEulerAlg(const Ideal& ideal) {
  _stepsPerformed = 0;
  _termTmp = Ops::newTerm(ideal.getVarCount());
  try {
	_euler = 0;

	vector<EulerState*> todo;
	ElementDeleter<vector<EulerState*> > todoDeleter(todo);
	{
	  auto_ptr<EulerState> initialState(new EulerState(ideal));
	  exceptionSafePushBack(todo, initialState);
	}

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
  } catch (...) {
	Ops::deleteTerm(_termTmp);
	throw;
  }
  Ops::deleteTerm(_termTmp);
}

mpz_class PivotEulerAlg::getEuler() {
  return _euler;
}
