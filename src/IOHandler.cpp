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
#include "IOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "TermTranslator.h"
#include "Ideal.h"
#include "Term.h"
#include "TermConsumer.h"
#include "VarNames.h"
#include "CoefTermConsumer.h"
#include "Polynomial.h"
#include "BigPolynomial.h"
#include "NameFactory.h"
#include "error.h"
#include "FrobbyStringStream.h"
#include "ElementDeleter.h"
#include "BigTermConsumer.h"
#include "BigTermRecorder.h"
#include "TranslatingTermConsumer.h"
#include "IrreducibleIdealSplitter.h"
#include "DataType.h"
#include "IdealConsolidator.h"
#include "CoefBigTermConsumer.h"
#include "TranslatingCoefTermConsumer.h"
#include "CountingIOHandler.h"

#include "NewMonosIOHandler.h"
#include "MonosIOHandler.h"
#include "Macaulay2IOHandler.h"
#include "Fourti2IOHandler.h"
#include "NullIOHandler.h"
#include "CoCoA4IOHandler.h"
#include "SingularIOHandler.h"

bool IOHandler::supportsInput(const DataType& type) const {
  return std::find(_supportedInputs.begin(), _supportedInputs.end(),
				   &type) != _supportedInputs.end();
}

bool IOHandler::supportsOutput(const DataType& type) const {
  return std::find(_supportedOutputs.begin(), _supportedOutputs.end(),
				   &type) != _supportedOutputs.end();
}

void IOHandler::registerInput(const DataType& type) {
  ASSERT(!type.isNull());
  ASSERT(!supportsInput(type));

  _supportedInputs.push_back(&type);
}

void IOHandler::registerOutput(const DataType& type) {
  ASSERT(!type.isNull());
  ASSERT(!supportsOutput(type));

  _supportedOutputs.push_back(&type);
}

class IdealWriter : public BigTermConsumer {
public:
  IdealWriter(IOHandler* handler, FILE* out):
	_handler(handler),
	_out(out),
	_firstIdeal(true),
	_firstGenerator(true),
	_names() {
	ASSERT(handler != 0);
	ASSERT(out != 0);
  }

  virtual void consumeRing(const VarNames& names) {
	if (_names != names) {
	  _names = names;
	  _firstIdeal = true;
	}
  }

  virtual void beginConsumingList() {
	_firstIdeal = true;
  }

  virtual void beginConsuming() {
	_handler->writeIdealHeader(_names, _firstIdeal, _out);
	_firstGenerator = true;
  }

  virtual void consume(const Term& term) {
	ASSERT(term.getVarCount() == _names.getVarCount());

	BigTermConsumer::consume(term);
	_firstGenerator = false;
  }

  virtual void consume(const Term& term, const TermTranslator& translator) {
	ASSERT(term.getVarCount() == _names.getVarCount());

	_handler->writeTermOfIdeal(term, &translator, _firstGenerator, _out);
	_firstGenerator = false;	
  }

  virtual void consume(const vector<mpz_class>& term) {
	ASSERT(term.size() == _names.getVarCount());

	_handler->writeTermOfIdeal
	  (term, _names, _firstGenerator, _out);
	_firstGenerator = false;
  }

  virtual void doneConsuming() {
	_handler->writeIdealFooter(_names, !_firstGenerator, _out);
	_firstIdeal = false;
  }

  virtual void doneConsumingList() {
	if (_firstIdeal)
	  _handler->writeRing(_names, _out);
  }

  virtual void consume(const BigIdeal& ideal) {
	consumeRing(ideal.getNames());

	// We do not just call beginConsuming() because we need to call
	// the overload of writeIdealHeader that specifies the number of
	// generators to follow.
	_handler->writeIdealHeader(_names, _firstIdeal,
							   ideal.getGeneratorCount(), _out);
	_firstGenerator = true;

	for (size_t term = 0; term < ideal.getGeneratorCount(); ++term)
	  consume(ideal.getTerm(term));
	doneConsuming();
  }

private:
  IOHandler* _handler;
  FILE* _out;
  bool _firstIdeal;
  bool _firstGenerator;
  VarNames _names;
};

void IOHandler::readTerm(Scanner& in,
						 const VarNames& names,
						 vector<mpz_class>& term) {
  // TODO: Consider eliminating the code duplication from the other
  // readTerm.

  term.resize(names.getVarCount());
  for (size_t var = 0; var < term.size(); ++var)
	term[var] = 0;

  if (in.match('1'))
    return;

  do {
    readVarPower(term, names, in);
  } while (in.match('*'));
}

IOHandler::~IOHandler() {
}

bool IOHandler::hasMoreInput(Scanner& in) const {
  return !in.matchEOF();
}

const char* IOHandler::getName() const {
  return _formatName;
}

const char* IOHandler::getDescription() const {
  return _formatDescription;
}

IOHandler::IOHandler(const char* formatName,
					 const char* formatDescription,
					 bool requiresSizeForIdealOutput):
  _formatName(formatName),
  _formatDescription(formatDescription),
  _requiresSizeForIdealOutput(requiresSizeForIdealOutput) {
  ASSERT(formatName != 0);
  ASSERT(formatDescription != 0);
}

auto_ptr<BigTermConsumer> IOHandler::createIdealWriter(FILE* out) {
  ASSERT(supportsOutput(DataType::getMonomialIdealType()));

  auto_ptr<BigTermConsumer> writer(new IdealWriter(this, out));
  return writer;
}

void IOHandler::writeCoefTermProduct(const mpz_class& coef,
									 const Term& term,
									 const TermTranslator* translator,
									 bool hidePlus,
									 FILE* out) {
  if (coef >= 0 && !hidePlus)
	fputc('+', out);

  if (term.isIdentity()) {
	gmp_fprintf(out, "%Zd", coef.get_mpz_t());
	return;
  }

  if (coef == -1)
	fputc('-', out);
  else if (coef != 1)
	gmp_fprintf(out, "%Zd*", coef.get_mpz_t());

  writeTermProduct(term, translator, out);
}

void IOHandler::writeCoefTermProduct(const mpz_class& coef,
									 const vector<mpz_class>& term,
									 const VarNames& names,
									 bool hidePlus,
									 FILE* out) {
  if (coef >= 0 && !hidePlus)
	fputc('+', out);

  bool isIdentity = true;
  for (size_t var = 0; var < term.size(); ++var)
	if (term[var] != 0)
	  isIdentity = false;

  if (isIdentity) {
	gmp_fprintf(out, "%Zd", coef.get_mpz_t());
	return;
  }

  if (coef == -1)
	fputc('-', out);
  else if (coef != 1)
	gmp_fprintf(out, "%Zd*", coef.get_mpz_t());

  writeTermProduct(term, names, out);
}

void IOHandler::writeTermProduct(const Term& term,
								 const TermTranslator* translator,
								 FILE* out) {
  bool seenNonZero = false;
  size_t varCount = term.getVarCount();
  for (size_t var = 0; var < varCount; ++var) {
	const char* exp = translator->getVarExponentString(var, term[var]);
	if (exp == 0)
	  continue;

	if (seenNonZero)
	  putc('*', out);
	else
	  seenNonZero = true;

	fputs(exp, out);
  }

  if (!seenNonZero)
	fputc('1', out);
}

void IOHandler::writeTermProduct(const vector<mpz_class>& term,
								 const VarNames& names,
								 FILE* out) {
  bool seenNonZero = false;
  size_t varCount = term.size();
  for (size_t var = 0; var < varCount; ++var) {
    if (term[var] == 0)
      continue;

	if (seenNonZero)
	  fputc('*', out);
	else
	  seenNonZero = true;

    fputs(names.getName(var).c_str(), out);
    if ((term[var]) != 1) {
	  fputc('^', out);
	  mpz_out_str(out, 10, term[var].get_mpz_t());
	}
  }

  if (!seenNonZero)
    fputc('1', out);
}

void IOHandler::readTerm(BigIdeal& ideal, Scanner& in) {
  ideal.newLastTerm();

  if (in.match('1'))
    return;

  do {
    readVarPower(ideal.getLastTermRef(), ideal.getNames(), in);
  } while (in.match('*'));
}

void IOHandler::readCoefTerm
(mpz_class& coef,
 vector<mpz_class>& term,
 const VarNames& names,
 bool firstTerm,
 Scanner& in) {
  term.resize(names.getVarCount());
  for (size_t var = 0; var < term.size(); ++var)
	term[var] = 0;

  bool positive = true;
  if (!firstTerm && in.match('+'))
	positive = !in.match('-');
  else if (in.match('-'))
	positive = false;
  else if (!firstTerm) {
	in.expect('+');
	return;
  }
  if (in.match('+') || in.match('-'))
	reportSyntaxError(in, "Too many adjacent signs.");

  if (in.peekIdentifier()) {
	coef = 1;
	readVarPower(term, names, in);
  } else
	in.readInteger(coef);

  while (in.match('*'))
	readVarPower(term, names, in);

  if (!positive)
	coef = -coef;
}

void IOHandler::readCoefTerm(BigPolynomial& polynomial,
							 bool firstTerm,
							 Scanner& in) {
  polynomial.newLastTerm();
  mpz_class& coef = polynomial.getLastCoef();
  vector<mpz_class>& term = polynomial.getLastTerm();

  readCoefTerm(coef, term, polynomial.getNames(), firstTerm, in);
}

void IOHandler::readVarPower(vector<mpz_class>& term,
							 const VarNames& names, Scanner& in) {
  size_t var = in.readVariable(names);

  if (term[var] != 0) {
	const string& name = names.getName(var);
	reportSyntaxError(in, "The variable " + 
					  name + " appears more than once in monomial.");
  }

  if (in.match('^')) {
    in.readInteger(term[var]);
    if (term[var] <= 0) {
	  FrobbyStringStream errorMsg;
	  errorMsg << "Expected positive integer as exponent but got "
			   << term[var] << ".";
	  reportSyntaxError(in, errorMsg);
    }
  } else
    term[var] = 1;
}

namespace {
  typedef NameFactory<IOHandler> IOHandlerFactory;

  IOHandlerFactory getIOHandlerFactory() {
	IOHandlerFactory factory;

	nameFactoryRegister<Macaulay2IOHandler>(factory);
	nameFactoryRegister<CoCoA4IOHandler>(factory);
	nameFactoryRegister<SingularIOHandler>(factory);
	nameFactoryRegister<MonosIOHandler>(factory);
	nameFactoryRegister<NewMonosIOHandler>(factory);
	nameFactoryRegister<Fourti2IOHandler>(factory);
	nameFactoryRegister<NullIOHandler>(factory);
	nameFactoryRegister<CountingIOHandler>(factory);

	return factory;
  }
}

auto_ptr<IOHandler> IOHandler::createIOHandler(const string& name) {
  return getIOHandlerFactory().create(name);
}

void IOHandler::addFormatNames(vector<string>& names) {
  getIOHandlerFactory().addNamesWithPrefix("", names);
}

void IOHandler::writePolynomialHeader(const VarNames& names, FILE* out) {
  ASSERT(false);
  reportInternalError("Called IOHandler::writePolynomialHeader.");
}

void IOHandler::writePolynomialHeader(const VarNames& names,
									  size_t termCount,
									  FILE* out) {
  writePolynomialHeader(names, out);
}

void IOHandler::writeTermOfPolynomial(const mpz_class& coef,
									  const Term& term,
									  const TermTranslator* translator,
									  bool isFirst,
									  FILE* out) {
  ASSERT(false);
  reportInternalError("Called IOHandler::writeTermOfPolynomial (small).");
}

void IOHandler::writeTermOfPolynomial(const mpz_class& coef,
									  const vector<mpz_class>& term,
									  const VarNames& names,
									  bool isFirst,
									  FILE* out) {
  ASSERT(false);
  reportInternalError("Called IOHandler::writeTermOfPolynomial (big).");
}

void IOHandler::writePolynomialFooter(const VarNames& names,
									  bool wroteAnyGenerators,
									  FILE* out) {
  ASSERT(false);
  reportInternalError("Called IOHandler::writePolynomialFooter.\n");
}

void IOHandler::writeIdealHeader(const VarNames& names,
								 bool defineNewRing,
								 size_t generatorCount,
								 FILE* out) {
  writeIdealHeader(names, defineNewRing, out);
}

class PolynomialWriter : public CoefBigTermConsumer {
public:
  PolynomialWriter(IOHandler* handler, FILE* out):
	_handler(handler),
	_out(out),
	_firstTerm(true) {
	ASSERT(handler != 0);
  }

  virtual void consumeRing(const VarNames& names) {
	_names = names;
  }

  virtual void beginConsuming() {
	_handler->writePolynomialHeader(_names, _out);
	_firstTerm = true;
  }

  virtual void consume(const mpz_class& coef, const vector<mpz_class>& term) {
	_handler->writeTermOfPolynomial(coef, term, _names, _firstTerm, _out);
	_firstTerm = false;	
  }

  virtual void consume
  (const mpz_class& coef, const Term& term, const TermTranslator& translator) {
	_handler->writeTermOfPolynomial(coef, term, &translator, _firstTerm, _out);
	_firstTerm = false;
  }	

  virtual void doneConsuming() {
	_handler->writePolynomialFooter(_names, !_firstTerm, _out);
  }

  virtual void consume(const BigPolynomial& poly) {
	consumeRing(poly.getNames());

	// We do this instead of calling beginConsuming directly so that
	// we can specify the number of terms.
	_handler->writePolynomialHeader(_names, poly.getTermCount(), _out);

	for (size_t index = 0; index < poly.getTermCount(); ++index)
	  consume(poly.getCoef(index), poly.getTerm(index));
	doneConsuming();
  }

private:
  IOHandler* _handler;
  FILE* _out;
  bool _firstTerm;
  VarNames _names;
};

auto_ptr<CoefBigTermConsumer> IOHandler::createPolynomialWriter(FILE* out) {
  ASSERT(supportsOutput(DataType::getPolynomialType()));
  return auto_ptr<CoefBigTermConsumer>(new PolynomialWriter(this, out));
}

void readFrobeniusInstance(Scanner& in, vector<mpz_class>& numbers) {
  numbers.clear();

  string number;
  mpz_class n;
  while (!in.matchEOF()) {
	in.readInteger(n);

    if (n <= 1) {
	  FrobbyStringStream errorMsg;
	  errorMsg << "Read the number " << n
			   << " while reading Frobenius instance. "
			   << "Only integers strictly larger than 1 are valid.";
	  reportSyntaxError(in, errorMsg);
    }

    numbers.push_back(n);
  }

  if (numbers.empty())
	reportSyntaxError
	  (in, "Read empty Frobenius instance, which is not allowed.");

  mpz_class gcd = numbers[0];
  for (size_t i = 1; i < numbers.size(); ++i)
    mpz_gcd(gcd.get_mpz_t(), gcd.get_mpz_t(), numbers[i].get_mpz_t());

  if (gcd != 1) {
	// Maybe not strictly speaking a syntax error, but that category
	// of errors still fits best.
	FrobbyStringStream errorMsg;
	errorMsg << "The numbers in the Frobenius instance are not "
			 << "relatively prime. They are all divisible by "
			 << gcd << '.';
	reportSyntaxError(in, errorMsg);
  }
}
