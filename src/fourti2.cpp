#include "stdinc.h"
#include "fourti2.h"

#include "BigIdeal.h"
#include "Lexer.h"
#include "io.h"

namespace fourti2 {
  void readTerm(BigIdeal& ideal, Lexer& lexer) {
    ideal.newLastTerm();

    mpz_class tmp;
    lexer.readInteger(tmp);

    if (tmp > 0) {
      fputs("ERROR: Encountered positive entry as first entry in term.\n"
	    "This is impossible if using the required degree reverse "
	    "lexicographic term order.\n", stderr);
      exit(1);
    }

    for (size_t i = 0; i < ideal.getVarCount(); ++i) {
      lexer.readInteger(tmp);
      if (tmp > 0)
	ideal.getLastTermExponentRef(i) = tmp;
    }
  }

  void readLatticeBasis(FILE* in, BigIdeal& basis) {
    Lexer lexer(in);

    unsigned int rowCount;
    unsigned int columnCount;

    lexer.readInteger(rowCount);
    lexer.readInteger(columnCount);

    VarNames names(columnCount);
    basis.clearAndSetNames(names);

    for (unsigned int i = 0; i < rowCount; ++i) {
      basis.newLastTerm();
      for (unsigned int j = 0; j < columnCount; ++j)
	lexer.readInteger(basis.getLastTermExponentRef(j));
    }
  }

  void writeLatticeBasis(FILE* out, const BigIdeal& basis) {
    ASSERT(!basis.empty());

    fprintf(out, "%u %u\n",
	    (unsigned int)basis.getGeneratorCount(),
	    (unsigned int)basis[0].size());
    for (unsigned int i = 0; i < basis.getGeneratorCount(); ++i) {
      const char* prefix = "";
      for (unsigned int j = 0; j < basis[i].size(); ++j) {
	gmp_fprintf(out, "%s%Zd", prefix, basis[i][j].get_mpz_t());
	prefix = " ";
      }
      fputc('\n', out);
    }
  }

  void readGrobnerBasis(FILE* in, BigIdeal& basis) {
    Lexer lexer(in);

    size_t termCount;
    size_t varCount;

    lexer.readInteger(termCount);
    lexer.readInteger(varCount);

    VarNames names(varCount - 1);
    basis.clearAndSetNames(names);

    for (size_t i = 0; i < termCount; ++i)
      readTerm(basis, lexer);
  }
}
