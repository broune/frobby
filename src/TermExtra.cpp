/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)
   Copyright (C) 2010 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "TermExtra.h"

#include <algorithm>
#include <vector>

Exponent median(const Exponent* a, size_t varCount) {
  if (varCount == 0)
    return 0;

  Term t(a, varCount);
  Exponent* middle = t.begin() + varCount / 2;
  nth_element(t.begin(), middle, t.end());
  return *middle;
}

Exponent medianPositive(const Exponent* a, size_t varCount) {
  vector<Exponent> exps;
  exps.reserve(varCount);
  for (size_t var = 0; var < varCount; ++var)
    if (a[var] > 0)
      exps.push_back(a[var]);
  if (exps.empty())
    return 0;

  vector<Exponent>::iterator middle = exps.begin() + exps.size() / 2;
  nth_element(exps.begin(), middle, exps.end());
  return *middle;
}

void totalDegree(mpz_class& res, const Exponent* a, size_t varCount) {
  res = a[0];
  for (size_t var = 1; var < varCount; ++var)
    res += a[var];
}

Exponent minimumPositive(const Exponent* a, size_t varCount) {
  for (size_t var = 0; var < varCount; ++var) {
    if (a[var] != 0) {
      Exponent min = a[var];
      for (; var < varCount; ++var)
        if (a[var] != 0 && a[var] < min)
          min = a[var];
      return min;
    }
  }
  return 0;
}

Exponent maximum(const Exponent* a, size_t varCount) {
  Exponent max = 0;
  for (size_t var = 0; var < varCount; ++var)
    if (a[var] > max)
      max = a[var];
  return max;
}
