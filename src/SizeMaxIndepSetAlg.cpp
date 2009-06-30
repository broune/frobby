/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 Bjarke Hammersholt Roune (www.broune.com)

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
#include "SizeMaxIndepSetAlg.h"

#include "Ideal.h"
#include "Term.h"

#include <iostream>
int co;
void SizeMaxIndepSetAlg::run(Ideal& ideal) {
  ASSERT(ideal.isSquareFree());

  ideal.sortReverseLex();

  _varCount = ideal.getVarCount();
  _minExcluded = _varCount;
  _undo.resize(_varCount + 1);

  for (size_t term = 0; term < ideal.getGeneratorCount(); ++term) {
	_edges.push_back(Term::getSizeOfSupport(ideal[term], _varCount));
	for (size_t var = 0; var < _varCount; ++var) {
	  if (ideal[term][var] != 0) {
		ASSERT(ideal[term][var] == 1);
		_edges.push_back(var);
	  }
	}
  }

  _endPos = _edges.size();
  _state.clear();
  _state.resize(_varCount);

  recurse(0, 0);
  cerr << "Million situations considered: " << co / 1000000 << endl;
}

const mpz_class& SizeMaxIndepSetAlg::getMaxSize() {
  static mpz_class tmp; // TODO: BAAAAAAD
  tmp = _varCount - _minExcluded;
  return tmp;
}

size_t SizeMaxIndepSetAlg::upperBound(const State& state) const {
  size_t bound = 0;
  for (size_t var = 0; var < _varCount; ++var) {
	if (state[var] != IsNotInSet) {
	  ASSERT(state[var] == IsInSet || state[var] == IsMaybeInSet);
	  ++bound;
	}
  }

  return _varCount - bound;
}

/**
@todo get rid of recursion.

@todo change ordering of variables to something better. Which?

@todo look at using coloring to improve bound

@todo do bitsets for vars < 64
*/

bool SizeMaxIndepSetAlg::isIndependentIncludingMaybe(size_t pos) {
  while (pos != _endPos) {
	size_t nextPos = pos + _edges[pos] + 1;
	while (true) {
	  ++pos;
	  if (pos == nextPos)
		return false;
	  if (_state[_edges[pos]] == IsNotInSet)
		break;
	}
	pos = nextPos;
  }
  return true;
}

inline bool SizeMaxIndepSetAlg::couldBeDependence(size_t pos, size_t nextPos, size_t& maybeCount) {
  maybeCount = 0;
  for (size_t p = pos + 1; p != nextPos; ++p) {
	VarState varState = _state[_edges[p]];
	if (varState == IsNotInSet) {
	  // In this case the term at pos can do nothing to make the set
	  // dependent, so move on.
	  return false;
	} else if (varState == IsMaybeInSet)
	  ++maybeCount;
  }
  return true;  
}

void SizeMaxIndepSetAlg::recurse(size_t pos, size_t excluded) {
  ++co;

  ASSERT(excluded == upperBound(_state));
  ASSERT(_undo[excluded].empty());
  ASSERT(pos <= _endPos);
  ASSERT(excluded <= _varCount);

  // TODO: Can this ever happen?
  if (excluded >= _minExcluded)
	return;

  if (excluded + 1 == _minExcluded) {
	// TODO: Look into moving this to avoid recursive call.
	if (isIndependentIncludingMaybe(pos))
	  _minExcluded = excluded;
	return;
  }

  while (true) {
	if (pos == _endPos) {
	  ASSERT(excluded == upperBound(_state));
	  ASSERT(excluded < _minExcluded);
	  _minExcluded = excluded;
	  break;
	}

	size_t nextPos = pos + _edges[pos] + 1;

	size_t maybeCount;
	if (!couldBeDependence(pos, nextPos, maybeCount)) {
	  pos = nextPos;
	  continue;
	}

	if (maybeCount == 0)
	  break;

	vector<size_t>& undo = _undo[excluded];
	for (size_t p = pos + 1; p != nextPos; ++p) {
	  size_t var = _edges[p];
	  VarState& varState = _state[var];

	  if (varState != IsMaybeInSet)
		continue;

	  varState = IsNotInSet;
	  recurse(nextPos, excluded + 1);

	  if (maybeCount == 1) {
		varState = IsMaybeInSet;
		while (!undo.empty()) {
		  _state[undo.back()] = IsMaybeInSet;
		  undo.pop_back();
		}
		break;
	  }

	  varState = IsInSet;
	  --maybeCount;
	  undo.push_back(var);
	}
	break;
  }
}
