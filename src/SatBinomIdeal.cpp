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

#include <sstream>

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

  // Set rhs to max(0,g1,g1+g2)-1.
  for (size_t var = 0; var < getVarCount(); ++var) {
	rhs[var] = a[var] > b[var] ? a[var] : b[var];
	if (rhs[var] < 0)
	  rhs[var] = 0;
	rhs[var] -= 1;
  }

  return !isDominating(rhs);
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
