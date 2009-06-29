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

void SizeMaxIndepSetAlg::run(const Ideal& ideal) {
  ASSERT(ideal.isSquareFree());

  _varCount = ideal.getVarCount();
  _maxSize = -1;
  _endPos = ideal.end();

  recurse(State(_varCount), ideal.begin());
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
@todo Update upper bound instead of recomputing it every time

@todo sort terms so that those with similar support get together

@todo preallocate states and get rid of recursion.

@todo change ordering of variables to something better. Which?

@todo look at using coloring to improve bound

@todo do bitsets for vars < 64

@todo do sparse encoding for many vars

@todo try out looking through remaining sets to detect dependence early.

@todo index terms by last variable in current ordering. makes checking easy. Hmm... or first? maybe just sort them by this, does that have same effect?
   */
void SizeMaxIndepSetAlg::recurse(const State& currentState, Pos pos) {

  if (upperBound(currentState) <= _maxSize)
	return;
  State state = currentState;

  for (; pos != _endPos; ++pos) {
	size_t maybeCount = 0;
	for (size_t var = 0; var < _varCount; ++var) {
	  if ((*pos)[var] == 1 && state[var] == IsNotInSet) {
		// In this case the term at pos can do nothing to make the set
		// dependent, so move on.
		maybeCount = 0;
		goto moveOn;
	  }

	  if ((*pos)[var] == 1 && state[var] == IsMaybeInSet)
		++maybeCount;
	}

	if (maybeCount == 0) {
	  // In this case term makes this set surely dependent.
	  return;
	}

	{
	  for (size_t var = 0; var < _varCount; ++var) {
		if ((*pos)[var] == 1 && state[var] == IsMaybeInSet) {
		  state[var] = IsNotInSet;
		  recurse(state, pos);
		  state[var] = IsInSet;
		  --maybeCount;
		  
		  if (maybeCount == 0)
			return;
		}
	  }
	}
  moveOn:;
  }

  if (upperBound(state) > _maxSize)
	_maxSize = upperBound(state);
}
