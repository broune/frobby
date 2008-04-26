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
  ExternalConsumerAdapter(Frobby::TermConsumer* consumer,
			  size_t varCount):
	_consumer(consumer),
	_varCount(varCount),
	_term(new mpz_ptr[varCount]) {
	ASSERT(consumer != 0);
  }

  virtual ~ExternalConsumerAdapter() {
    delete[] _term;
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
  Frobby::TermConsumer* _consumer;
  size_t _varCount;
  mpz_ptr* _term;
};

Frobby::TermConsumer::~TermConsumer() {
}

namespace FrobbyImpl {
  using ::BigIdeal;

  class FrobbyIdealHelper {
  public:
    FrobbyIdealHelper(size_t variableCount):
      _ideal(VarNames(variableCount)),
      _atVariable(variableCount) {
    }

    static const BigIdeal& getIdeal(const Frobby::Ideal& ideal) {
      return ideal._data->_ideal;
    }

  private:
    friend class Frobby::Ideal;

    BigIdeal _ideal;
    size_t _atVariable;
  };
}

Frobby::Ideal::Ideal(size_t variableCount) {
  _data = new FrobbyImpl::FrobbyIdealHelper(variableCount);
}

Frobby::Ideal::Ideal(const Ideal& ideal) {
  _data = new FrobbyImpl::FrobbyIdealHelper(*ideal._data);
}

Frobby::Ideal::~Ideal() {
  delete _data;
}

Frobby::Ideal& Frobby::Ideal::operator=(const Ideal& ideal) {
  delete _data;
  _data = new FrobbyImpl::FrobbyIdealHelper(*ideal._data);
  return *this;
}

void Frobby::Ideal::addExponent(const mpz_t exponent) {
  ASSERT(_data->_atVariable <= _data->_ideal.getVarCount());

  if (_data->_atVariable == _data->_ideal.getVarCount()) {
    _data->_ideal.newLastTerm();
    _data->_atVariable = 0;
    if (_data->_ideal.getVarCount() == 0)
      return;
  }

  mpz_class& ref = _data->_ideal.getLastTermExponentRef(_data->_atVariable);
  mpz_set(ref.get_mpz_t(), exponent);
  ++_data->_atVariable;
}

void Frobby::Ideal::addExponent(int exponent) {
  mpz_class tmp(exponent);
  addExponent(tmp.get_mpz_t());
}

void Frobby::Ideal::addExponent(unsigned int exponent) {
  mpz_class tmp(exponent);
  addExponent(tmp.get_mpz_t());
}

void Frobby::alexanderDual(const Ideal& ideal,
			   const mpz_t* exponentVector,
			   TermConsumer& consumer) {
  const BigIdeal& bigIdeal = FrobbyImpl::FrobbyIdealHelper::getIdeal(ideal);

  vector<mpz_class> point;
  if (exponentVector != 0) {
    point.resize(bigIdeal.getVarCount());
    for (size_t var = 0; var < bigIdeal.getVarCount(); ++var)
      mpz_set(point[var].get_mpz_t(), exponentVector[var]);
  } else
    bigIdeal.getLcm(point);

  // We guarantee not to retain a reference to exponentVector when providing
  // terms to the consumer, so we must set exponentVector
  // to null. This is visible behavior if the caller is using something like
  // the Boehm garbage collector.
  exponentVector = 0;

  // We copy the ideal because computeAlexanderDual clears the ideal,
  // which we guarantee not to do. TODO: make computeAlexanderDual not
  // do this and remove this copy.
  BigIdeal bigIdealCopy(bigIdeal);

  IrreducibleDecomParameters params;
  IrreducibleDecomFacade facade(false, params);
  BigTermConsumer* adaptedConsumer = 
    new ExternalConsumerAdapter(&consumer, bigIdealCopy.getVarCount());
  facade.computeAlexanderDual(bigIdealCopy, &point, adaptedConsumer);
}
