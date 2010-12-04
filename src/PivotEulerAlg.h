/* Frobby: Software for monomial ideal computations.
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
#ifndef PIVOT_EULER_ALG_GUARD
#define PIVOT_EULER_ALG_GUARD

class Ideal;
class RawSquareFreeIdeal;

#include <vector>

class EulerState;

class PivotEulerAlg {
 public:
  PivotEulerAlg(const Ideal& ideal);
  mpz_class getEuler();

 private:
  bool rec(EulerState& state, EulerState& newState);
  void taskRun(RawSquareFreeIdeal& ideal);
  void getPivot(const EulerState& state, Word* pivot);

  mpz_class _euler;
  Word* _lcm;
  Word* _pivot;
  vector<size_t> _counts;
};

#endif
