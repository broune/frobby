#ifndef FOURTI2_GUARD
#define FOURTI2_GUARD

class BigIdeal;
class Scanner;

namespace fourti2 {
  void readGrobnerBasis(Scanner& scanner, BigIdeal& basis);
  void readLatticeBasis(Scanner& scanner, BigIdeal& basis);

  void writeLatticeBasis(FILE* out, const BigIdeal& basis);
}

#endif
