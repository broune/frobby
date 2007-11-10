#include "stdinc.h"
#include "fourti2.h"

#include "BigIdeal.h"
#include "Lexer.h"
#include "io.h"

namespace fourti2 {
  void writeDegrees(ostream& out, const vector<Degree>& degrees) {
    out << "1 " << degrees.size() << '\n';
    out << degrees[0];
    for (size_t i = 1; i < degrees.size(); ++i)
      out << ' ' << degrees[i];
    out << endl;
  }
  
  void readTerm(BigIdeal& ideal, Lexer& lexer) {
    ideal.newLastTerm();

    mpz_class tmp;
    lexer.readInteger(tmp);

    if (tmp > 0) {
      cerr << "ERROR: Encountered positive entry as first entry in term.\n"
	   << "This is impossible if using the required degree reverse "
	   << "lexicographic term order." << endl;
      exit(0);
    }

    for (size_t i = 0; i < ideal.getVarCount(); ++i) {
      lexer.readInteger(tmp);
      if (tmp > 0)
	ideal.getLastTermExponentRef(i) = tmp;
    }
  }

  void readLatticeBasis(istream& in, BigIdeal& basis) {
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

  void writeLatticeBasis(ostream& out, const BigIdeal& basis) {
    ASSERT(!basis.empty());

    out << basis.getGeneratorCount() << ' ' << basis[0].size() << '\n';
    for (unsigned int i = 0; i < basis.getGeneratorCount(); ++i) {
      const char* prefix = "";
      for (unsigned int j = 0; j < basis[i].size(); ++j) {
	out << prefix << basis[i][j];
	prefix = " ";
      }
      out << '\n';
    }
  }

  void readGrobnerBasis(istream& in, BigIdeal& basis) {
    Lexer lexer(in);

    unsigned int termCount;
    unsigned int dim;

    lexer.readInteger(termCount);
    lexer.readInteger(dim);

    VarNames names(dim - 1);
    basis.clearAndSetNames(names);

    for (unsigned int i = 0; i < termCount; ++i)
      readTerm(basis, lexer);

    // We cannot do this when we want to read a grading afterwards.
    //lexer.expectEOF();
  }
}
