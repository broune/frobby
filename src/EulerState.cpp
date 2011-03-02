/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2011 Bjarke Hammersholt Roune (www.broune.com)

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
#include "EulerState.h"

#include "RawSquareFreeTerm.h"
#include "Ideal.h"

#include <limits>

namespace Ops = SquareFreeTermOps;

EulerState::EulerState(const Ideal& idealParam) {
  const size_t varCount = idealParam.getVarCount();

	toZero();
	allocateIdealAndEliminated(varCount, idealParam.getGeneratorCount());

	ideal->insert(idealParam);
	Ops::setToIdentity(eliminated, varCount);
	sign = 1;
	ASSERT(debugIsValid());
}

EulerState::EulerState() {
	toZero();
}

EulerState::~EulerState() {
  deallocate();
}

void EulerState::inPlaceStdSplit(size_t pivotVar, EulerState& subState) {
  ASSERT(pivotVar < getVarCount());
  ASSERT(this != &subState);

  makeSumSubState(pivotVar, subState);
  toColonSubState(pivotVar);
}

void EulerState::inPlaceStdSplit(Word* pivot, EulerState& subState) {
  ASSERT(pivot != 0);
  ASSERT(this != &subState);

  makeSumSubState(pivot, subState);
  toColonSubState(pivot);
}

void EulerState::inPlaceGenSplit(size_t pivotIndex, EulerState& subState) {
  ASSERT(pivotIndex < getIdeal().getGeneratorCount());
  ASSERT(this != &subState);

  const Word* pivot = getIdeal().getGenerator(pivotIndex);
  subState = *this;
  subState.removeGenerator(pivotIndex);
  subState.toColonSubState(pivot);
  subState.flipSign();
  removeGenerator(pivotIndex);
}

bool EulerState::toColonSubState(const Word* pivot) {
  ASSERT(pivot != 0);
  ASSERT(Ops::isRelativelyPrime(pivot, getEliminatedVars(), getVarCount()));

  const size_t genCountBefore = getIdeal().getGeneratorCount();
  ideal->colonReminimize(pivot);
  Ops::lcmInPlace(eliminated, pivot, ideal->getVarCount());
  ASSERT(debugIsValid());
  return genCountBefore != getIdeal().getGeneratorCount();
}

bool EulerState::toColonSubState(size_t pivotVar) {
  ASSERT(pivotVar < getVarCount());
  ASSERT(Ops::getExponent(getEliminatedVars(), pivotVar) == 0);

  const size_t genCountBefore = getIdeal().getGeneratorCount();
  ideal->colonReminimize(pivotVar);
  Ops::setExponent(eliminated, pivotVar, true);
  ASSERT(debugIsValid());
  return genCountBefore != getIdeal().getGeneratorCount();
}

void EulerState::toColonSubStateNoReminimizeNecessary(size_t pivotVar) {
  ASSERT(pivotVar < getVarCount());
  ASSERT(Ops::getExponent(getEliminatedVars(), pivotVar) == 0);

  ideal->colon(pivotVar);
  Ops::setExponent(eliminated, pivotVar, true);
  ASSERT(debugIsValid());
}

void EulerState::toColonSubStateNoReminimizeNecessary(Word* pivot) {
  ASSERT(pivot != 0);
  ASSERT(Ops::isRelativelyPrime(getEliminatedVars(), pivot, getVarCount()));

  ideal->colon(pivot);
  Ops::lcmInPlace(eliminated, pivot, getVarCount());
  ASSERT(debugIsValid());
}

void EulerState::makeSumSubState(size_t pivotVar, EulerState& subState) {
  ASSERT(&subState != this);

  const size_t capacity = ideal->getGeneratorCount();
  const size_t varCount = ideal->getVarCount();
  subState.allocateIdealAndEliminated(varCount, capacity);

  subState.ideal->insertNonMultiples(pivotVar, *ideal);
  Ops::assign(subState.eliminated, eliminated, varCount);
  Ops::setExponent(subState.eliminated, pivotVar, 1);
  subState.sign = sign;
  subState.flipSign();

  ASSERT(subState.debugIsValid());
}

void EulerState::makeSumSubState(Word* pivot, EulerState& subState) {
  ASSERT(&subState != this);
  const size_t varCount = ideal->getVarCount();

  const size_t capacity = ideal->getGeneratorCount() + 1;
  subState.allocateIdealAndEliminated(varCount, capacity);

  subState.ideal->insertNonMultiples(pivot, *ideal);
  subState.ideal->insert(pivot);
  Ops::assign(subState.eliminated, eliminated, varCount);
  subState.sign = sign;

  ASSERT(subState.debugIsValid());
}

void EulerState::reset() {
  deallocate();
  toZero();
}

void EulerState::compactEliminatedVariablesIfProfitable() {
  const size_t varCount = getVarCount();
  const size_t varsLeft = getNonEliminatedVarCount();
  if (Ops::getWordCount(varCount) > Ops::getWordCount(varsLeft)) {
	ideal->compact(eliminated);
	Ops::setToIdentity(eliminated, ideal->getVarCount());
	ASSERT(debugIsValid());
  }
}

void EulerState::print(FILE* out) {
  fputs("** an Euler characteristic algorithm state:\n", out);
  fprintf(out, "State sign: %s\n", sign == 1 ? "+1" : "-1");
  fputs("Eliminated: ", out);
  Ops::print(out, eliminated, ideal->getVarCount());
  fputc('\n', out);
  ideal->print(out);
}

#ifdef DEBUG
bool EulerState::debugIsValid() const {
  if (ideal == 0 || eliminated == 0)
	return false;
  if (sign != 1 && sign != -1)
	return false;
  if (!Ops::isValid(eliminated, ideal->getVarCount()))
	return false;
  if (!ideal->isMinimallyGenerated())
	return false;
  if (ideal->getVarCount() != 0 &&
	  ideal->getNotRelativelyPrime(eliminated) !=
	  ideal->getGeneratorCount())
	return false;
  return true;
}
#endif

EulerState& EulerState::operator=(const EulerState& state) {
  if (&state == this)
	return *this;

  const size_t capacity = state.getIdeal().getGeneratorCount();
  const size_t varCount = state.getIdeal().getVarCount();
  allocateIdealAndEliminated(varCount, capacity);
  *ideal = state.getIdeal();
  Ops::assign(eliminated, state.eliminated, varCount);
  sign = state.sign;

  ASSERT(debugIsValid());
  return *this;
}

void EulerState::toZero() {
  ideal = 0;
  eliminated = 0;
  sign = 1;
  idealCapacity = 0;
}

void EulerState::allocateIdealAndEliminated(size_t varCount, size_t capacity) {
  size_t bytesNeeded =
	RawSquareFreeIdeal::getBytesOfMemoryFor(varCount, capacity + 1);
  // capacity can't be max as we need to be able to add 1 to it.
  if (bytesNeeded == 0 || capacity == numeric_limits<size_t>::max())
	throw bad_alloc();
  void* buffer = 0;
  if (bytesNeeded > idealCapacity) {
	// Add 1 to the capacity to leave room for eliminated at the end.
	buffer = new char[bytesNeeded];
	deallocate();
	idealCapacity = bytesNeeded;
  } else
	buffer = ideal;
  ASSERT(bytesNeeded <= idealCapacity);

  ideal = RawSquareFreeIdeal::construct(buffer, varCount);
  eliminated = ideal->getGeneratorUnsafe(capacity);
}

void EulerState::deallocate() {
  deleteRawSquareFreeIdeal(ideal);
  idealCapacity = 0;
  // eliminated does not need to be deallocated as it is placed
  // inside the memory allocated for ideal.
}

size_t EulerState::getNonEliminatedVarCount() const {
  const size_t eliminatedVarCount =
	Ops::getSizeOfSupport(getEliminatedVars(), getVarCount());

  ASSERT(getVarCount() >= eliminatedVarCount);
  return getVarCount() - eliminatedVarCount;
}
