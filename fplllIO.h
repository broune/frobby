#ifndef FPLLL_IO_GUARD
#define FPLLL_IO_GUARD

class BigIdeal;

namespace fplll {
  void writeLatticeBasis(ostream& out, const BigIdeal& basis);

  void readLatticeBasis(istream& in,
			unsigned int rowCount,
			unsigned int columnCount,
			BigIdeal& basis);

  void reduceLatticeBasis(BigIdeal& basis);

  void addMultiple(BigIdeal& basis,
		   unsigned int add,
		   unsigned int addTo,
		   const mpz_class& mult);

  void makeZeroesInLatticeBasis(BigIdeal& basis);
}

#endif
