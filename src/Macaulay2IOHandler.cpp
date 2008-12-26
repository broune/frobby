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
#include "TermTranslator.h"
#include "error.h"

#include <cstdio>

Macaulay2IOHandler::Macaulay2IOHandler():
  IOHandler(staticGetName(),
			"Format understandable by the program Macaulay 2.", false) {
  registerInput(MonomialIdeal);
  registerInput(MonomialIdealList);
  registerInput(Polynomial);
  registerOutput(MonomialIdeal);
  registerOutput(MonomialIdealList);
  registerOutput(Polynomial);
}

const char* Macaulay2IOHandler::staticGetName() {
  return "m2";
}

void Macaulay2IOHandler::writeIdeals(const vector<BigIdeal*>& ideals,
									const VarNames& names,
									FILE* out) {
  if (ideals.empty())
	writeRing(names, out);
  else
	IOHandler::writeIdeals(ideals, names, out);
}

void Macaulay2IOHandler::writeTerm(const vector<mpz_class>& term,
								   const VarNames& names,
								   FILE* out) {
  writeTermProduct(term, names, out);
}

void Macaulay2IOHandler::writeBareIdeal(const BigIdeal& ideal, FILE* out) {
  size_t generatorCount = ideal.getGeneratorCount();
  fputs("I = monomialIdeal(", out);
  for (size_t i = 0; i < generatorCount; ++i)
	writeTermOfIdeal(ideal[i], ideal.getNames(), i == 0, out);
  writeIdealFooter(ideal.getNames(), generatorCount > 0, out);
}

void Macaulay2IOHandler::writeIdealHeader(const VarNames& names,
										  bool defineNewRing,
										  FILE* out) {
  if (defineNewRing)
	writeRing(names, out);
  fputs("I = monomialIdeal(", out);
}

void Macaulay2IOHandler::writeTermOfIdeal(const Term& term,
										  const TermTranslator* translator,
										  bool isFirst,
										  FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  IOHandler::writeTermProduct(term, translator, out);

  size_t varCount = translator->getVarCount();
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
	fputs("0_R", out); // Macaulay 2's monomialIdeal reports an error otherwise.
  fputs(");\n", out);  
}

void Macaulay2IOHandler::readRing(Scanner& in, VarNames& names) {
  in.expect('R');
  in.expect('=');

  in.eatWhite();
  if (in.peek() == 'Z') {
	displayNote("In the Macaulay 2 format, writing ZZ as the ground field "
				"instead of QQ is deprecated and may not work in future "
				"releases of Frobby.");
	in.expect("ZZ");
  } else
	in.expect("QQ");
  in.expect('[');

  // The enclosing braces are optional, but if the start brace is
  // there, then the end brace should be there too.
  bool readBrace = in.match('{'); 
  if (readBrace) {
	displayNote("In the Macaulay 2 format, putting braces { } around the "
				"variables is deprecated and may not work in future "
				"releases of Frobby.");
  }

  if (in.peekIdentifier()) {
	do {
	  names.addVarSyntaxCheckUnique(in, in.readIdentifier());
	} while (in.match(','));
  }

  if (readBrace)
	in.expect('}');
  in.expect(']');
  in.expect(';');
}

void Macaulay2IOHandler::readIdeal(Scanner& in, BigIdeal& ideal,
								   const VarNames& names) {
  if (in.peek('R'))
	readIdeal(in, ideal);
  else if (in.peek('I'))
	readBareIdeal(in, ideal, names);
  else
	in.expected('R', 'I');
}

void Macaulay2IOHandler::readIdeals(Scanner& in,
									vector<BigIdeal*>& ideals,
									VarNames& names) {
  readRing(in, names);

  // TODO: make test that triggers this error message.
  if (in.match('R'))
	reportSyntaxError(in, "Did not expect another ring description.");

  IOHandler::readIdeals(in, ideals, names);
}

void Macaulay2IOHandler::
readBareIdeal(Scanner& in, BigIdeal& ideal, const VarNames& names) {
  ideal.clearAndSetNames(names);

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
	  if (in.match('_'))
		in.expect('R');
	} while (in.match(','));
  }
  in.expect(')');
  in.expect(';');
}

void Macaulay2IOHandler::readIdeal(Scanner& in, BigIdeal& ideal) {
  VarNames names;
  readRing(in, names);
  readBareIdeal(in, ideal, names);
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
  if (in.peek() == 'Z') {
	displayNote("In the Macaulay 2 format, writing ZZ as the ground field "
				"instead of QQ is deprecated and may not work in future "
				"releases of Frobby.");
	in.expect("ZZ");
  } else
	in.expect("QQ");
  in.expect('[');

  // The enclosing braces are optional, but if the start brace is
  // there, then the end brace should be there too.
  bool readBrace = in.match('{'); 
  if (readBrace) {
	displayNote("In the Macaulay 2 format, putting braces { } around the "
				"variables is deprecated and may not work in future "
				"releases of Frobby.");
  }

  if (in.peekIdentifier()) {
	do {
	  const char* varName = in.readIdentifier();
	  if (names.contains(varName))
		reportSyntaxError
		  (in, "The variable " + string(varName) + " is declared twice.");
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

  if (isFirst)
	fputs("\n ", out);
  else
	fputs(" +\n ", out);

  writeCoefTermProduct(coef, term, translator, true, out);
}

void Macaulay2IOHandler::writeTermOfPolynomial(const mpz_class& coef,
											   const vector<mpz_class>& term,
											   const VarNames& names,
											   bool isFirst,
											   FILE* out) {
  if (isFirst)
	fputs("\n ", out);
  else
	fputs(" +\n ", out);

  writeCoefTermProduct(coef, term, names, true, out);
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
	if (names.getName(i) == "R") {
	  displayNote
		("Using R as a variable name is supported by Frobby, but even though "
		 "this data is being written in Macaulay 2 format, Macaulay 2 will "
		 "likely not be able to read it since it will confuse the variable R "
		 "with the polynomial ring R.");
	}
	// TODO: make the same note for I. Consider using a name like "frobbyRing"
	// and "frobbyIdeal" instead of I and R.

	fputs(names.getName(i).c_str(), out);
	pre = ", ";
  }
  fputs("];\n", out);
}
