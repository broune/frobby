#ifndef FOURTI2_GUARD
#define FOURTI2_GUARD

class BigIdeal;
class Lexer;

namespace fourti2 {
  void readTerm(BigIdeal& ideal, Lexer& lexer);

  void readLatticeBasis(FILE* in, BigIdeal& basis);
  void writeLatticeBasis(FILE* out, const BigIdeal& basis);

  void readGrobnerBasis(FILE* in, BigIdeal& basis);
}

#endif
