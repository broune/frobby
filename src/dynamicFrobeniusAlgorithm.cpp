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
#include "dynamicFrobeniusAlgorithm.h"

#include <set>
#include <algorithm>

mpz_class dynamicFrobeniusAlgorithm(const vector<mpz_class>& numbers) {
  if (numbers.size() == 2)
    return numbers[0] * numbers[1] - numbers[0] - numbers[1];

  set<mpz_class> representable;
  representable.insert(0);
  mpz_class minNumber = *min_element(numbers.begin(), numbers.end());

  mpz_class maximumNotRepresentable = 0;
  int representableRun = 0;
  mpz_class number = 1;

  while (representableRun < minNumber) {
    bool isNumberRepresentable = false;

    for (size_t i = 0; i < numbers.size(); ++i) {
      if (representable.find(number - numbers[i]) !=
      representable.end()) {
    isNumberRepresentable = true;
    break;
      }
    }

    if (isNumberRepresentable) {
      representable.insert(number);
      ++representableRun;
    } else {
      maximumNotRepresentable = number;
      representableRun = 0;
    }

    ++number;
  }

  return maximumNotRepresentable;
}
