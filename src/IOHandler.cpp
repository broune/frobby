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

#include "NewMonosIOHandler.h"
#include "MonosIOHandler.h"
#include "Macaulay2IOHandler.h"
#include "Fourti2IOHandler.h"
#include "NullIOHandler.h"
#include "CoCoA4IOHandler.h"
#include "SingularIOHandler.h"

bool IOHandler::supportsInput(DataType type) const {
  return std::find(_supportedInputs.begin(), _supportedInputs.end(),
				   type) != _supportedInputs.end();
}

bool IOHandler::supportsOutput(DataType type) const {
  return std::find(_supportedOutputs.begin(), _supportedOutputs.end(),
				   type) != _supportedOutputs.end();
}

const char* IOHandler::getDataTypeName(DataType type) {
  switch (type) {
  case None:
	return "nothing";

  case MonomialIdeal:
	return "a monomial ideal";

  case Polynomial:
	return "a polynomial";

  case MonomialIdealList:
	return "a list of monomial ideals";

  default:
	ASSERT(false);
	reportInternalError("Unknown DataType enum in getDataTypeName.");
	return 0; // To ensure that a static analysis does not report an issue.
  }
}

void IOHandler::addDataTypes(vector<DataType>& types) {
  types.push_back(MonomialIdeal);
  types.push_back(MonomialIdealList);
  types.push_back(Polynomial);
}

void IOHandler::registerInput(DataType type) {
  ASSERT(type != None);
  ASSERT(!supportsInput(type));
  _supportedInputs.push_back(type);
}

void IOHandler::registerOutput(DataType type) {
  ASSERT(type != None);
  ASSERT(!supportsOutput(type));
  _supportedOutputs.push_back(type);
}

class IdealWriter : public TermConsumer {
public:
  IdealWriter(IOHandler* handler, const TermTranslator* translator, FILE* out):
	_handler(handler),
	_translator(translator),
	_out(out),
	_first(true) {
	ASSERT(handler != 0);
	ASSERT(translator != 0);
	ASSERT(out != 0);
  }

  IdealWriter(IOHandler* handler,
			  const TermTranslator* translator,
			  size_t generatorCount,
			  FILE* out):
	_handler(handler),
	_translator(translator),
	_out(out),
	_first(true) {
	ASSERT(handler != 0);
	ASSERT(translator != 0);
	ASSERT(out != 0);
  }

  virtual void beginConsuming() {
	_handler->writeIdealHeader(_translator->getNames(), _out);
	_first = true;
  }

  virtual void consume(const Term& term) {
	ASSERT(term.getVarCount() == _translator->getVarCount());

	_handler->writeTermOfIdeal(term, _translator, _first, _out);
	_first = false;
  }

  virtual void doneConsuming() {
	_handler->writeIdealFooter(_translator->getNames(), !_first, _out);
  }

private:
  IOHandler* _handler;
  const TermTranslator* _translator;
  FILE* _out;
  bool _first;
};

class IrreducibleIdealWriter : public TermConsumer {
public:
  IrreducibleIdealWriter(IOHandler* handler,
						 const TermTranslator* translator,
						 FILE* out):
	_varCount(translator->getVarCount()),
	_handler(handler),
	_translator(translator),
	_out(out),
	_tmp(translator->getVarCount()) {
	ASSERT(handler != 0);
	ASSERT(translator != 0);
	ASSERT(out != 0);
	ASSERT(_tmp.isIdentity());
  }

  virtual void beginConsuming() {
  }

  virtual void consume(const Term& term) {
	ASSERT(term.getVarCount() == _varCount);
	ASSERT(_tmp.isIdentity());

	size_t support = 0;
	for (size_t var = 0; var < _varCount; ++var)
	  if (_translator->getExponent(var, term) != 0)
		++support;

	_handler->writeIdealHeader(_translator->getNames(), support, _out);
	bool first = true;
	for (size_t var = 0; var < _varCount; ++var) {
	  if (_translator->getExponent(var, term) != 0) {
		_tmp[var] = term[var];
		_handler->writeTermOfIdeal(_tmp, _translator, first, _out);
		first = false;
		_tmp[var] = 0;
	  }
	}
	_handler->writeIdealFooter(_translator->getNames(), true, _out);

	ASSERT(_tmp.isIdentity());
  }

  virtual void doneConsuming() {
  }

private:
  size_t _varCount;
  IOHandler* _handler;
  const TermTranslator* _translator;
  FILE* _out;
  Term _tmp;
};

class DelayedIdealWriter : public TermConsumer {
 public:
  DelayedIdealWriter(IOHandler* handler,
					 const TermTranslator* translator,
					 FILE* out):
	_handler(handler),
	_translator(translator),
	_out(out),
	_ideal(translator->getVarCount()) {
	ASSERT(handler != 0);
	ASSERT(translator != 0);
	ASSERT(out != 0);
  }

  virtual void beginConsuming() {
  }

  virtual void consume(const Term& term) {
	ASSERT(term.getVarCount() == _ideal.getVarCount());

	_ideal.insert(term);
  }

  virtual void doneConsuming() {
	IdealWriter writer(_handler, _translator,
					   _ideal.getGeneratorCount(), _out);

	Term tmp(_ideal.getVarCount());
	Ideal::const_iterator stop = _ideal.end();
	for (Ideal::const_iterator it = _ideal.begin(); it != stop; ++it) {
	  tmp = *it;
	  writer.consume(tmp);
	}
  }

private:
  IOHandler* _handler;
  const TermTranslator* _translator;
  FILE* _out;
  Ideal _ideal;
};

void IOHandler::readIdeals(Scanner& in, vector<BigIdeal*> ideals) {
  ASSERT(supportsInput(MonomialIdealList));

  while (hasMoreInput(in)) {
	BigIdeal* ideal = new BigIdeal();
	readIdeal(in, *ideal);
	ideals.push_back(ideal);
  }
}

void IOHandler::readTerm(Scanner& in,
						 const VarNames& names,
						 vector<mpz_class>& term) {
  BigIdeal tmp(names);
  readTerm(tmp, in);
  term = tmp.getTerm(0);
}

void IOHandler::readPolynomial(Scanner& in, BigPolynomial& polynomial) {
  ASSERT(false);
  reportInternalError("Called IOHandler::readPolynomial.");
}

IOHandler::~IOHandler() {
}

void IOHandler::writeIdeal(const BigIdeal& ideal, FILE* out) {
  size_t generatorCount = ideal.getGeneratorCount();
  writeIdealHeader(ideal.getNames(), generatorCount, out);
  for (size_t i = 0; i < generatorCount; ++i)
	writeTermOfIdeal(ideal[i], ideal.getNames(), i == 0, out);
  writeIdealFooter(ideal.getNames(), generatorCount > 0, out);
}

void IOHandler::writePolynomial(const BigPolynomial& polynomial, FILE* out) {
  size_t termCount = polynomial.getTermCount();
  writePolynomialHeader(polynomial.getNames(), termCount, out);
  for (size_t i = 0; i < termCount; ++i) {
	writeTermOfPolynomial(polynomial.getCoef(i), polynomial.getTerm(i),
						  polynomial.getNames(), i == 0, out);
  }
  writePolynomialFooter(polynomial.getNames(), termCount > 0, out);
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

TermConsumer* IOHandler::createIdealWriter(const TermTranslator* translator,
										   FILE* out) {
  ASSERT(supportsOutput(MonomialIdeal));

  if (_requiresSizeForIdealOutput) {
	FrobbyStringStream msg;
	msg << "Using the format " << getName() <<
	  " makes it necessary to store all of the output in "
	  "memory before writing it out. This increases "
	  "memory consumption and decreases performance.";
	displayNote(msg);

	return new DelayedIdealWriter(this, translator, out);
  }
  else
	return new IdealWriter(this, translator, out);
}

TermConsumer* IOHandler::createIrreducibleIdealWriter
(const TermTranslator* translator,
 FILE* out) {
  return new IrreducibleIdealWriter(this, translator, out);
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

void IOHandler::readCoefTerm(BigPolynomial& polynomial,
							 bool firstTerm,
							 Scanner& in) {
  polynomial.newLastTerm();
  mpz_class& coef = polynomial.getLastCoef();
  vector<mpz_class>& term = polynomial.getLastTerm();

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
	readVarPower(term, polynomial.getNames(), in);
  } else
	in.readInteger(coef);

  while (in.match('*'))
	readVarPower(term, polynomial.getNames(), in);

  if (!positive)
	coef = -coef;
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
								 size_t generatorCount,
								 FILE* out) {
  writeIdealHeader(names, out);
}

class PolynomialWriter : public CoefTermConsumer {
public:
  PolynomialWriter(IOHandler* handler,
				   const TermTranslator* translator,
				   FILE* out):
	_handler(handler),
	_translator(translator),
	_out(out),
	_first(true) {
	ASSERT(handler != 0);
	ASSERT(translator != 0);
	ASSERT(out != 0);
  }

  PolynomialWriter(IOHandler* handler,
				   const TermTranslator* translator,
				   size_t termCount,
				   FILE* out):
	_handler(handler),
	_translator(translator),
	_out(out),
	_first(true),
	_termCount(new size_t(termCount)) {
	ASSERT(handler != 0);
	ASSERT(translator != 0);
	ASSERT(out != 0);
  }

  virtual void beginConsuming() {
	if (_termCount.get() == 0)
	  _handler->writePolynomialHeader(_translator->getNames(), _out);
	else
	  _handler->writePolynomialHeader(_translator->getNames(),
									  *_termCount, _out);
  }

  virtual void consume(const mpz_class& coef, const Term& term) {
	ASSERT(coef != 0);
	_handler->writeTermOfPolynomial(coef, term, _translator, _first, _out);
	_first = false;
  }

  virtual void doneConsuming() {
	_handler->writePolynomialFooter(_translator->getNames(), !_first, _out);
  }

private:
  IOHandler* _handler;
  const TermTranslator* _translator;
  FILE* _out;
  bool _first;
  auto_ptr<size_t> _termCount;
};

class DelayedPolynomialWriter : public CoefTermConsumer {
 public:
  DelayedPolynomialWriter(IOHandler* handler,
						  const TermTranslator* translator,
						  FILE* out):
	_handler(handler),
	_translator(translator),
	_out(out),
	_polynomial(translator->getVarCount()) {
	ASSERT(handler != 0);
	ASSERT(translator != 0);
	ASSERT(out != 0);
  }

  virtual void beginConsuming() {
  }

  virtual void consume(const mpz_class& coef, const Term& term) {
	ASSERT(term.getVarCount() == _polynomial.getVarCount());

	_polynomial.add(coef, term);
  }

  virtual void doneConsuming() {
	size_t termCount = _polynomial.getTermCount();
	PolynomialWriter writer(_handler, _translator, termCount,_out);
	writer.beginConsuming();
	for (size_t term = 0; term < termCount; ++term)
	  writer.consume(_polynomial.getCoef(term), _polynomial.getTerm(term));
	writer.doneConsuming();
	_polynomial.clear();
  }

private:
  IOHandler* _handler;
  const TermTranslator* _translator;
  FILE* _out;
  Polynomial _polynomial;
};

CoefTermConsumer* IOHandler::createPolynomialWriter
(const TermTranslator* translator, FILE* out) {
  ASSERT(supportsOutput(Polynomial));

  if (_requiresSizeForIdealOutput) {
	FrobbyStringStream msg;
	msg << "Using the format " << getName() <<
	  " makes it necessary to store all of the output in "
	  "memory before writing it out. This increases "
	  "memory consumption and decreases performance.";
	displayNote(msg);

	return new DelayedPolynomialWriter(this, translator, out);
  }
  else
	return new PolynomialWriter(this, translator, out);
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
