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

// TODO: move implementation to .cpp file

#include "VarNames.h"
#include <algorithm>

struct VarSorter {
  VarSorter(VarNames& names):
    _names(names),
	_tmp(names.getVarCount()) {
	_permutation.reserve(names.getVarCount());
    for (size_t i = 0; i < names.getVarCount(); ++i)
      _permutation.push_back(i);
    sort(_permutation.begin(), _permutation.end(), *this);
  }

  bool operator()(size_t a, size_t b) const {
    return
      _names.getName(_permutation[a]) <
      _names.getName(_permutation[b]);
  }

  void getOrderedNames(VarNames& names) {
    names.clear();
    for (size_t i = 0; i < _permutation.size(); ++i)
      names.addVar(_names.getName(_permutation[i]));
  }

  void permute(vector<mpz_class>& term) {
	ASSERT(term.size() == _tmp.size());
    _tmp.swap(term);
    for (size_t i = 0; i < _permutation.size(); ++i)
	  mpz_swap(term[i].get_mpz_t(), _tmp[_permutation[i]].get_mpz_t());
  }

private:
  vector<size_t> _permutation;
  VarNames _names;
  vector<mpz_class> _tmp;
};
