#ifndef IO_FACADE_GUARD
#define IO_FACADE_GUARD

#include "Facade.h"
#include <vector>

class BigIdeal;

class IOFacade : private Facade {
 public:
  IOFacade(bool printActions);

  bool isValidMonomialIdealFormat(const string& format);
  void readIdeal(FILE* in, BigIdeal& ideal, const string& format);
  void readIdeals(FILE* in, vector<BigIdeal*>& ideal,
				  const string& format); // inserts the read ideals
  void writeIdeal(FILE* out, BigIdeal& ideal, const string& format);

  void readFrobeniusInstance(FILE* in, vector<mpz_class>& instance);
  void readFrobeniusInstanceWithGrobnerBasis
    (FILE* in, BigIdeal& ideal, vector<mpz_class>& instance);
  void writeFrobeniusInstance(FILE* out, vector<mpz_class>& instance);

  bool readAlexanderDualInstance
	(FILE* in, BigIdeal& ideal, vector<mpz_class>& term,
	 const string& format);

  bool isValidLatticeFormat(const string& format);
  void readLattice(FILE* in, BigIdeal& ideal, const string& format);
  void writeLattice(FILE* out, const BigIdeal& ideal, const string& format);
};

#endif
