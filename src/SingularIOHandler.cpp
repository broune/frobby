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
#include "BigPolynomial.h"
#include "error.h"
#include "BigTermConsumer.h"
#include "FrobbyStringStream.h"
#include "DataType.h"
#include "CoefBigTermConsumer.h"

#include <cstdio>

SingularIOHandler::SingularIOHandler():
  IOHandlerCommon(staticGetName(),
				  "Format understandable by the program Singular.") {
  registerInput(DataType::getMonomialIdealType());
  registerInput(DataType::getMonomialIdealListType());
  registerInput(DataType::getPolynomialType());
  registerOutput(DataType::getMonomialIdealType());
  registerOutput(DataType::getPolynomialType());
}

const char* SingularIOHandler::staticGetName() {
  return "singular";
}

void SingularIOHandler::writeTerm(const vector<mpz_class>& term,
								   const VarNames& names,
								   FILE* out) {
  writeTermProduct(term, names, out);
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

void SingularIOHandler::writeTermOfPolynomial(const mpz_class& coef,
											  const vector<mpz_class>& term,
											  const VarNames& names,
											  bool isFirst,
											  FILE* out) {
  fputs("\n ", out);
  writeCoefTermProduct(coef, term, names, isFirst, out);
}

void SingularIOHandler::writePolynomialFooter(const VarNames& names,
											  bool wroteAnyGenerators,
											  FILE* out) {
  if (!wroteAnyGenerators)
	fputs("\n 0", out);
  fputs(";\n", out);
}

void SingularIOHandler::writeIdealHeader(const VarNames& names,
										 bool defineNewRing,
										 FILE* out) {
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

void SingularIOHandler::writeTermOfIdeal(const vector<mpz_class>& term,
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
	fputs("\n 0", out);
  fputs(";\n", out);  
}

void SingularIOHandler::readRing(Scanner& in, VarNames& names) {
  names.clear();

  in.expect("ring");
  in.expect('R');
  in.expect('=');
  in.expect('0');
  in.expect(',');
  in.expect('(');

  do {
	names.addVarSyntaxCheckUnique(in, in.readIdentifier());
  } while (in.match(','));

  in.expect(')');
  in.expect(',');
  in.expect("lp");
  in.expect(';');

  in.expect("int");
  in.expect("noVars");
  in.expect("=");
  if (in.match('1')) {
	if (names.getVarCount() != 1 ||
		names.getName(0) != string("dummy")) {
	  FrobbyStringStream errorMsg;
	  errorMsg <<
		"A singular ring with no actual variables must have a single "
		"place-holder variable named \"dummy\", and in this case ";
	  if (names.getVarCount() != 1)
		errorMsg << "there are " << names.getVarCount()
				 << " place-holder variables.";
	  else
		errorMsg << "it has the name \"" << names.getName(0) << "\".";

	  reportSyntaxError(in, errorMsg);
	}
	names.clear();
  } else if (!in.match('0')) {
	// TODO: Replace following line with: in.expect('0', '1');
	reportSyntaxError(in, "noVars must be either 0 or 1.");
  }

  in.expect(';');
}

void SingularIOHandler::readBareIdeal(Scanner& in, const VarNames& names,
									  BigTermConsumer& consumer) {
  consumer.beginConsuming(names);
  vector<mpz_class> term(names.getVarCount());

  in.expect("ideal");
  in.expect('I');
  in.expect('=');

  if (!in.match('0')) {
	do {
	  readTerm(in, names, term);
	  consumer.consume(term);
	} while (in.match(','));
  }
  in.expect(';');

  consumer.doneConsuming();
}

void SingularIOHandler::readBarePolynomial
(Scanner& in, const VarNames& names, CoefBigTermConsumer& consumer) {
  consumer.consumeRing(names);
  vector<mpz_class> term(names.getVarCount());
  mpz_class coef;

  in.expect("poly");
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

bool SingularIOHandler::peekRing(Scanner& in) {
  return in.peek('r') || in.peek('R');
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
