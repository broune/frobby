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
#include "BigattiBaseCase.h"

#include "BigattiState.h"
#include <algorithm>

BigattiBaseCase::BigattiBaseCase(size_t varCount):
 _varCount(varCount),
 _maxCount(varCount),
 _lcm(varCount),
 _taken(varCount),
 _lcms(varCount),
 _output(varCount),
 _one(1),
 _minusOne(-1),
 _totalBaseCasesEver(0),
 _totalTermsOutputEver(0),
 _printDebug(false) {
  for (size_t i = 0; i < varCount; ++i)
    _lcms.insert(_lcm);
}

bool BigattiBaseCase::genericBaseCase(const BigattiState& state) {
  ASSERT(state.getIdeal().getVarCount() == _varCount);
  if (baseCase(state))
	return true;
  ASSERT(_state == &state); // done by baseCase.

  if (!state.getIdeal().isWeaklyGeneric())
	return false;

  Term term(_varCount);
  generic(term, _state->getIdeal().begin(), true);

  ++_totalBaseCasesEver;
  return true;
}

void BigattiBaseCase::generic(const Term& term, Ideal::const_iterator pos, bool plus) {
  if (pos == _state->getIdeal().end()) {
    _lcm.product(term, _state->getMultiply());
	if (plus)
      outputPlus(_lcm);
    else
      outputMinus(_lcm);
  } else {
	generic(term, pos + 1, plus);
	Term lcm(_varCount);
	lcm.lcm(term, *pos);
	if (!_state->getIdeal().strictlyContains(lcm))
	  generic(lcm, pos + 1, !plus);
  }
}

bool BigattiBaseCase::baseCase(const BigattiState& state) {
  ASSERT(state.getIdeal().getVarCount() == _varCount);
  _state = &state;

  if (simpleBaseCase())
    return true;

  if (state.getIdeal().getGeneratorCount() > _varCount)
	return false;

  state.getIdeal().getLcm(_lcm);
  if (state.getIdeal().getGeneratorCount() > _lcm.getSizeOfSupport())
	return false;

  fill(_maxCount.begin(), _maxCount.end(), 0);
  Ideal::const_iterator end = state.getIdeal().end();
  Ideal::const_iterator it = state.getIdeal().begin();
  for (; it != end; ++it) {
	bool hasMax = false;
	for (size_t var = 0; var < _varCount; ++var) {
	  ASSERT((*it)[var] <= _lcm[var]);
	  if ((*it)[var] == _lcm[var] && _lcm[var] > 0) {
		hasMax = true;
		_maxCount[var] += 1;
		if (_maxCount[var] > 1)
		  return false;
	  }
	}
	if (!hasMax)
	  return false;
  }

  allCombinations();

  ++_totalBaseCasesEver;
  return true;
}

void BigattiBaseCase::setPrintDebug(bool value) {
  _printDebug = value;
}

bool BigattiBaseCase::simpleBaseCase() {
  const Ideal& ideal = _state->getIdeal();
  size_t genCount = ideal.getGeneratorCount();
  const Term& multiply = _state->getMultiply();

  if (genCount > 2)
    return false;

  outputPlus(multiply);
  if (genCount == 0)
    return true;

  _lcm.product(multiply, ideal[0]);
  outputMinus(_lcm);
  if (genCount == 1)
    return true;

  ASSERT(genCount == 2);
  _lcm.product(multiply, ideal[1]);
  outputMinus(_lcm);

  _lcm.lcm(ideal[0], ideal[1]);
  _lcm.product(_lcm, multiply);
  outputPlus(_lcm);

  ++_totalBaseCasesEver;
  return true;
}

void BigattiBaseCase::allCombinations() {
  ASSERT(_state->getVarCount() == _varCount);
  ASSERT(_state->getIdeal().getGeneratorCount() <= _varCount);
  ASSERT(_taken.size() == _varCount);
  ASSERT(_lcms.getGeneratorCount() == _varCount);

  const Ideal& ideal = _state->getIdeal();
  size_t genCount = ideal.getGeneratorCount();

  for (size_t i = 0; i < genCount; ++i) {
    Term::setToIdentity(_lcms[i], _varCount);
	_taken[i] = false;
  }
  _takenCount = 0;

  do {
    _lcm.product(_lcms[0], _state->getMultiply());
    if ((_takenCount & 1) == 0)
      outputPlus(_lcm);
    else
      outputMinus(_lcm);
  } while (nextCombination());
}

bool BigattiBaseCase::nextCombination() {
  const Ideal& ideal = _state->getIdeal();
  size_t genCount = ideal.getGeneratorCount();

  // Consider this as adding 1 in binary. We get a carry
  // for every taken element (1), so find the first not
  // taken (0).
  size_t gen = 0;
  while (true) {
    if (gen == genCount)
      return false; // We have gone through all combinations.
    if (!_taken[gen])
      break;
    ++gen;
  }

  take(gen);
  while (gen > 0) {
    --gen;
    drop(gen);
  }
  return true;
}

void BigattiBaseCase::take(size_t gen) {
  ASSERT(!_taken[gen]);
  _taken[gen] = true;
  ++_takenCount;

  // Update _lcms[gen]. We are taking gen, so the lcm is the
  // lcm of the previous level and ideal[gen].
  const Ideal& ideal = _state->getIdeal();
  if (gen == ideal.getGeneratorCount() - 1)
      copy(ideal[gen], ideal[gen] + _varCount, _lcms[gen]); // No previous level
  else
      Term::lcm(_lcms[gen], ideal[gen], _lcms[gen + 1], _varCount);
}

void BigattiBaseCase::drop(size_t gen) {
  ASSERT(_taken[gen]);
  _taken[gen] = false;
  --_takenCount;

  // Update _lcms[gen]. We are not taking gen, so the lcm is the
  // same as it was at the previous level.
  const Ideal& ideal = _state->getIdeal();
  if (gen == ideal.getGeneratorCount() - 1)
    Term::setToIdentity(_lcms[gen], _varCount); // No previous level
  else
    copy(_lcms[gen + 1], _lcms[gen + 1] + _varCount, _lcms[gen]);
}

void BigattiBaseCase::outputPlus(const Term& term) {
  if (_printDebug) {
	fputs("Debug: Outputting term +", stderr);
	term.print(stderr);
	fputs(".\n", stderr);
  }

  ++_totalTermsOutputEver;
  _output.add(_one, term);
}

void BigattiBaseCase::outputMinus(const Term& term) {
  if (_printDebug) {
	fputs("Debug: Outputting term -", stderr);
	term.print(stderr);
	fputs(".\n", stderr);
  }

  ++_totalTermsOutputEver;
  _output.add(_minusOne, term);
}

void BigattiBaseCase::feedOutputTo(CoefTermConsumer& consumer) {
  _output.feedTo(consumer);
}

size_t BigattiBaseCase::getTotalBaseCasesEver() const {
  return _totalBaseCasesEver;
}

size_t BigattiBaseCase::getTotalTermsOutputEver() const {
  return _totalTermsOutputEver;
}

size_t BigattiBaseCase::getTotalTermsInOutput() const {
  return _output.getTermCount();
}
