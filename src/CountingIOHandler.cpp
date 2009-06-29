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
#include "CountingIOHandler.h"

#include "BigTermConsumer.h"
#include "CoefBigTermConsumer.h"
#include "DataType.h"
#include "BigIdeal.h"
#include "BigPolynomial.h"

namespace {
  /** This class is a helper class for CountingIOHandler. It
   the number of terms that it consumes.
  */
  class CountingConsumer :
	public BigTermConsumer, public CoefBigTermConsumer {
  public:
	CountingConsumer(FILE* out):
	  _termCount(0),
	  _out(out) {
	}

	/** This destructor writes the final number out.

	 @todo This should not be happening in a destructor. Make a
	  doneAllConsuming or something like that to do the output.
	*/
	virtual ~CountingConsumer() {
	  // Destructors must not throw exceptions. As far as I know,
	  // bad_alloc is the only thing that should conceivably be thrown
	  // from gmp_fprintf.
	  try {
		gmp_fprintf(_out, "%Zd\n", _termCount.get_mpz_t());
	  } catch (std::bad_alloc) {
		// Not good to ignore this, but what can we do...
	  } catch (...) {
		ASSERT(false);
		throw;
	  }
	}

	virtual void consumeRing(const VarNames& names) {
	}

	virtual void beginConsuming() {
	}

	virtual void consume(const Term& term, const TermTranslator& translator) {
	  ++_termCount;
	}

	virtual void consume(const vector<mpz_class>& term) {
	  ++_termCount;
	}

	virtual void consume(const BigIdeal& ideal) {
	  beginConsuming();
	  _termCount += ideal.getGeneratorCount();
	  doneConsuming();
	}

	virtual void consume(const mpz_class& coef, const Term& term) {
	  ++_termCount;
	}

	virtual void consume
	(const mpz_class& coef,
	 const Term& term,
	 const TermTranslator& translator) {
	  ++_termCount;
	}

	virtual void consume
	(const mpz_class& coef, const vector<mpz_class>& term) {
	  ++_termCount;
	}

	virtual void consume(const BigPolynomial& poly) {
	  beginConsuming();
	  _termCount += poly.getTermCount();
	  doneConsuming();
	}

	virtual void doneConsuming() {
	}

  private:
	mpz_class _termCount;
	FILE* _out;
  };
}


CountingIOHandler::CountingIOHandler():
  IOHandler(staticGetName(), "Writes the number of output terms.", false) {

  registerOutput(DataType::getMonomialIdealType());
  registerOutput(DataType::getPolynomialType());
  registerOutput(DataType::getMonomialIdealListType());
}

const char* CountingIOHandler::staticGetName() {
  return "count";
}

void CountingIOHandler::readIdeal(Scanner& in, BigTermConsumer& consumer) {
}

void CountingIOHandler::readIdeals(Scanner& in, BigTermConsumer& consumer) {
}

void CountingIOHandler::writeRing(const VarNames& names, FILE* out) {
}

void CountingIOHandler::writeTerm(const vector<mpz_class>& term,
								  const VarNames& names,
								  FILE* out) {
}

auto_ptr<BigTermConsumer> CountingIOHandler::createIdealWriter(FILE* out) {
  return auto_ptr<BigTermConsumer>(new CountingConsumer(out));
}

void CountingIOHandler::writePolynomialHeader(const VarNames& names, FILE* out) {
}

void CountingIOHandler::writeTermOfPolynomial(const mpz_class& coef,
										  const Term& term,
										  const TermTranslator* translator,
										  bool isFirst,
										  FILE* out) {
}

void CountingIOHandler::writeTermOfPolynomial(const mpz_class& coef,
										  const vector<mpz_class>& term,
										  const VarNames& names,
										  bool isFirst,
										  FILE* out) {
}

void CountingIOHandler::writePolynomialFooter(const VarNames& names,
										  bool wroteAnyGenerators,
										  FILE* out) {
}

void CountingIOHandler::writeIdealHeader(const VarNames& names,
									 bool defineNewRing,
									 FILE* out) {
}

void CountingIOHandler::writeTermOfIdeal(const Term& term,
									 const TermTranslator* translator,
									 bool isFirst,
									 FILE* out) {
}

void CountingIOHandler::writeTermOfIdeal(const vector<mpz_class>& term,
									 const VarNames& names,
									 bool isFirst,
									 FILE* out) {
}

void CountingIOHandler::writeIdealFooter(const VarNames& names,
									 bool wroteAnyGenerators,
									 FILE* out) {
}

auto_ptr<CoefBigTermConsumer> CountingIOHandler::createPolynomialWriter
(FILE* out) {
  return auto_ptr<CoefBigTermConsumer>(new CountingConsumer(out));
}

void CountingIOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
}

bool CountingIOHandler::hasMoreInput(Scanner& scanner) const {
  return false;
}

void CountingIOHandler::readPolynomial
(Scanner& in, CoefBigTermConsumer& consumer) {
}
