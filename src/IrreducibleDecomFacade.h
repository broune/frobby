#ifndef IRREDUCIBLE_DECOM_FACADE_GUARD
#define IRREDUCIBLE_DECOM_FACADE_GUARD

#include "Facade.h"

class IrreducibleDecomParameters;
class BigIdeal;
class Strategy;
class TermTranslator;
class TermTree;

class IrreducibleDecomFacade : private Facade {
 public:
  IrreducibleDecomFacade(bool printActions,
			 const IrreducibleDecomParameters& parameters);

  // Clears ideal.
  void computeIrreducibleDecom(BigIdeal& ideal, ostream& out);

  // Clears ideal.
  void computeFrobeniusNumber(const vector<mpz_class>& instance,
			      BigIdeal& ideal, 
			      mpz_class& frobeniusNumber);

 private:
  void runAlgorithm(TermTree* tree, TermTranslator* translator,
		    Strategy* strategy);
  Strategy* addStrategy(vector<Strategy*>& strategies,
			Strategy* oldStrategy,
			Strategy* newStrategy);

  const IrreducibleDecomParameters& _parameters;
};

#endif
