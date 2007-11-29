#ifndef IRREDUCIBLE_DECOM_FACADE_GUARD
#define IRREDUCIBLE_DECOM_FACADE_GUARD

#include "Facade.h"

class IrreducibleDecomParameters;
class BigIdeal;
class Strategy;
class TermTranslator;
class DecomConsumer;
class SliceStrategy;
class Ideal;
class DecomConsumer;

class IrreducibleDecomFacade : private Facade {
 public:
  IrreducibleDecomFacade(bool printActions,
			 const IrreducibleDecomParameters& parameters);

  // These all clear ideal.
  void computeIrreducibleDecom(BigIdeal& ideal, ostream& out);
  void computeIrreducibleDecom(Ideal& ideal, DecomConsumer* decomConsumer);
  void computeFrobeniusNumber(const vector<mpz_class>& instance,
			      BigIdeal& ideal, 
			      mpz_class& frobeniusNumber);

 private:
  void runLabelAlgorithm(Ideal& ideal, Strategy* strategy);
  void runSliceAlgorithm(Ideal& ideal, DecomConsumer* consumer,
			 SliceStrategy* strategy);

  const IrreducibleDecomParameters& _parameters;
};

#endif
