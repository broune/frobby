#include "stdinc.h"
#include "fplllIO.h"

#include "BigIdeal.h"
#include "Lexer.h"

namespace fplll {
  void writeLatticeBasis(ostream& out, const BigIdeal& basis) {
    out << "[\n";
    for (unsigned int i = 0; i < basis.size(); ++i) {
      out << " [";
      const char* prefix = "";
      for (unsigned int j = 0; j < basis[i].size(); ++j) {
	out << prefix << basis[i][j];
	prefix = " ";
      }
      out << "]\n";
    }
    out << "]\n";
  }

  void readLatticeBasis(istream& in, BigIdeal& basis) {
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
    unsigned int rowCount = basis.size();
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
}
