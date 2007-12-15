#ifndef FOURTI2_GUARD
#define FOURTI2_GUARD

class BigIdeal;
class Lexer;

namespace fourti2 {
  void writeDegrees(ostream& out, const vector<Degree>& degrees);

  void readTerm(BigIdeal& ideal, Lexer& lexer);

  void readLatticeBasis(FILE* in, BigIdeal& basis);
  void writeLatticeBasis(ostream& out, const BigIdeal& basis);

  void readGrobnerBasis(FILE* in, BigIdeal& basis);
}

#endif
