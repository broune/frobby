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
#include "NewMonosIOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "VarNames.h"

NewMonosIOHandler::NewMonosIOHandler():
  IOHandler("newmonos", "Newer format used by the program Monos.", false) {
  registerInput(MonomialIdeal);
  registerInput(MonomialIdealList);
  registerOutput(MonomialIdeal);
}

void NewMonosIOHandler::writeIdealHeader(const VarNames& names, FILE* out) {
  fputs("(monomial-ideal-with-order\n (lex-order", out);
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
	putc(' ', out);
	fputs(names.getName(i).c_str(), out);
  }
  fputc(')', out);
}

void NewMonosIOHandler::writeTermOfIdeal(const Term& term,
										 const TermTranslator* translator,
										 bool isFirst,
										 FILE* out) {
  fputs("\n ", out);
  IOHandler::writeTermProduct(term, translator, out);
}

void NewMonosIOHandler::writeTermOfIdeal(const vector<mpz_class> term,
										 const VarNames& names,
										 bool isFirst,
										 FILE* out) {
  fputs("\n ", out);
  IOHandler::writeTermProduct(term, names, out);
}

void NewMonosIOHandler::writeIdealFooter(FILE* out) {
  fputs("\n)\n", out);
}

void NewMonosIOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  scanner.expect('(');
  scanner.expect("monomial-ideal-with-order");
  readVarsAndClearIdeal(ideal, scanner);

  while (!scanner.match(')'))
    readTerm(ideal, scanner);
}

void NewMonosIOHandler::readVarsAndClearIdeal(BigIdeal& ideal, Scanner& scanner) {
  scanner.expect('(');
  scanner.expect("lex-order");

  VarNames names;
  while (!scanner.match(')')) {
	const char* varName = scanner.readIdentifier();
    if (names.contains(varName)) {
	  scanner.printError();
      fprintf(stderr, "The variable %s is declared twice.\n", varName);
      exit(1);
    }
    names.addVar(varName);
  }

  ideal.clearAndSetNames(names);
}
