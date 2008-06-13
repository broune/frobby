/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "frobby.h"

#include "BigIdeal.h"
#include "SliceFacade.h"
#include "SliceParameters.h"
#include "BigTermConsumer.h"
#include "TermTranslator.h"
#include "Term.h"
#include "CoefBigTermConsumer.h"

class ExternalConsumerAdapter :
  public BigTermConsumer, public CoefBigTermConsumer{
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

  // BigTermConsumer methods.
  virtual void consume(const Term& term, TermTranslator* translator) {
	ASSERT(term.getVarCount() == _varCount);
	setTerm(term, translator);
	_consumer->consume(_term);
  }

  virtual void consume(mpz_ptr* term) {
	_consumer->consume(term);
  }

  // CoefBigTermConsumer methods.
  virtual void consume(const mpz_class& coef,
					   const Term& term,
					   TermTranslator* translator) {
	setTerm(term, translator);
	_consumer->consume(coef.get_mpz_t(), _term);
  }

  virtual void consume(const mpz_class& coef, mpz_ptr* term) {
	_consumer->consume(coef.get_mpz_t(), term);
  }

  virtual void consume(const mpz_class& coef,
					   const vector<mpz_class>& term) {
	for (size_t var = 0; var < _varCount; ++var)
	  _term[var] = const_cast<mpz_ptr>(term[var].get_mpz_t());
	_consumer->consume(coef.get_mpz_t(), _term);
  }

private:
  void setTerm(const Term& term, TermTranslator* translator) {
	ASSERT(term.getVarCount() == _varCount);
	ASSERT(translator->getVarCount() == _varCount);

	for (size_t var = 0; var < _varCount; ++var)
	  _term[var] = const_cast<mpz_ptr>
		(translator->getExponent(var, term).get_mpz_t());
  }

  Frobby::TermConsumer* _consumer;
  size_t _varCount;
  mpz_ptr* _term;
};

Frobby::TermConsumer::~TermConsumer() {
}

void Frobby::TermConsumer::consume(mpz_ptr* exponentVector) {
}

void Frobby::TermConsumer::consume(const mpz_t coefficient,
								   mpz_ptr* exponentVector) {
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
  // terms to the consumer.
  exponentVector = 0;

  BigTermConsumer* adaptedConsumer = 
    new ExternalConsumerAdapter(&consumer, bigIdeal.getVarCount());
  SliceFacade facade(bigIdeal, adaptedConsumer, false);
  SliceParameters params;
  params.apply(facade);

  facade.computeAlexanderDual(point);
}

void Frobby::multigradedHilbertPoincareSeries(const Ideal& ideal,
											  TermConsumer& consumer) {
  const BigIdeal& bigIdeal = FrobbyImpl::FrobbyIdealHelper::getIdeal(ideal);

  CoefBigTermConsumer* adaptedConsumer = 
    new ExternalConsumerAdapter(&consumer, bigIdeal.getVarCount());
  SliceFacade facade(bigIdeal, adaptedConsumer, false);
  SliceParameters params;
  params.apply(facade);

  facade.computeMultigradedHilbertSeries();
}

void Frobby::univariateHilbertPoincareSeries(const Ideal& ideal,
											 TermConsumer& consumer) {
  const BigIdeal& bigIdeal = FrobbyImpl::FrobbyIdealHelper::getIdeal(ideal);

  CoefBigTermConsumer* adaptedConsumer =
	new ExternalConsumerAdapter(&consumer, 1);
  SliceFacade facade(bigIdeal, adaptedConsumer, false);
  SliceParameters params;
  params.apply(facade);

  facade.computeUnivariateHilbertSeries();
}
