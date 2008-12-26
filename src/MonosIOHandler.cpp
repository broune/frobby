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
#include "MonosIOHandler.h"

#include "BigIdeal.h"
#include "Scanner.h"
#include "error.h"

#include <cstdio>

MonosIOHandler::MonosIOHandler():
  IOHandler(staticGetName(),
			"Older format used by the program Monos.", false) {
  registerInput(MonomialIdeal);
  registerInput(MonomialIdealList);
  registerOutput(MonomialIdeal);
  registerOutput(MonomialIdealList);
}

const char* MonosIOHandler::staticGetName() {
  return "monos";
}

void MonosIOHandler::writeTerm(const vector<mpz_class>& term,
							   const VarNames& names,
							   FILE* out) {
  writeTermProduct(term, names, out);
}

void MonosIOHandler::writeIdealHeader(const VarNames& names,
									  bool defineNewRing,
									  FILE* out) {
  fputs("vars ", out);
  const char* pre = "";
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
	fputs(pre, out);
	fputs(names.getName(i).c_str(), out);
	pre = ", ";
  }
  fputs(";\n[", out);
}

void MonosIOHandler::writeTermOfIdeal(const Term& term,
									  const TermTranslator* translator,
									  bool isFirst,
									  FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  IOHandler::writeTermProduct(term, translator, out);
}

void MonosIOHandler::writeTermOfIdeal(const vector<mpz_class> term,
									  const VarNames& names,
									  bool isFirst,
									  FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  IOHandler::writeTermProduct(term, names, out);
}

void MonosIOHandler::writeIdealFooter(const VarNames& names,
									  bool wroteAnyGenerators,
									  FILE* out) {
  fputs("\n];\n", out);
}

void MonosIOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  readVarsAndClearIdeal(ideal, scanner);
  
  scanner.expect('[');
  if (!scanner.match(']')) {
    do {
      readTerm(ideal, scanner);
    } while (scanner.match(','));
	if (!scanner.match(']')) {
	  if (scanner.peekIdentifier())
		scanner.expect('*');
	  else
		scanner.expect(']');
	}
  }
  scanner.expect(';');
}

void MonosIOHandler::readVarsAndClearIdeal(BigIdeal& ideal, Scanner& scanner) {
  ideal.clear();

  scanner.expect("vars");
  if (!scanner.match(';')) {
	do {
	  const char* varName = scanner.readIdentifier();
	  if (!ideal.addVarToClearedIdeal(varName))
		reportSyntaxError
		  (scanner, "The variable " + string(varName) + " is declared twice.");
	} while (scanner.match(','));

	scanner.expect(';');
  }
}
