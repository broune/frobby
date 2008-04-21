#ifndef FPLLL_IO_GUARD
#define FPLLL_IO_GUARD

class BigIdeal;
class Scanner;

namespace fplll {
  void readLatticeBasis(Scanner& scanner, BigIdeal& basis);

  void writeLatticeBasis(FILE* out, const BigIdeal& basis);
}

#endif
