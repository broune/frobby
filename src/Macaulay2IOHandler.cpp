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

// TODO: are these necessary?
#include "Term.h"
#include "TermTranslator.h"

Macaulay2IOHandler::Macaulay2IOHandler():
  IOHandler("m2", "Format understandable by the program Macaulay 2.", false) {
  registerInput(MonomialIdeal);
  registerInput(MonomialIdealList);
  registerOutput(MonomialIdeal);
  registerOutput(Polynomial);
}

void Macaulay2IOHandler::writeIdealHeader(const VarNames& names, FILE* out) {
  writeRing(names, out);
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

void Macaulay2IOHandler::writeIdealFooter(const VarNames& names,
										  bool wroteAnyGenerators,
										  FILE* out) {
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




void Macaulay2IOHandler::writePolynomialHeader(const VarNames& names,
											   FILE* out) {
  writeRing(names, out);
  fputs("p =", out);
}

void Macaulay2IOHandler::writeTermOfPolynomial
(const mpz_class& coef,
 const Term& term,
 const TermTranslator* translator,
 bool isFirst,
 FILE* out) {
  fputs("\n ", out);
  writeCoefTermProduct(coef, term, translator, isFirst, out);
}

void Macaulay2IOHandler::writePolynomialFooter(const VarNames& names,
											   bool wroteAnyGenerators,
											   FILE* out) {
  if (!wroteAnyGenerators)
	fputs(" 0", out);
  fputs(";\n", out);
}

void Macaulay2IOHandler::writeRing(const VarNames& names, FILE* out) {
  fputs("R = QQ[", out);

  const char* pre = "";
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
	fputs(pre, out);
	fputs(names.getName(i).c_str(), out);
	pre = ", ";
  }
  fputs("];\n", out);
}
