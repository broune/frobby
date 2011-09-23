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
#include "fourti2.h"

#include "BigIdeal.h"
#include "Scanner.h"
#include "IOHandler.h"
#include "error.h"

namespace fourti2 {
  void readTerm(BigIdeal& ideal, Scanner& scanner) {
    ideal.newLastTerm();

    mpz_class tmp;
    scanner.readIntegerAndNegativeAsZero(tmp);

    if (tmp > 0)
      reportError("Encountered positive entry as first entry in term. "
                  "This is impossible if using the required degree reverse "
                  "lexicographic term order.\n");

    for (size_t i = 0; i < ideal.getVarCount(); ++i) {
      scanner.readIntegerAndNegativeAsZero(tmp);
      if (tmp > 0)
        ideal.getLastTermExponentRef(i) = tmp;
    }
  }

  void readGrobnerBasis(Scanner& scanner, BigIdeal& basis) {
    size_t termCount;
    size_t varCount;

    scanner.readSizeT(termCount);
    scanner.readSizeT(varCount);

    if (varCount == 0)
      reportError
        ("The matrix defining the Frobenius-related Grobner basis must "
         "have at least one column, and this one has none.");

    VarNames names(varCount - 1);
    basis.clearAndSetNames(names);

    for (size_t i = 0; i < termCount; ++i)
      readTerm(basis, scanner);
  }

  void readLatticeBasis(Scanner& scanner, BigIdeal& basis) {
    size_t rowCount;
    size_t columnCount;

    scanner.readSizeT(rowCount);
    scanner.readSizeT(columnCount);

    VarNames names(columnCount);
    basis.clearAndSetNames(names);

    for (unsigned int i = 0; i < rowCount; ++i) {
      basis.newLastTerm();
      for (unsigned int j = 0; j < columnCount; ++j)
        scanner.readInteger(basis.getLastTermExponentRef(j));
    }
  }

  void writeLatticeBasis(FILE* out, const BigIdeal& basis) {
    fprintf(out, "%lu %lu\n",
            (unsigned long)basis.getGeneratorCount(),
            (unsigned long)basis.getVarCount());
    for (unsigned int i = 0; i < basis.getGeneratorCount(); ++i) {
      const char* prefix = "";
      for (unsigned int j = 0; j < basis[i].size(); ++j) {
        gmp_fprintf(out, "%s%Zd", prefix, basis[i][j].get_mpz_t());
        prefix = " ";
      }
      fputc('\n', out);
    }
  }
}
