#ifndef FOURTI2_GUARD
#define FOURTI2_GUARD

class BigIdeal;
class Lexer;

namespace fourti2 {
  void writeDegrees(ostream& out, const vector<Degree>& degrees);

  void readTerm(BigIdeal& ideal, Lexer& lexer);

  void readLatticeBasis(istream& in, BigIdeal& basis);
  void writeLatticeBasis(ostream& out, const BigIdeal& basis);

  void readGrobnerBasis(istream& in, BigIdeal& basis);
}

#endif
