#include "stdinc.h"
#include "fplllIO.h"

#include "BigIdeal.h"
#include "Lexer.h"

namespace fplll {
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

  void readLatticeBasis(FILE* in, BigIdeal& basis) {
    Lexer lexer(in);

    vector<vector<mpz_class> > tmp;

    lexer.expect('[');
    do {
      lexer.expect('[');
      tmp.resize(tmp.size() + 1);
      do {
	mpz_class integer;
	lexer.readInteger(integer);
	tmp.back().push_back(integer);
      } while (!lexer.match(']'));

      if (tmp.front().size() != tmp.back().size()) {
	cerr << "ERROR: Row 1 has " << tmp.front().size()
	     << " entries, while row " << tmp.size()
	     << " has " << tmp.back().size()
	     << " entries." << endl;
	exit(1);
      }
    } while (!lexer.match(']'));

    ASSERT(!tmp.empty());
    VarNames names(tmp.front().size());
    basis.clearAndSetNames(names);

    for (unsigned int i = 0; i < tmp.size(); ++i) {
      basis.newLastTerm();
      for (unsigned int j = 0; j < tmp[i].size(); ++j)
	basis.getLastTermExponentRef(j) = tmp[i][j];
    }
  }
}
