#ifndef IO_FACADE_GUARD
#define IO_FACADE_GUARD

#include "Facade.h"

class BigIdeal;

class IOFacade : private Facade {
 public:
  IOFacade(bool printActions);

  bool isValidMonomialIdealFormat(const char* format);
  void readIdeal(istream& in, BigIdeal& ideal, const char* format = "monos");
  void writeIdeal(ostream& out, BigIdeal& ideal, const char* format = "monos");

  void readFrobeniusInstance(istream& in, vector<mpz_class>& instance);
  void readFrobeniusInstanceWithGrobnerBasis
    (istream& in, BigIdeal& ideal, vector<mpz_class>& instance);
  void writeFrobeniusInstance(ostream& out, vector<mpz_class>& instance);

  bool isValidLatticeFormat(const char* format);
  void readLattice(istream& in, BigIdeal& ideal, const char* format);
  void writeLattice(ostream& out, const BigIdeal& ideal, const char* format);
};

#endif
