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
#ifndef SIZE_MAX_INDEP_SET_ALG
#define SIZE_MAX_INDEP_SET_ALG

#include "Ideal.h"

class SizeMaxIndepSetAlg {
 public:
  /** May rearrange the generators of ideal. */
  void run(Ideal& ideal);

  const mpz_class& getMaxSize();

 private:
  enum VarState {
	IsInSet,
	IsNotInSet,
	IsMaybeInSet = 0
  };
  typedef vector<VarState> State;
  typedef Ideal::const_iterator Pos;

  vector<vector<size_t> > _undo;

  size_t upperBound(const State& state) const;

  void recurse(State& currentState, size_t pos, size_t bound);

  size_t _varCount;
  mpz_class _maxSize;
  size_t _endPos;

  vector<size_t> _edges;
};

#endif
