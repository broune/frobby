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
#include "lattice.h"

#include "BigIdeal.h"

void addMultiple(BigIdeal& basis,
         unsigned int add,
         unsigned int addTo,
         const mpz_class& mult) {
  if (mult == 0)
    return;

  for (unsigned int i = 0; i < basis[0].size(); ++i)
    basis[addTo][i] += basis[add][i] * mult;
}

void makeZeroesInLatticeBasis(BigIdeal& basis) {
  ASSERT(!basis.empty());
  unsigned int rowCount = basis.getGeneratorCount();
  unsigned int columnCount = basis[0].size();

  for (unsigned int col = columnCount - 1; col >= 1; --col) {
    for (unsigned int i = 0; i < rowCount; ++i) {
      mpz_class sign;
      if (basis[i][col] == 1)
    sign = 1;
      else if (basis[i][col] == -1)
    sign = -1;
      else
    continue;

      for (unsigned int j = 0; j < rowCount; ++j) {
    if (j == i)
      continue;
    addMultiple(basis, i, j, -1 * sign * basis[j][col]);
      }

      break;
    }
  }
}
