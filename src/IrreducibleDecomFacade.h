#ifndef IRREDUCIBLE_DECOM_FACADE_GUARD
#define IRREDUCIBLE_DECOM_FACADE_GUARD

#include "Facade.h"
#include <vector>

class IrreducibleDecomParameters;
class BigIdeal;
class Strategy;
class TermTranslator;
class SliceStrategy;
class Ideal;
class TermConsumer;


// TODO: move this elsewhere
#include "frobby.h"
#include "TermConsumer.h"
#include "TermTranslator.h"
class ExternalConsumer : public TermConsumer {
 public:
  ExternalConsumer(Frobby::ExternalTermConsumer* consumer, TermTranslator* trans):
	_varCount(trans->getNames().getVarCount()),
	_consumer(consumer),
	_translator(trans),
	_exponentVector(new mpz_ptr[_varCount]) {
  }

  ~ExternalConsumer() {
	delete[] _exponentVector;
  }

  virtual void consume(const Term& term) {
	for (size_t var = 0; var < _varCount; ++var)
	  _exponentVector[var] =
		const_cast<mpz_ptr>(_translator->getExponent(var, term).get_mpz_t());
	_consumer->consume(_exponentVector);
  }

 private:
  size_t _varCount;															
  Frobby::ExternalTermConsumer* _consumer;
  TermTranslator* _translator;
  mpz_ptr* _exponentVector;
};

class IrreducibleDecomFacade : private Facade {
 public:
  IrreducibleDecomFacade(bool printActions,
						 const IrreducibleDecomParameters& parameters);

  void printLabels(BigIdeal& ideal, FILE* out, const string& format);

  // These all clear ideal.
  void computeIrreducibleDecom(BigIdeal& ideal, FILE* out, const string& format);
  void computeAlexanderDual(BigIdeal& ideal, FILE* out, const string& format);
  void computeAlexanderDual(BigIdeal& ideal,
							const vector<mpz_class>& point, FILE* out,
							const string& format);
  void computeAlexanderDual(BigIdeal& ideal,
							const vector<mpz_class>& point,
							Frobby::ExternalTermConsumer* consumer);

  void computeFrobeniusNumber(const vector<mpz_class>& instance,
							  BigIdeal& ideal, 
							  mpz_class& frobeniusNumber,
							  vector<mpz_class>& vector);

  void computeIrreducibleDecom(Ideal& ideal, TermConsumer* consumer);

 private:
  void computeIrreducibleDecom(Ideal& ideal,
							   TermConsumer* consumer,
							   bool preMinimized);
  void computeAlexanderDual(BigIdeal& ideal,
							const vector<mpz_class>& point,
							bool useLcm,
							FILE* out,
							const string& format);

  void runSliceAlgorithm(Ideal& ideal, SliceStrategy* strategy);

  const IrreducibleDecomParameters& _parameters;
};

#endif
