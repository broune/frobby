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
#ifndef IDEAL_COMPARATOR_GUARD
#define IDEAL_COMPARATOR_GUARD

class Ideal;
class BigIdeal;
class TermTranslator;

// These three classes are in the same file because they use
// templates in a way so that they all have the same implementation.
// They all define the same order given the natural bijection between
// the three different objects, and it is true of all of them that the
// order of the generators matter, and that non-minimal generators
// also matter. So to get an actual canonical order on the ideals as
// mathematical objects, each ideal must be in a canonical form already.

// Defines a total order on Ideal. The ideals compared must have the same
// number of variables, though the number can vary between individual
// calls to operator().
class IdealComparator {
 public:
  bool operator()(const Ideal& a, const Ideal& b) const;
  bool operator()(const Ideal* a, const Ideal* b) const;
};

// Defines a total order on Ideal based on the translated values. Each
// ideal must have the same number of variables as the translator.
class TranslatedIdealComparator {
 public:
  TranslatedIdealComparator(TermTranslator& translator);
  bool operator()(const Ideal& a, const Ideal& b) const;
  bool operator()(const Ideal* a, const Ideal* b) const;
 private:
  TermTranslator& _translator;
};

// Defines a total order on BigIdeal.
class BigIdealComparator {
 public:
  bool operator()(const BigIdeal& a, const BigIdeal& b) const;
  bool operator()(const BigIdeal* a, const BigIdeal* b) const;
};

namespace IdealComparatorHelpers {
  bool idealLessThan(const BigIdeal& a, const BigIdeal& b);
  bool idealLessThan(const Ideal& a, const Ideal& b);
  bool idealLessThan(const Ideal& a, const Ideal& b,
                     TermTranslator& translator);
}

inline bool IdealComparator::operator()(const Ideal& a, const Ideal& b) const {
  return IdealComparatorHelpers::idealLessThan(a, b);
}

inline bool IdealComparator::operator()(const Ideal* a, const Ideal* b) const {
  return IdealComparatorHelpers::idealLessThan(*a, *b);
}

inline bool TranslatedIdealComparator::operator()(const Ideal& a,
                                                 const Ideal& b) const {
  return IdealComparatorHelpers::idealLessThan(a, b, _translator);
}

inline bool TranslatedIdealComparator::operator()(const Ideal* a,
                                                 const Ideal* b) const {
  return IdealComparatorHelpers::idealLessThan(*a, *b, _translator);
}

inline bool BigIdealComparator::operator()(const BigIdeal& a,
                                          const BigIdeal& b) const {
  return IdealComparatorHelpers::idealLessThan(a, b);
}

inline bool BigIdealComparator::operator()(const BigIdeal* a, const
                                          BigIdeal* b) const {
  return IdealComparatorHelpers::idealLessThan(*a, *b);
}

#endif
