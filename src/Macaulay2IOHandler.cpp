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
#include "BigPolynomial.h"

// TODO: are these necessary?
#include "Term.h"
#include "TermTranslator.h"

Macaulay2IOHandler::Macaulay2IOHandler():
  IOHandler("m2", "Format understandable by the program Macaulay 2.", false) {
  registerInput(MonomialIdeal);
  registerInput(MonomialIdealList);
  registerInput(Polynomial);
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

  size_t varCount = term.getVarCount();
  for (size_t var = 0; var < varCount; ++var)
	if (translator->getExponent(var, term) != 0)
	  return;
  fputs("_R", out);
}

void Macaulay2IOHandler::writeTermOfIdeal(const vector<mpz_class> term,
										  const VarNames& names,
										  bool isFirst,
										  FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  IOHandler::writeTermProduct(term, names, out);

  size_t varCount = term.size();
  for (size_t var = 0; var < varCount; ++var)
	if (term[var] != 0)
	  return;
  fputs("_R", out);
}

void Macaulay2IOHandler::writeIdealFooter(const VarNames& names,
										  bool wroteAnyGenerators,
										  FILE* out) {
  if (wroteAnyGenerators)
	fputc('\n', out);
  else
	fputs("0_R", out); // monomialIdeal reports an error otherwise.
  fputs(");\n", out);  
}

void Macaulay2IOHandler::readIdeal(Scanner& in, BigIdeal& ideal) {
  {
	VarNames names;
	readVars(names, in);
	ideal.clearAndSetNames(names);
  }

  in.expect('I');
  in.expect('=');
  in.expect("monomialIdeal");
  in.expect('(');

  if (in.match('0')) {
	if (in.match('_'))
	  in.expect('R');
  } else {
	do {
	  readTerm(ideal, in);
	  
	  vector<mpz_class>& term = ideal.getLastTermRef();
	  bool isIdentity = true;
	  size_t varCount = term.size();
	  for (size_t var = 0; var < varCount; ++var) {
		if (term[var] != 0) {
		  isIdentity = false;
		  break;
		}
	  }
	  if (in.match('_'))
		in.expect('R');
	} while (in.match(','));
  }
  in.expect(')');
  in.expect(';');
}

void Macaulay2IOHandler::readPolynomial(Scanner& in,
										BigPolynomial& polynomial) {
  {
	VarNames names;
	readVars(names, in);
	polynomial.clearAndSetNames(names);
  }

  in.expect('p');
  in.expect('=');

  bool first = true;
  do {
	readCoefTerm(polynomial, first, in);
	first = false;
  } while (!in.match(';'));
}

void Macaulay2IOHandler::readVars(VarNames& names, Scanner& in) {
  in.expect('R');
  in.expect('=');
  in.eatWhite();
  if (in.peek() == 'Z')
	in.expect("ZZ");
  else
	in.expect("QQ");
  in.expect('[');

  // The enclosing braces are optional, but if the start brace is
  // there, then the end brace should be there too.
  bool readBrace = in.match('{'); 

  if (in.peekIdentifier()) {
	do {
	  const char* varName = in.readIdentifier();
	  if (names.contains(varName)) {
		in.printError();
		fprintf(stderr, "The variable %s is declared twice.\n", varName);
		exit(1);
	  }
	  names.addVar(varName);
	} while (in.match(','));
  }

  if (readBrace)
	in.expect('}');
  in.expect(']');
  in.expect(';');
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
  ASSERT(translator != 0);
  ASSERT(out != 0);

  fputs("\n ", out);
  writeCoefTermProduct(coef, term, translator, isFirst, out);
}

void Macaulay2IOHandler::writeTermOfPolynomial(const mpz_class& coef,
											   const vector<mpz_class>& term,
											   const VarNames& names,
											   bool isFirst,
											   FILE* out) {
  fputs("\n ", out);
  writeCoefTermProduct(coef, term, names, isFirst, out);
}

void Macaulay2IOHandler::writePolynomialFooter(const VarNames& names,
											   bool wroteAnyGenerators,
											   FILE* out) {
  if (!wroteAnyGenerators)
	fputs("\n 0", out);
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
