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
  _maxSize = -1;
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

  recurse((size_t)0, _varCount);
  cerr << "Million situations considered: " << co / 1000000 << endl;
}

const mpz_class& SizeMaxIndepSetAlg::getMaxSize() {
  return _maxSize;
}

size_t SizeMaxIndepSetAlg::upperBound(const State& state) const {
  size_t bound = 0;
  for (size_t var = 0; var < _varCount; ++var) {
	if (state[var] != IsNotInSet) {
	  ASSERT(state[var] == IsInSet || state[var] == IsMaybeInSet);
	  ++bound;
	}
  }

  return bound;
}

  /**
@todo get rid of recursion.

@todo change ordering of variables to something better. Which?

@todo look at using coloring to improve bound

@todo do bitsets for vars < 64

@todo try out looking through remaining sets to detect dependence early.

@todo index terms by last variable in current ordering. makes checking easy. Hmm... or first? maybe just sort them by this, does that have same effect?
   */
void SizeMaxIndepSetAlg::recurse(size_t pos, size_t bound) {
  ++co;

  ASSERT(bound == upperBound(_state));
  ASSERT(_undo[bound].empty());
  ASSERT(pos <= _endPos);
  ASSERT(bound <= _varCount);

  if (bound <= _maxSize)
	return;

  while (pos != _endPos) {
	size_t supportSize = _edges[pos];
	++pos;
	size_t maybeCount = 0;
	for (size_t var2 = 0; var2 < supportSize; ++var2) {
	  size_t var = _edges[pos + var2];
	  if (_state[var] == IsNotInSet) {
		// In this case the term at pos can do nothing to make the set
		// dependent, so move on.
		maybeCount = 0;
		goto moveOn;
	  } else if (_state[var] == IsMaybeInSet)
		++maybeCount;
	}

	if (maybeCount == 0) {
	  // In this case term makes this set surely dependent.
	  return;
	}

	{
	  for (size_t var2 = 0; var2 < supportSize; ++var2) {
		size_t var = _edges[pos + var2];
		if (_state[var] == IsMaybeInSet) {
		  _state[var] = IsNotInSet;
		  recurse(pos + supportSize, bound - 1);

		  if (maybeCount == 1) {
			_state[var] = IsMaybeInSet;
			while (!_undo[bound].empty()) {
			  _state[_undo[bound].back()] = IsMaybeInSet;
			  _undo[bound].pop_back();
			}
			return;
		  } else {
			ASSERT(maybeCount >= 2);

			_state[var] = IsInSet;
			--maybeCount;
			_undo[bound].push_back(var);
		  }
		}
	  }
	}
  moveOn:;

	pos += supportSize;
  }

  ASSERT(bound == upperBound(_state));
  if (bound > _maxSize)
	_maxSize = bound;
}
