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
#include "stdinc.h"
#include "Projection.h"

#include "Term.h"
#include "Partition.h"
#include <algorithm>

size_t Projection::getRangeVarCount() const {
  return _offsets.size();
}

void Projection::reset(const Partition& partition,
                       int number) {
  _offsets.clear();

  size_t root = 0xFFFFFFFF;
  for (size_t i = 0; i < partition.getSize(); ++i) {
    if (i == partition.getRoot(i)) {
      if (number == 0) {
        root = i;
        break;
      }
      --number;
    }
  }
  ASSERT(number == 0 && root != 0xFFFFFFFF);

  for (size_t i = 0; i < partition.getSize(); ++i) {
    if (partition.getRoot(i) != root)
      continue;

    _offsets.push_back(i);
  }

  updateHasProjections();
}

void Projection::reset(const vector<size_t>& inverseProjections) {
  _offsets = inverseProjections;
  updateHasProjections();
}

void Projection::setToIdentity(size_t varCount) {
  _offsets.clear();
  for (size_t var = 0; var < varCount; ++var)
    _offsets.push_back(var);
  updateHasProjections();
}


size_t Projection::getDomainVar(size_t rangeVar) {
  ASSERT(rangeVar < getRangeVarCount());
  return _offsets[rangeVar];
}

void Projection::project(Exponent* to, const Exponent* from) const {
  size_t size = _offsets.size();
  for (size_t i = 0; i < size; ++i)
    to[i] = from[_offsets[i]];
}

void Projection::inverseProject(Term& to, const Exponent* from) const {
  size_t size = _offsets.size();
  for (size_t i = 0; i < size; ++i)
    to[_offsets[i]] = from[i];
}

size_t Projection::inverseProjectVar(size_t rangeVar) const {
  ASSERT(rangeVar < _offsets.size());
  return _offsets[rangeVar];
}

bool Projection::domainVarHasProjection(size_t var) const {
  if (var >= _domainVarHasProjection.size())
    _domainVarHasProjection.resize(var + 1);

#ifdef DEBUG
  bool has = false;
  for (size_t rangeVar = 0; rangeVar < _offsets.size(); ++rangeVar)
    if (var == inverseProjectVar(rangeVar))
      has = true;
  ASSERT(has == static_cast<bool>(_domainVarHasProjection[var]));
#endif

  return _domainVarHasProjection[var];
}

void Projection::print(FILE* file) const {
  fputs("Projection:", file);
  for (size_t var = 0; var < _offsets.size(); ++var)
    fprintf(file, " %lu", (unsigned long)_offsets[var]);
  fputc('\n', file);
}

void Projection::swap(Projection& projection) {
  _offsets.swap(projection._offsets);
  _domainVarHasProjection.swap(projection._domainVarHasProjection);
}

void Projection::updateHasProjections() {
  _domainVarHasProjection.clear();
  if (_offsets.empty())
    return;

  size_t max = *max_element(_offsets.begin(), _offsets.end());
  _domainVarHasProjection.resize(max + 1);

  for (size_t rangeVar = 0; rangeVar < _offsets.size(); ++rangeVar) {
    ASSERT(inverseProjectVar(rangeVar) < _domainVarHasProjection.size());
    _domainVarHasProjection[inverseProjectVar(rangeVar)] = true;
  }
}
