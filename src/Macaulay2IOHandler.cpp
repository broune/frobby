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
#include "BigTermConsumer.h"
#include "error.h"
#include "DataType.h"
#include "CoefBigTermConsumer.h"

#include <cstdio>

Macaulay2IOHandler::Macaulay2IOHandler():
  IOHandlerCommon(staticGetName(),
				  "Format understandable by the program Macaulay 2.") {
  registerInput(DataType::getMonomialIdealType());
  registerInput(DataType::getMonomialIdealListType());
  registerInput(DataType::getPolynomialType());
  registerOutput(DataType::getMonomialIdealType());
  registerOutput(DataType::getMonomialIdealListType());
  registerOutput(DataType::getPolynomialType());
}

const char* Macaulay2IOHandler::staticGetName() {
  return "m2";
}

string Macaulay2IOHandler::getRingName(const VarNames& names) {
  if (!names.contains("R"))
	return "R";

  string name;
  for (mpz_class i = 1; true; ++i) {
	name = "R" + i.get_str();
	if (!names.contains(name))
	  return name;
  }
}

void Macaulay2IOHandler::writeTerm(const vector<mpz_class>& term,
								   const VarNames& names,
								   FILE* out) {
  writeTermProduct(term, names, out);
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

  fputc('_', out);
  fputs(getRingName(translator->getNames()).c_str(), out);
}

void Macaulay2IOHandler::writeTermOfIdeal(const vector<mpz_class>& term,
										  const VarNames& names,
										  bool isFirst,
										  FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  IOHandler::writeTermProduct(term, names, out);

  size_t varCount = term.size();
  for (size_t var = 0; var < varCount; ++var)
	if (term[var] != 0)
	  return;

  fputc('_', out);
  fputs(getRingName(names).c_str(), out);
}

void Macaulay2IOHandler::writeIdealFooter(const VarNames& names,
										  bool wroteAnyGenerators,
										  FILE* out) {
  if (wroteAnyGenerators)
	fputc('\n', out);
  else {
	// Macaulay 2's monomialIdeal reports an error if 0 is not
	// explicitly embedded in the a polynomial ideal.
	fputs("0_", out);
	fputs(getRingName(names).c_str(), out);
  }
  fputs(");\n", out);  
}

void Macaulay2IOHandler::readRing(Scanner& in, VarNames& names) {
  names.clear();
  const char* ringName = in.readIdentifier();
  ASSERT(ringName != 0 && string(ringName) != "");
  if (ringName[0] != 'R') {
	reportSyntaxError
	  (in, "Expected name of ring to start with an upper case R.");
	ASSERT(false); // shouldn't reach here.
  }

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

void Macaulay2IOHandler::readBareIdeal(Scanner& in,
									   const VarNames& names,
									   BigTermConsumer& consumer) {
  consumer.beginConsuming(names);
  vector<mpz_class> term(names.getVarCount());

  in.expect('I');
  in.expect('=');
  in.expect("monomialIdeal");
  in.expect('(');

  if (in.match('0')) {
	if (in.match('_'))
	  in.readIdentifier();
  } else {
	do {
	  readTerm(in, names, term);
	  if (in.match('_'))
		in.readIdentifier();
	  consumer.consume(term);
	} while (in.match(','));
  }
  in.expect(')');
  in.expect(';');
  consumer.doneConsuming();
}

void Macaulay2IOHandler::readBarePolynomial
(Scanner& in, const VarNames& names, CoefBigTermConsumer& consumer) {
  consumer.consumeRing(names);
  vector<mpz_class> term(names.getVarCount());
  mpz_class coef;

  in.expect('p');
  in.expect('=');

  consumer.beginConsuming();
  bool first = true;
  do {
	readCoefTerm(coef, term, names, first, in);
	consumer.consume(coef, term);
	first = false;
  } while (!in.match(';'));
  consumer.doneConsuming();
}

bool Macaulay2IOHandler::peekRing(Scanner& in) {
  return in.peek('R') || in.peek('r');
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
  fputs(getRingName(names).c_str(), out);
  fputs(" = QQ[", out);

  const char* pre = "";
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
	fputs(pre, out);
	if (names.getName(i) == "R") {
	  string msg = 
		"The name of the ring in Macaulay 2 format is usually named R,\n"
		"but in this case there is already a variable named R. Thus,\n"
		"the ring has been renamed to " + getRingName(names) + '.';
	  displayNote(msg);
	}
	fputs(names.getName(i).c_str(), out);
	pre = ", ";
  }
  fputs("];\n", out);
}
