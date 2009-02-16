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

#ifndef VAR_SORTER_GUARD
#define VAR_SORTER_GUARD

#include "VarNames.h"
#include <vector>
#include "Term.h"

class TermTranslator;

struct VarSorter {
  // Makes this object represent a sorted permutation of names.
  VarSorter(const VarNames& names);

  // Permute the variables according to the permutation generated at
  // construction.
  void permute(vector<mpz_class>& term);
  void permute(Exponent* term);
  void permute(TermTranslator* translator);

  // Set the parameter equal the names this object represents in
  // sorted order.
  void getOrderedNames(VarNames& names);

private:
  vector<size_t> _permutation;
  VarNames _names;
  vector<mpz_class> _bigTmpTerm;
  Term _tmpTerm;
};

#endif
