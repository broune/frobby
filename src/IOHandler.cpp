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
#include "NewMonosIOHandler.h"
#include "MonosIOHandler.h"
#include "Macaulay2IOHandler.h"
#include "Fourti2IOHandler.h"
#include "NullIOHandler.h"

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
  IdealWriter(IOHandler* handler, TermTranslator* translator, FILE* out):
	_handler(handler),
	_translator(translator),
	_out(out),
	_first(true) {
	ASSERT(handler != 0);
	ASSERT(translator != 0);
	ASSERT(out != 0);

	_handler->writeIdealHeader(_translator->getNames(), _out);
  }

  virtual ~IdealWriter() {
	_handler->writeIdealFooter(_out);
  }

  virtual void consume(const Term& term) {
	_handler->writeTermOfIdeal(term, _translator, _first, _out);
	_first = false;
  }

private:
  IOHandler* _handler;
  TermTranslator* _translator;
  FILE* _out;
  bool _first;
};

class DelayedIdealWriter : public TermConsumer {
 public:
  DelayedIdealWriter(IOHandler* handler,
					 TermTranslator* translator,
					 FILE* out):
	_handler(handler),
	_translator(translator),
	_out(out),
	_ideal(translator->getNames().getVarCount()) {
	ASSERT(handler != 0);
	ASSERT(translator != 0);
	ASSERT(out != 0);
  }

  virtual ~DelayedIdealWriter() {
	IdealWriter writer(_handler, _translator, _out);

	Term tmp(_ideal.getVarCount());
	Ideal::const_iterator stop = _ideal.end();
	for (Ideal::const_iterator it = _ideal.begin(); it != stop; ++it) {
	  tmp = *it;
	  writer.consume(tmp);
	}
  }

  virtual void consume(const Term& term) {
	ASSERT(term == _ideal.getVarCount());
	_ideal.insert(term);
  }

private:
  IOHandler* _handler;
  TermTranslator* _translator;
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

IOHandler::~IOHandler() {
}

void IOHandler::writeIdeal(FILE* out, const BigIdeal& ideal) {
  size_t generatorCount = ideal.getGeneratorCount();
  writeIdealHeader(ideal.getNames(), generatorCount, out);
  for (size_t i = 0; i < generatorCount; ++i)
	writeTermOfIdeal(ideal[i], ideal.getNames(), i == 0, out);
  writeIdealFooter(out);
}

bool IOHandler::hasMoreInput(Scanner& scanner) const {
  return !scanner.matchEOF();
}

const char* IOHandler::getName() const {
  return _formatName;
}

const char* IOHandler::getDescription() const {
  return _formatDescription;
}

CoefTermConsumer* IOHandler::createCoefTermWriter
(FILE* out, const TermTranslator* translator) {
  fprintf(stderr, "The format %s does not support polynomials.\n",
		  getName());
  ASSERT(false);
  exit(1);
  return 0;
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

TermConsumer* IOHandler::createIdealWriter(TermTranslator* translator,
										   FILE* out) {
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

void IOHandler::readTerm(BigIdeal& ideal, Scanner& scanner) {
  ideal.newLastTerm();

  if (scanner.match('1'))
    return;

  do {
    readVarPower(ideal.getLastTermRef(), ideal.getNames(), scanner);
  } while (scanner.match('*'));
}

void IOHandler::readVarPower(vector<mpz_class>& term,
							 const VarNames& names, Scanner& scanner) {
  size_t var = scanner.readVariable(names);

  if (term[var] != 0) {
	scanner.printError();
	fputs("A variable appears twice.\n", stderr);
	exit(1);
  }

  if (scanner.match('^')) {
    scanner.readInteger(term[var]);
    if (term[var] <= 0) {
	  scanner.printError();
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


void readFrobeniusInstance(Scanner& scanner, vector<mpz_class>& numbers) {
  numbers.clear();

  string number;
  mpz_class n;
  while (!scanner.matchEOF()) {
	scanner.readInteger(n);

    if (n <= 1) {
	  scanner.printError();
      gmp_fprintf(stderr,
				  "Read the number %Zd while reading Frobenius instance.\n"
				  "Only integers strictly larger than 1 are valid.\n",
				  n.get_mpz_t());
      exit(1);
    }

    numbers.push_back(n);
  }

  if (numbers.empty()) {
	scanner.printError();
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
