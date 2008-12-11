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
#include "fplllIO.h"

#include "BigIdeal.h"
#include "Scanner.h"

#include <cstdlib>

namespace fplll {
  void readLatticeBasis(Scanner& scanner, BigIdeal& basis) {
    vector<vector<mpz_class> > tmp;

    scanner.expect('[');
	while (!scanner.match(']')) {
      scanner.expect('[');
      tmp.resize(tmp.size() + 1);
	  while (!scanner.match(']')) {
		mpz_class integer;
		scanner.readInteger(integer);
		tmp.back().push_back(integer);
      }

      if (tmp.front().size() != tmp.back().size()) {
		fprintf
		  (stderr, 
		   "ERROR: Row 1 has %lu entries, while row %lu has %lu entries.\n",
		   (unsigned long)tmp.front().size(),
		   (unsigned long)tmp.size(),
		   (unsigned long)tmp.back().size());
		exit(1);
      }
    }

    VarNames names(tmp.empty() ? 0 : tmp.front().size());
    basis.clearAndSetNames(names);

    for (unsigned int i = 0; i < tmp.size(); ++i) {
      basis.newLastTerm();
      for (unsigned int j = 0; j < tmp[i].size(); ++j)
		basis.getLastTermExponentRef(j) = tmp[i][j];
    }
  }

  void writeLatticeBasis(FILE* out, const BigIdeal& basis) {
    fputs("[\n", out);
    for (unsigned int i = 0; i < basis.getGeneratorCount(); ++i) {
      fputs(" [", out);
      const char* prefix = "";
      for (unsigned int j = 0; j < basis[i].size(); ++j) {
		gmp_fprintf(out, "%s%Zd", prefix, basis[i][j].get_mpz_t());
		prefix = " ";
      }
      fputs("]\n", out);
    }
    fputs("]\n", out);
  }
}
