#include "stdinc.h"
#include "frobby.h"

#include "BigIdeal.h"
#include "IrreducibleDecomFacade.h"
#include "IrreducibleDecomParameters.h"
#include "BigTermConsumer.h"
#include "TermTranslator.h"
#include "Term.h"

class ExternalConsumerAdapter : public BigTermConsumer {
public:
  ExternalConsumerAdapter(Frobby::ExternalTermConsumer* consumer,
						  size_t varCount):
	_consumer(consumer),
	_varCount(varCount),
	_term(new mpz_ptr[varCount]) {
	ASSERT(consumer != 0);
  }

  virtual ~ExternalConsumerAdapter() {
  }

  virtual void consume(const Term& term, TermTranslator* translator) {
	ASSERT(term.getVarCount() == _varCount);
	for (size_t var = 0; var < _varCount; ++var)
	  _term[var] = const_cast<mpz_ptr>
		(translator->getExponent(var, term).get_mpz_t());
	_consumer->consume(_term);
  }

  virtual void consume(mpz_ptr* term) {
	_consumer->consume(term);
  }

private:
  Frobby::ExternalTermConsumer* _consumer;
  size_t _varCount;
  mpz_ptr* _term;
};

Frobby::ExternalTermConsumer::~ExternalTermConsumer() {
}

Frobby::ExternalIdeal::ExternalIdeal(size_t variableCount, size_t expectedGeneratorCount) {
  _data = new BigIdeal(VarNames(variableCount));

  BigIdeal* ideal = (BigIdeal*)_data;
  ideal->reserve(expectedGeneratorCount);
}

Frobby::ExternalIdeal::ExternalIdeal(size_t variableCount) {
  _data = new BigIdeal(VarNames(variableCount));
}

Frobby::ExternalIdeal::~ExternalIdeal() {
  BigIdeal* ideal = (BigIdeal*)_data;

  delete ideal;
}

void Frobby::ExternalIdeal::addGenerator(const mpz_ptr* exponentVector) {
  BigIdeal* ideal = (BigIdeal*)_data;

  ideal->newLastTerm();
  for (size_t var = 0; var < ideal->getVarCount(); ++var)
	mpz_set(ideal->getLastTermExponentRef(var).get_mpz_t(),
			exponentVector[var]);
}

void Frobby::ExternalIdeal::addGenerator(const mpz_t* exponentVector) {
  BigIdeal* ideal = (BigIdeal*)_data;

  ideal->newLastTerm();
  for (size_t var = 0; var < ideal->getVarCount(); ++var)
	mpz_set(ideal->getLastTermExponentRef(var).get_mpz_t(),
			exponentVector[var]);
}

void* Frobby::ExternalIdeal::getData() {
  return _data;
}

void Frobby::alexanderDual(ExternalIdeal* ideal, const mpz_ptr* exponentVector,
						   ExternalTermConsumer* consumer) {
  IrreducibleDecomParameters params;
  IrreducibleDecomFacade facade(false, params);

  BigIdeal* bigIdeal = (BigIdeal*)ideal->getData();
  vector<mpz_class> point(bigIdeal->getVarCount());
  for (size_t var = 0; var < bigIdeal->getVarCount(); ++var)
	mpz_set(point[var].get_mpz_t(), exponentVector[var]);

  facade.computeAlexanderDual(*bigIdeal, &point,
							  new ExternalConsumerAdapter
							  (consumer, bigIdeal->getVarCount()));
}
