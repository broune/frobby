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
	fputs("INTERNAL ERROR: Unknown DataType enum in getDataTypeName.\n",
		  stderr);
	ASSERT(false);
	exit(1);
	return 0;
  }
}

const vector<IOHandler::DataType>& IOHandler::getDataTypes() {
  static vector<DataType> types;
  if (types.empty()) {
	types.push_back(MonomialIdeal);
	types.push_back(MonomialIdealList);
	types.push_back(Polynomial);
  }
  return types;
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

	_handler->writeIdealHeader(_translator->getNames(), _out);
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

	_handler->writeIdealHeader(_translator->getNames(), generatorCount, _out);
  }

  virtual ~IdealWriter() {
	_handler->writeIdealFooter(_translator->getNames(), !_first, _out);
  }

  virtual void consume(const Term& term) {
	ASSERT(term.getVarCount() == _translator->getVarCount());

	_handler->writeTermOfIdeal(term, _translator, _first, _out);
	_first = false;
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

  virtual void consume(const Term& term) {
	ASSERT(term.getVarCount() == _varCount);
	ASSERT(_tmp.isIdentity());

	size_t support = 0;
	for (size_t var = 0; var < _varCount; ++var)
	  if (_translator->getExponent(var, term) != 0)
		++support;

	if (support == 0) {
	  _handler->writeIdealHeader(_translator->getNames(), 1, _out);
	  _handler->writeTermOfIdeal(_tmp, _translator, true, _out);
	  _handler->writeIdealFooter(_translator->getNames(), true, _out);
	} else {
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
	}
	ASSERT(_tmp.isIdentity());
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

  virtual ~DelayedIdealWriter() {
	IdealWriter writer(_handler, _translator,
					   _ideal.getGeneratorCount(), _out);

	Term tmp(_ideal.getVarCount());
	Ideal::const_iterator stop = _ideal.end();
	for (Ideal::const_iterator it = _ideal.begin(); it != stop; ++it) {
	  tmp = *it;
	  writer.consume(tmp);
	}
  }

  virtual void consume(const Term& term) {
	ASSERT(term.getVarCount() == _ideal.getVarCount());

	_ideal.insert(term);
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
  fputs("INTERNAL ERROR: Called IOHandler::readPolynomial.\n", stderr);
  ASSERT(false);
  exit(1); 
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
	fprintf(stderr,
			"NOTE: Using the format %s makes it necessary to store all of\n"
			"the output in memory before writing it out. This increases\n"
			"memory consumption and decreases performance.\n",
			getName());
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
  // TODO: do something to avoid constructing these each time.
  mpz_class coef;
  vector<mpz_class> term;
  term.resize(polynomial.getNames().getVarCount());

  bool positive = true;
  if (in.match('+'))
	positive = !in.match('-');
  else if (in.match('-'))
	positive = false;
  else if (!firstTerm) {
	in.expect('+');
	return;
  }
  if (in.match('+') || in.match('-')) {
	fputs("ERROR: Too many signs.\n", stderr);
	exit(1);
  }
	  
  if (in.peekIdentifier()) {
	coef = 1;
	readVarPower(term, polynomial.getNames(), in);
  } else
	in.readInteger(coef);

  while (in.match('*'))
	readVarPower(term, polynomial.getNames(), in);

  if (!positive)
	coef = -coef;

  polynomial.add(coef, term);
}

void IOHandler::readVarPower(vector<mpz_class>& term,
							 const VarNames& names, Scanner& in) {
  size_t var = in.readVariable(names);

  if (term[var] != 0) {
	in.printError();
	fputs("A variable appears twice.\n", stderr);
	exit(1);
  }

  if (in.match('^')) {
    in.readInteger(term[var]);
    if (term[var] <= 0) {
	  in.printError();
      gmp_fprintf
		(stderr, "Expected positive integer as exponent but got %Zd.\n",
		 term[var].get_mpz_t());
      exit(1);
    }
  } else
    term[var] = 1;
}

IOHandler* IOHandler::getIOHandler(const string& name) {
  const vector<IOHandler*>& handlers = getIOHandlers();
  for (vector<IOHandler*>::const_iterator it = handlers.begin();
	   it != handlers.end(); ++it) {
	if (name == (*it)->getName())
	  return *it;
  }
  return 0;
}

const vector<IOHandler*>& IOHandler::getIOHandlers() {
  static vector<IOHandler*> handlers;
  if (handlers.empty()) {
	static Macaulay2IOHandler m2;
	handlers.push_back(&m2);

	static CoCoA4IOHandler cocoa4;
	handlers.push_back(&cocoa4);

	static SingularIOHandler singular;
	handlers.push_back(&singular);

	static MonosIOHandler monos;
	handlers.push_back(&monos);

	static NewMonosIOHandler newMonos;
	handlers.push_back(&newMonos);

	static Fourti2IOHandler fourti2;
	handlers.push_back(&fourti2);

	static NullIOHandler nullHandler;
	handlers.push_back(&nullHandler);
  }
  return handlers;
}

void IOHandler::writePolynomialHeader(const VarNames& names, FILE* out) {
  fputs("INTERNAL ERROR: Called IOHandler::writePolynomialHeader.\n", stderr);
  ASSERT(false);
  exit(1);
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
  fputs("INTERNAL ERROR: Called IOHandler::writeTermOfPolynomial (small).\n",
		stderr);
  ASSERT(false);
  exit(1);
}

void IOHandler::writeTermOfPolynomial(const mpz_class& coef,
									  const vector<mpz_class>& term,
									  const VarNames& names,
									  bool isFirst,
									  FILE* out) {
  fputs("INTERNAL ERROR: Called IOHandler::writeTermOfPolynomial (big).\n",
		stderr);
  ASSERT(false);
  exit(1);
}

void IOHandler::writePolynomialFooter(const VarNames& names,
									  bool wroteAnyGenerators,
									  FILE* out) {
  fputs("INTERNAL ERROR: Called IOHandler::writePolynomialFooter.\n", stderr);
  ASSERT(false);
  exit(1);
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

	_handler->writePolynomialHeader(_translator->getNames(), _out);
  }

  PolynomialWriter(IOHandler* handler,
				   const TermTranslator* translator,
				   size_t termCount,
				   FILE* out):
	_handler(handler),
	_translator(translator),
	_out(out),
	_first(true) {
	ASSERT(handler != 0);
	ASSERT(translator != 0);
	ASSERT(out != 0);

	_handler->writePolynomialHeader(_translator->getNames(), termCount, _out);
  }

  virtual ~PolynomialWriter() {
	_handler->writePolynomialFooter(_translator->getNames(), !_first, _out);
  }

  virtual void consume(const mpz_class& coef, const Term& term) {
	ASSERT(coef != 0);
	_handler->writeTermOfPolynomial(coef, term, _translator, _first, _out);
	_first = false;
  }

private:
  IOHandler* _handler;
  const TermTranslator* _translator;
  FILE* _out;
  bool _first;
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

  virtual ~DelayedPolynomialWriter() {
	size_t termCount = _polynomial.getTermCount();
	PolynomialWriter writer(_handler, _translator, termCount,_out);
	for (size_t term = 0; term < termCount; ++term)
	  writer.consume(_polynomial.getCoef(term), _polynomial.getTerm(term));
  }

  virtual void consume(const mpz_class& coef, const Term& term) {
	ASSERT(term.getVarCount() == _polynomial.getVarCount());

	_polynomial.add(coef, term);
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
	fprintf(stderr,
			"NOTE: Using the format %s makes it necessary to store all of\n"
			"the output in memory before writing it out. This increases\n"
			"memory consumption and decreases performance.\n",
			getName());
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
	  in.printError();
      gmp_fprintf(stderr,
				  "Read the number %Zd while reading Frobenius instance.\n"
				  "Only integers strictly larger than 1 are valid.\n",
				  n.get_mpz_t());
      exit(1);
    }

    numbers.push_back(n);
  }

  if (numbers.empty()) {
	in.printError();
    fputs("Read empty Frobenius instance, which is not allowed.\n", stderr);
    exit(1);
  }

  mpz_class gcd = numbers[0];
  for (size_t i = 1; i < numbers.size(); ++i)
    mpz_gcd(gcd.get_mpz_t(), gcd.get_mpz_t(), numbers[i].get_mpz_t());

  if (gcd != 1) {
    gmp_fprintf(stderr,
				"ERROR: The numbers in the Frobenius instance are not "
				"relatively prime.\nThey are all divisible by %Zd.\n",
				gcd.get_mpz_t());
    exit(1);
  }
}
