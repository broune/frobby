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
#include "SingularIOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "VarNames.h"
#include "CoefTermConsumer.h"

SingularIOHandler::SingularIOHandler():
  IOHandler("singular",
			"Format understandable by the program Singular.", false) {
  registerInput(MonomialIdeal);
  registerInput(MonomialIdealList);
  registerOutput(MonomialIdeal);
  registerOutput(Polynomial);
}

void SingularIOHandler::writePolynomialHeader(const VarNames& names,
											  FILE* out) {
  writeRing(names, out);
  fputs("poly p =", out);
}

void SingularIOHandler::writeTermOfPolynomial(const mpz_class& coef,
											  const Term& term,
											  const TermTranslator* translator,
											  bool isFirst,
											  FILE* out) {
  fputs("\n ", out);
  writeCoefTermProduct(coef, term, translator, isFirst, out);
}

void SingularIOHandler::writePolynomialFooter(const VarNames& names,
											  bool wroteAnyGenerators,
											  FILE* out) {
  if (!wroteAnyGenerators)
	fputs(" 0", out);
  fputs(";\n", out);
}

void SingularIOHandler::writeIdealHeader(const VarNames& names, FILE* out) {
  writeRing(names, out);
  fputs("ideal I =", out);
}

void SingularIOHandler::writeTermOfIdeal(const Term& term,
										 const TermTranslator* translator,
										 bool isFirst,
										 FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  IOHandler::writeTermProduct(term, translator, out);
}

void SingularIOHandler::writeTermOfIdeal(const vector<mpz_class> term,
										 const VarNames& names,
										 bool isFirst,
										 FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  IOHandler::writeTermProduct(term, names, out);
}

void SingularIOHandler::writeIdealFooter(const VarNames& names,
										 bool wroteAnyGenerators,
										 FILE* out) {
  if (!wroteAnyGenerators)
	fputs(" 0", out);
  fputs(";\n", out);  
}

void SingularIOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  readVarsAndClearIdeal(ideal, scanner);

  scanner.expect("ideal");
  scanner.expect('I');
  scanner.expect('=');

  if (!scanner.match('0')) {
	do
	  readTerm(ideal, scanner);
	while (scanner.match(','));
  }
  scanner.expect(';');
}

void SingularIOHandler::readVarsAndClearIdeal(BigIdeal& ideal, Scanner& scanner) {
  scanner.expect("ring");
  scanner.expect('R');
  scanner.expect('=');
  scanner.expect('0');
  scanner.expect(',');
  scanner.expect('(');

  VarNames names;
  do {
	const char* varName = scanner.readIdentifier();
	if (names.contains(varName)) {
	  scanner.printError();
	  fprintf(stderr, "The variable %s is declared twice.\n", varName);
	  exit(1);
	}
	names.addVar(varName);
  } while (scanner.match(','));

  scanner.expect(')');
  scanner.expect(',');
  scanner.expect("lp");
  scanner.expect(';');

  scanner.expect("int");
  scanner.expect("noVars");
  scanner.expect("=");
  if (scanner.match('1')) {
	if (names.getVarCount() != 1 ||
		names.getName(0) != string("dummy")) {
	  fputs("ERROR: Encountered variable name other than \"dummy\" in\n"
			"Singular ring with no variables.\n", stderr);
	  exit(1);
	}
	names.clear();
  }
  else if (!scanner.match('0')) {
	scanner.printError("Expected 0 or 1.\n");
	exit(1);
  }
  scanner.expect(';');

  ideal.clearAndSetNames(names);
}

void SingularIOHandler::writeRing(const VarNames& names, FILE* out) {
  if (names.getVarCount() == 0)
	fputs("ring R = 0, (dummy), lp;\nint noVars = 1;\n", out);
  else {
    fputs("ring R = 0, (", out);

    const char* pre = "";
    for (unsigned int i = 0; i < names.getVarCount(); ++i) {
      fputs(pre, out);
      fputs(names.getName(i).c_str(), out);
      pre = ", ";
    }
    fputs("), lp;\nint noVars = 0;\n", out);
  }
}
