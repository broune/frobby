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
#include "IdealComparator.h"

#include "BigIdeal.h"
#include "Ideal.h"
#include "TermTranslator.h"
#include "TermPredicate.h"
#include "Term.h"

TranslatedIdealComparator::
TranslatedIdealComparator(TermTranslator& translator):
  _translator(translator) {
}

namespace IdealComparatorHelpers {
  template<class MyIdeal, class MyTermCompare>
  bool idealLessThanImpl(const MyIdeal& a,
                         const MyIdeal& b,
                         const MyTermCompare& predicate) {
    ASSERT(a.getVarCount() == b.getVarCount());

    for (size_t term = 0; term < a.getGeneratorCount(); ++term) {
      if (term == b.getGeneratorCount())
        return true;

      if (predicate(a[term], b[term]))
        return true;
      if (predicate(b[term], a[term]))
        return false;
    }
    ASSERT(a.getGeneratorCount() <= b.getGeneratorCount());

    return false;
  }

  bool idealLessThan(const BigIdeal& a, const BigIdeal& b) {
    if (a.getNames() < b.getNames())
      return true;
    if (b.getNames() < a.getNames())
      return false;

    return idealLessThanImpl(a, b, BigIdeal::bigTermCompare);
  }

  bool idealLessThan(const Ideal& a, const Ideal& b) {
    ASSERT(a.getVarCount() == b.getVarCount());

    return idealLessThanImpl(a, b, ReverseLexComparator(a.getVarCount()));
  }

  bool idealLessThan(const Ideal& a, const Ideal& b,
                     TermTranslator& translator) {
    ASSERT(a.getVarCount() == translator.getVarCount());
    ASSERT(b.getVarCount() == translator.getVarCount());

    return idealLessThanImpl(a, b, TranslatedReverseLexComparator(translator));
  }
}
