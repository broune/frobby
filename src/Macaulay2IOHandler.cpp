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
#include "Macaulay2IOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "VarNames.h"
#include "CoefTermConsumer.h"

Macaulay2IOHandler::Macaulay2IOHandler():
  IOHandler("m2", "Format understandable by the program Macaulay 2.", false) {
  registerInput(MonomialIdeal);
  registerInput(MonomialIdealList);
  registerOutput(MonomialIdeal);
  registerOutput(Polynomial);
}

void Macaulay2IOHandler::writeIdealHeader(const VarNames& names, FILE* out) {
    fputs("R = QQ[", out);

    const char* pre = "";
    for (unsigned int i = 0; i < names.getVarCount(); ++i) {
      fputs(pre, out);
      fputs(names.getName(i).c_str(), out);
      pre = ", ";
    }
    fputs("];\n", out);
    fputs("I = monomialIdeal(", out);
}

void Macaulay2IOHandler::writeTermOfIdeal(const Term& term,
										  const TermTranslator* translator,
										  bool isFirst,
										  FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  IOHandler::writeTermProduct(term, translator, out);
}

void Macaulay2IOHandler::writeTermOfIdeal(const vector<mpz_class> term,
										  const VarNames& names,
										  bool isFirst,
										  FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  IOHandler::writeTermProduct(term, names, out);
}

void Macaulay2IOHandler::writeIdealFooter(FILE* out) {
  fputs("\n);\n", out);  
}

void Macaulay2IOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  readVarsAndClearIdeal(ideal, scanner);

  scanner.expect('I');
  scanner.expect('=');
  scanner.expect("monomialIdeal");
  scanner.expect('(');

  if (!scanner.match(')')) {
	do
	  readTerm(ideal, scanner);
	while (scanner.match(','));
	scanner.expect(')');
  }
  scanner.match(';');
  scanner.expectEOF();
}

void Macaulay2IOHandler::readIrreducibleDecomposition(Scanner& scanner,
													  BigIdeal& decom) {
  readVarsAndClearIdeal(decom, scanner);
  readIrreducibleIdealList(decom, scanner);
  scanner.expectEOF();
}

void Macaulay2IOHandler::readIrreducibleIdeal(BigIdeal& ideal, Scanner& scanner) {
  ideal.newLastTerm();

  scanner.expect("monomialIdeal");
  scanner.expect('(');

  do
    readVarPower(ideal.getLastTermRef(), ideal.getNames(), scanner);
  while (scanner.match(','));

  scanner.expect(')');
}

void Macaulay2IOHandler::readIrreducibleIdealList(BigIdeal& ideals,
						  Scanner& scanner) {
  scanner.expect('{');
  if (scanner.match('}'))
    return;

  do {
    readIrreducibleIdeal(ideals, scanner);
  } while (scanner.match(','));

  scanner.expect('}');
  scanner.match(';');
}

void Macaulay2IOHandler::readVarsAndClearIdeal(BigIdeal& ideal, Scanner& scanner) {
  scanner.expect('R');
  scanner.expect('=');
  scanner.eatWhite();
  if (scanner.peek() == 'Z')
	scanner.expect("ZZ");
  else
	scanner.expect("QQ");
  scanner.expect('[');

  // The enclosing braces are optional, but if the start brace is
  // there, then the end brace should be there too.
  bool readBrace = scanner.match('{'); 

  VarNames names;
  if (scanner.peekIdentifier()) {
	do {
	  const char* varName = scanner.readIdentifier();
	  if (names.contains(varName)) {
		scanner.printError();
		fprintf(stderr, "The variable %s is declared twice.\n", varName);
		exit(1);
	  }
	  names.addVar(varName);
	} while (scanner.match(','));
  }

  if (readBrace)
	scanner.expect('}');
  scanner.expect(']');
  scanner.match(';');

  ideal.clearAndSetNames(names);
}

// TODO: integrate this better and move some of it elsewhere
#include "TermTranslator.h"
#include "Term.h"
class CoefTermWriter : public CoefTermConsumer {
 public:
  CoefTermWriter(FILE* file, const TermTranslator* translator):
	_file(file),
	_translator(translator),
	_justStartedWriting(true) {
	// TODO: get rid of this code duplication from the ideal writer.
    fputs("R = ZZ[{", _file);

	const VarNames& names = _translator->getNames();
    const char* pre = "";
    for (unsigned int i = 0; i < names.getVarCount(); ++i) {
      fputs(pre, _file);
      fputs(names.getName(i).c_str(), _file);
      pre = ", ";
    }
    fputs("}];\n", _file);
    fputs("p = ", _file);
  }

  virtual ~CoefTermWriter() {
	if (_justStartedWriting)
	  fputc('0', _file);
    fputs(";\n", _file);
  };

  virtual void consume(const mpz_class& coef, const Term& term) {
    if (_justStartedWriting)
      _justStartedWriting = false;
	else if (coef >= 0)
	  fputc('+', _file);

	bool needsSeperator = true;
	if (coef == 1) {
	  if (term.isIdentity()) {
		fputc('1', _file);
		return;
	  }
	  needsSeperator = false;
	}
	if (coef != 1) {
	  if (coef == -1) {
		fputc('-', _file);
		if (term.isIdentity()) {
		  fputc('1', _file);
		  return;
		}
		needsSeperator = false;
	  }
	  else
		gmp_fprintf(_file, "%Zd", coef.get_mpz_t());
	}

	size_t varCount = term.getVarCount();
	for (size_t j = 0; j < varCount; ++j) {
	  const char* exp = _translator->getVarExponentString(j, term[j]);
	  if (exp == 0)
		continue;

	  if (needsSeperator)
		putc('*', _file);
	  else
		needsSeperator = true;

	  fputs(exp, _file);
	}
  }

private:
  FILE* _file;
  const TermTranslator* _translator;
  bool _justStartedWriting;
};

CoefTermConsumer* Macaulay2IOHandler::createCoefTermWriter
(FILE* file, const TermTranslator* translator) {
  return new CoefTermWriter(file, translator);
}
