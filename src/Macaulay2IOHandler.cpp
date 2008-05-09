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

class Macaulay2IdealWriter : public IdealWriter {
public:
  Macaulay2IdealWriter(FILE* file, const VarNames& names):
    IdealWriter(file, names),
    _justStartedWritingIdeal(true) {
    writeHeader();
  }

  Macaulay2IdealWriter(FILE* file, const TermTranslator* translator):
    IdealWriter(file, translator, true),
    _justStartedWritingIdeal(true) {
    writeHeader();
  }

  virtual ~Macaulay2IdealWriter() {
    fputs("\n);\n", _file);
  }

  virtual void consume(const vector<const char*>& term) {
    writeSeparator();
    writeTerm(term, _file);
  }

  virtual void consume(const vector<mpz_class>& term) {
    writeSeparator();
    writeTerm(term, _names, _file);
  }

  virtual void consume(const Term& term) {
    writeSeparator();
    writeTerm(term, _translator, _file);
  }

private:
  void writeHeader() {
    fputs("R = ZZ[{", _file);

    const char* pre = "";
    for (unsigned int i = 0; i < _names.getVarCount(); ++i) {
      fputs(pre, _file);
      fputs(_names.getName(i).c_str(), _file);
      pre = ", ";
    }
    fputs("}];\n", _file);
    fputs("I = monomialIdeal(", _file);
  }

  void writeSeparator() {
    if (_justStartedWritingIdeal) {
      _justStartedWritingIdeal = false;
      fputs("\n", _file);
    }
    else
      fputs(",\n", _file);
  }

  bool _justStartedWritingIdeal;
};

IdealWriter* Macaulay2IOHandler::
createWriter(FILE* file, const VarNames& names) const {
  return new Macaulay2IdealWriter(file, names);
}

IdealWriter* Macaulay2IOHandler::
createWriter(FILE* file, const TermTranslator* translator) const {
  return new Macaulay2IdealWriter(file, translator);
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

const char* Macaulay2IOHandler::getFormatName() const {
  return "m2";
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
  scanner.expect("ZZ");
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
