/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

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
#ifndef PROJECTION_GUARD
#define PROJECTION_GUARD

#include <vector>

class Partition;
class Term;

// Recall that a function maps elements from its domain to its
// range. Projection represents a function that maps variables from
// its range ring to its domain ring, where some variables can be
// mapped to zero.
class Projection {
 public:
  Projection() {}

  void reset(const Partition& partition, int set);
  void reset(const vector<size_t>& inverseProjections);
  void setToIdentity(size_t varCount);

  size_t getRangeVarCount() const;

  size_t getDomainVar(size_t rangeVar);

  void project(Exponent* to, const Exponent* from) const;

  void inverseProject(Term& to, const Exponent* from) const;
  size_t inverseProjectVar(size_t rangeVar) const;

  void swap(Projection& projection);

  void print(FILE* file) const;

  bool domainVarHasProjection(size_t var) const;

 private:
  void updateHasProjections();

  vector<size_t> _offsets;
  mutable vector<int> _domainVarHasProjection;
};

#endif
