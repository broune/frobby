#include "stdinc.h"
#include "frobby.h"

#include "BigIdeal.h"
#include "IrreducibleDecomFacade.h"
#include "IrreducibleDecomParameters.h"

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

  facade.computeAlexanderDual(*bigIdeal, point, consumer);
}
