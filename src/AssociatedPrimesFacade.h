#ifndef ASSOCIATED_PRIMES_GUARD
#define ASSOCIATED_PRIMES_GUARD

#include "Facade.h"

class IrreducibleDecomParameters;
class BigIdeal;

class AssociatedPrimesFacade : Facade {
 public:
  AssociatedPrimesFacade(bool printActions);

  // Clears ideal.
  void computeAPUsingIrrDecom(BigIdeal& ideal,
			      const IrreducibleDecomParameters& params,
			      FILE* out);
};


#endif
