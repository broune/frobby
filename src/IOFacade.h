#ifndef IO_FACADE_GUARD
#define IO_FACADE_GUARD

#include "Facade.h"
#include <vector>

class BigIdeal;

class IOFacade : private Facade {
 public:
  IOFacade(bool printActions);

  bool isValidMonomialIdealFormat(const char* format);
  void readIdeal(FILE* in, BigIdeal& ideal, const char* format = "monos");
  void readIdeals(FILE* in, vector<BigIdeal*>& ideal,
				  const char* format = "monos"); // inserts the read ideals
  void writeIdeal(FILE* out, BigIdeal& ideal, const char* format = "monos");

  void readFrobeniusInstance(FILE* in, vector<mpz_class>& instance);
  void readFrobeniusInstanceWithGrobnerBasis
    (FILE* in, BigIdeal& ideal, vector<mpz_class>& instance);
  void writeFrobeniusInstance(FILE* out, vector<mpz_class>& instance);

  bool isValidLatticeFormat(const char* format);
  void readLattice(FILE* in, BigIdeal& ideal, const char* format);
  void writeLattice(FILE* out, const BigIdeal& ideal, const char* format);
};

#endif
