#include "stdinc.h"
#include "fourti2.h"

#include "BigIdeal.h"
#include "Scanner.h"
#include "io.h"

namespace fourti2 {
  void readTerm(BigIdeal& ideal, Scanner& scanner) {
    ideal.newLastTerm();

    mpz_class tmp;
    scanner.readInteger(tmp);

    if (tmp > 0) {
      fputs("ERROR: Encountered positive entry as first entry in term.\n"
	    "This is impossible if using the required degree reverse "
	    "lexicographic term order.\n", stderr);
      exit(1);
    }

    for (size_t i = 0; i < ideal.getVarCount(); ++i) {
      scanner.readInteger(tmp);
      if (tmp > 0)
	ideal.getLastTermExponentRef(i) = tmp;
    }
  }

  void readGrobnerBasis(Scanner& scanner, BigIdeal& basis) {
    size_t termCount;
    size_t varCount;

    scanner.readInteger(termCount);
    scanner.readInteger(varCount);

    VarNames names(varCount - 1);
    basis.clearAndSetNames(names);

    for (size_t i = 0; i < termCount; ++i)
      readTerm(basis, scanner);
  }

  void readLatticeBasis(Scanner& scanner, BigIdeal& basis) {
    unsigned int rowCount;
    unsigned int columnCount;

    scanner.readInteger(rowCount);
    scanner.readInteger(columnCount);

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
