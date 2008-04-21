#ifndef IO_FACADE_GUARD
#define IO_FACADE_GUARD

#include "Facade.h"
#include <vector>

class BigIdeal;
class Scanner;

class IOFacade : private Facade {
 public:
  IOFacade(bool printActions);

  bool isValidMonomialIdealFormat(const string& format);
  void readIdeal(Scanner& in, BigIdeal& ideal);
  void readIdeals(Scanner& in,
				  vector<BigIdeal*>& ideal); // inserts the read ideals
  void writeIdeal(FILE* out, BigIdeal& ideal, const string& format);

  void readFrobeniusInstance(Scanner& in, vector<mpz_class>& instance);
  void readFrobeniusInstanceWithGrobnerBasis
    (Scanner& in, BigIdeal& ideal, vector<mpz_class>& instance);
  void writeFrobeniusInstance(FILE* out, vector<mpz_class>& instance);

  bool readAlexanderDualInstance
	(Scanner& in, BigIdeal& ideal, vector<mpz_class>& term);

  bool isValidLatticeFormat(const string& format);
  void readLattice(Scanner& in, BigIdeal& ideal);
  void writeLattice(FILE* out, const BigIdeal& ideal, const string& format);
};

#endif
