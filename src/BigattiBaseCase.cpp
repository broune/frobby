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

  enumerateScarfComplex(state);

  ++_totalBaseCasesEver;
  return true;
}

void BigattiBaseCase::enumerateScarfComplex(const BigattiState& state) {
  ASSERT(_state->getVarCount() == _varCount);

  const Ideal& ideal = _state->getIdeal();

  size_t needed = ideal.getGeneratorCount() + 1; 
  if (_states.size() < needed)
	_states.resize(needed);
  for (size_t i = 0; i < _states.size(); ++i)
	_states[i].term.reset(_varCount);

  ASSERT(!ideal.isZeroIdeal());
  _states[0].plus = true;
  _states[0].pos = ideal.begin();
  ASSERT(_states[0].term.isIdentity());

  Ideal::const_iterator stop = ideal.end();
 
  size_t current = 0;
  while (true) {
	ASSERT(current < _states.size());
	State& state = _states[current];
	if (state.pos == stop) {
	  _lcm.product(state.term, _state->getMultiply());
	  if (state.plus)
		outputPlus(_lcm);
	  else
		outputMinus(_lcm);
	  if (current == 0)
		break;
	  --current;
	} else {
	  ASSERT(current + 1 < _states.size());
	  State& next = _states[current + 1];

	  next.term.lcm(state.term, *state.pos);
	  next.plus = !state.plus;
	  next.pos = ++state.pos;

	  if (!ideal.strictlyContains(next.term))
		++current;
	}
  }
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

  size_t needed = ideal.getGeneratorCount() + 1; 
  if (_states.size() < needed)
	_states.resize(needed);
  for (size_t i = 0; i < _states.size(); ++i)
	_states[i].term.reset(_varCount);

  ASSERT(!ideal.isZeroIdeal());
  _states[0].plus = true;
  _states[0].pos = ideal.begin();
  ASSERT(_states[0].term.isIdentity());
  
  Ideal::const_iterator stop = ideal.end();
 
  size_t current = 0;
  while (true) {
	ASSERT(current < _states.size());
	State& state = _states[current];
	if (state.pos == stop) {
	  _lcm.product(state.term, _state->getMultiply());
	  if (state.plus)
		outputPlus(_lcm);
	  else
		outputMinus(_lcm);
	  if (current == 0)
		break;
	  --current;
	} else {
	  ASSERT(current + 1 < _states.size());
	  State& next = _states[current + 1];

	  next.term.lcm(state.term, *state.pos);
	  next.plus = !state.plus;
	  next.pos = ++state.pos;
	  ++current;
	}
  }
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
