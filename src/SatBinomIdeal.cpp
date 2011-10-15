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
#include "SatBinomIdeal.h"

#include "BigIdeal.h"
#include "Matrix.h"

#include <sstream>
#include <limits>

SatBinomIdeal::SatBinomIdeal() {
}

SatBinomIdeal::SatBinomIdeal(const VarNames& names):
  _names(names) {
}

void SatBinomIdeal::insert(const vector<mpz_class>& binom) {
  ASSERT(binom.size() == getVarCount());

  _gens.push_back(binom);
}

const vector<mpz_class>& SatBinomIdeal::getGenerator(size_t index) const {
  ASSERT(index < getGeneratorCount());

  return _gens[index];
}

size_t SatBinomIdeal::getGeneratorCount() const {
  return _gens.size();
}

void SatBinomIdeal::print(FILE* out) const {
  ostringstream tmp;
  print(tmp);
  fputs(tmp.str().c_str(), out);
}

void SatBinomIdeal::print(ostream& out) const {
  out << "/---- SatBinomIdeal of " << _gens.size() << " generators:\n";
  for (vector<vector<mpz_class> >::const_iterator it = _gens.begin();
       it != _gens.end(); ++it) {
    for (vector<mpz_class>::const_iterator entry = it->begin();
         entry != it->end(); ++entry)
      out << *entry << ' ';
    out << '\n';
  }
  out << "----/ End of list.\n";
}

void SatBinomIdeal::clearAndSetNames(const VarNames& names) {
  _gens.clear();
  _names = names;
}

size_t SatBinomIdeal::getVarCount() const {
  return _names.getVarCount();
}

void SatBinomIdeal::renameVars(const VarNames& names) {
  ASSERT(names.getVarCount() == getVarCount());

  _names = names;
}

const VarNames& SatBinomIdeal::getNames() const {
  return _names;
}

void SatBinomIdeal::clear() {
  _gens.clear();
  _names.clear();
}

void SatBinomIdeal::reserve(size_t size) {
  _gens.reserve(size);
}

void SatBinomIdeal::removeGeneratorsWithLeadingZero() {
  size_t gen = 0;
  while (gen < getGeneratorCount()) {
    if (getGenerator(gen)[0] == 0) {
      _gens[gen] = _gens.back();
      _gens.pop_back();
    } else
      ++gen;
  }
}

void SatBinomIdeal::removeGeneratorsWithoutLeadingZero() {
  size_t gen = 0;
  while (gen < getGeneratorCount()) {
    if (getGenerator(gen)[0] != 0) {
      _gens[gen] = _gens.back();
      _gens.pop_back();
    } else
      ++gen;
  }
}

void SatBinomIdeal::getInitialIdeal(BigIdeal& ideal) const {
  ideal.clearAndSetNames(getNames());
  ideal.reserve(getGeneratorCount());

  for (size_t gen = 0; gen < getGeneratorCount(); ++gen) {
    ideal.newLastTerm();
    for (size_t var = 0; var < getVarCount(); ++var)
      if (getGenerator(gen)[var] > 0)
        ideal.getLastTermExponentRef(var) = getGenerator(gen)[var];
  }
}

vector<mpz_class>& SatBinomIdeal::getLastBinomRef() {
  ASSERT(!_gens.empty());

  return _gens.back();
}

void SatBinomIdeal::newLastTerm() {
  _gens.resize(_gens.size() + 1);
  _gens.back().resize(_names.getVarCount());
}

bool SatBinomIdeal::hasZeroEntry() const {
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
    for (size_t var = 0; var < getVarCount(); ++var)
      if (getGenerator(gen)[var] == 0)
        return true;
  return false;
}

bool SatBinomIdeal::initialIdealIsWeaklyGeneric() const {
  vector<mpz_class> v(getVarCount());

  for (size_t gen1 = 0; gen1 < getGeneratorCount(); ++gen1) {
    for (size_t gen2 = gen1 + 1; gen2 < getGeneratorCount(); ++gen2) {
      const vector<mpz_class>& g1 = getGenerator(gen1);
      const vector<mpz_class>& g2 = getGenerator(gen2);

      // Skip if g1 and g2 are different in each entry.
      bool sharesEntry = false;
      for (size_t var = 0; var < getVarCount(); ++var) {
        if (g1[var] == g2[var] && g1[var] > 0) {
          sharesEntry = true;
          break;
        }
      }
      if (!sharesEntry)
        continue;

      if (isPointFreeBody(g1, g2))
        return false;
    }
  }

  return true;
}

bool SatBinomIdeal::isPointFreeBody(const vector<mpz_class>& a,
                                    const vector<mpz_class>& b) const {
  ASSERT(a.size() == getVarCount());
  ASSERT(b.size() == getVarCount());

  vector<mpz_class> rhs(getVarCount());

  // Set rhs to max(0,g1,g2)-1.
  for (size_t var = 0; var < getVarCount(); ++var) {
    rhs[var] = a[var] > b[var] ? a[var] : b[var];
    if (rhs[var] < 0)
      rhs[var] = 0;
    rhs[var] -= 1;
  }

  return !isDominating(rhs);
}

bool SatBinomIdeal::isPointFreeBody(const vector<mpz_class>& a,
                                    const vector<mpz_class>& b,
									const vector<mpz_class>& c) const {
  ASSERT(a.size() == getVarCount());
  ASSERT(b.size() == getVarCount());

  vector<mpz_class> rhs(getVarCount());

  // Set rhs to max(0,g1,g1+g2)-1.
  for (size_t var = 0; var < getVarCount(); ++var) {
    rhs[var] = a[var] > b[var] ? a[var] : b[var];
	rhs[var] = rhs[var] > c[var] ? rhs[var] : c[var];
    if (rhs[var] < 0)
      rhs[var] = 0;
    rhs[var] -= 1;
  }

  return !isDominating(rhs);
}

bool SatBinomIdeal::isInterior(const vector<mpz_class>& a,
							   const vector<mpz_class>& b) const {
  ASSERT(a.size() == b.size());

  if (!isPointFreeBody(a, b))
	return false;

  for (size_t var = 1; var < a.size(); ++var)
	if (a[var] <= 0 && b[var] <= 0)
	  return false;
  return true;
}

namespace {
  bool hasCycle(size_t gen, vector<char>& color, const SatBinomIdeal& ideal) {
	// 0 = Not seen before.
	// 1 = Exploring now.
	// 2 = Already explored. Does not lead to cycle.
	if (color[gen] == 1)
	  return true;
	if (color[gen] == 2)
	  return false;
	color[gen] = 1;
	for (size_t g = 0; g < ideal.getGeneratorCount(); ++g)
	  if (ideal.isInteriorEdge(gen, g) &&
		  !ideal.isTerminatingEdge(gen, g) &&
		  hasCycle(g, color, ideal))
		return true;
	color[gen] = 2;
	return false;
  }
}

bool SatBinomIdeal::validate() const {
  // check the graph satisifies what we think it should.

  bool generic = !hasZeroEntry();
  if (!generic)
	return true;

  // termination.
  vector<char> color(getGeneratorCount());
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
	if (hasCycle(gen, color, *this))
	  return false;

  // out-degree 1 when generic
  for (size_t from = 0; from < getGeneratorCount(); ++from) {
	const vector<mpz_class>& fromGen = getGenerator(from);

	size_t outDegree = 0;
	for (size_t to = 0; to < getGeneratorCount(); ++to) {
	  if (isInteriorEdge(from, to))
		++outDegree;
	}
	if (isInterior(fromGen, fromGen)) {
	  if (outDegree != 0)
		return false;
	} else {
	  if (outDegree != 1)
		return false;
	}
  }

  return true;
}

bool SatBinomIdeal::isInteriorEdge(size_t from, size_t to) const {
  const vector<mpz_class>& fromGen = getGenerator(from);
  const vector<mpz_class>& toGen = getGenerator(to);

  if (isInterior(fromGen, fromGen))
	return false;
  if (isInterior(toGen, toGen))
	return false;

  vector<mpz_class> sum(fromGen.size());
  for (size_t var = 0; var < fromGen.size(); ++var)
	sum[var] = fromGen[var] + toGen[var];
  return isInterior(toGen, sum);
}

bool SatBinomIdeal::isTerminatingEdge(size_t from, size_t to) const {
  if (!isInteriorEdge(from, to))
	return false;

  const vector<mpz_class> fromGen = getGenerator(from);
  const vector<mpz_class> toGen = getGenerator(to);

  vector<mpz_class> sum(fromGen.size());
  for (size_t var = 0; var < fromGen.size(); ++var)
	sum[var] = fromGen[var] + toGen[var];
  return isPointFreeBody(fromGen, sum);
}

void SatBinomIdeal::getDoubleTriangleCount(mpz_class& count) const {
  vector<mpz_class> sum(getVarCount());

  count = 0;
  for (size_t gen1 = 0; gen1 < getGeneratorCount(); ++gen1) {
    for (size_t gen2 = gen1 + 1; gen2 < getGeneratorCount(); ++gen2) {
      const vector<mpz_class>& g1 = getGenerator(gen1);
      const vector<mpz_class>& g2 = getGenerator(gen2);

      // Set sum = g1 + g2.
      for (size_t var = 0; var < getVarCount(); ++var)
        sum[var] = g1[var] + g2[var];

      if (isPointFreeBody(g1, sum) && isPointFreeBody(g2, sum))
        ++count;
    }
  }
}

bool SatBinomIdeal::isGeneric() const {
  return !hasZeroEntry() && initialIdealIsWeaklyGeneric();
}

SatBinomIdeal& SatBinomIdeal::operator=(const SatBinomIdeal& ideal) {
  _gens = ideal._gens;
  _names = ideal._names;
  return *this;
}

bool SatBinomIdeal::isDominating(const vector<mpz_class>& v) const {
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen) {
    bool dom = true;
    for (size_t var = 0; var < getVarCount(); ++var) {
      if (v[var] < getGenerator(gen)[var]) {
        dom = false;
        break;
      }
    }
    if (dom)
      return true;
  }
  return false;
}

bool SatBinomIdeal::isGenerator
(const vector<mpz_class>& v) const {
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
    if (getGenerator(gen) == v)
      return true;
  return false;
}

void SatBinomIdeal::projectVar(size_t var) {
  ASSERT(var < getVarCount());

  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
    _gens[gen].erase(_gens[gen].begin() + var);
  _names.projectVar(var);
}

void SatBinomIdeal::getMatrix(Matrix& matrix) const {
  matrix.resize(getGeneratorCount(), getVarCount());
  for (size_t gen = 0; gen < getGeneratorCount(); ++gen)
	for (size_t var = 0; var < getVarCount(); ++var)
	  matrix(gen, var) = _gens[gen][var];
}
