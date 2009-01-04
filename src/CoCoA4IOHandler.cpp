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
#include "CoCoA4IOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "VarNames.h"
#include "CoefTermConsumer.h"
#include "Term.h"
#include "TermTranslator.h"
#include "BigPolynomial.h"
#include "error.h"
#include "FrobbyStringStream.h"
#include "BigTermConsumer.h"
#include "DataType.h"
#include "CoefBigTermConsumer.h"

#include <cstdio>

CoCoA4IOHandler::CoCoA4IOHandler():
  IOHandlerCommon(staticGetName(),
				  "Format understandable by the program CoCoA 4.") {
  registerInput(DataType::getMonomialIdealType());
  registerInput(DataType::getMonomialIdealListType());
  registerInput(DataType::getPolynomialType());
  registerOutput(DataType::getMonomialIdealType());
  registerOutput(DataType::getPolynomialType());
}

const char* CoCoA4IOHandler::staticGetName() {
  return "cocoa4";
}

void CoCoA4IOHandler::writeTerm(const vector<mpz_class>& term,
								const VarNames& names,
								FILE* out) {
  writeCoCoA4TermProduct(term, names, out);
}

void CoCoA4IOHandler::writeIdealHeader(const VarNames& names,
									   bool defineNewRing,
									   FILE* out) {
  writeRing(names, out);
  fputs("I := Ideal(", out);
}

void CoCoA4IOHandler::writeTermOfIdeal(const Term& term,
									   const TermTranslator* translator,
									   bool isFirst,
									   FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  writeCoCoA4TermProduct(term, translator, out);
}

void CoCoA4IOHandler::writeTermOfIdeal(const vector<mpz_class>& term,
									   const VarNames& names,
									   bool isFirst,
									   FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  writeCoCoA4TermProduct(term, names, out);
}

void CoCoA4IOHandler::writeIdealFooter(const VarNames& names,
									   bool wroteAnyGenerators,
									   FILE* out) {
  fputs("\n);\n", out);  
}

void CoCoA4IOHandler::writePolynomialHeader(const VarNames& names,
											FILE* out) {
  writeRing(names, out);
  fputs("p :=", out);
}

void CoCoA4IOHandler::writeTermOfPolynomial(const mpz_class& coef,
											const Term& term,
											const TermTranslator* translator,
											bool isFirst,
											FILE* out) {
  fputs("\n ", out);

  if (coef >= 0 && !isFirst)
	fputc('+', out);

  if (term.isIdentity()) {
	gmp_fprintf(out, "%Zd", coef.get_mpz_t());
	return;
  }

  if (coef == -1)
	fputc('-', out);
  else if (coef != 1)
	gmp_fprintf(out, "%Zd", coef.get_mpz_t());

  writeCoCoA4TermProduct(term, translator, out);
}

void CoCoA4IOHandler::writeTermOfPolynomial(const mpz_class& coef,
											const vector<mpz_class>& term,
											const VarNames& names,
											bool isFirst,
											FILE* out) {
  fputs("\n ", out);
  if (coef >= 0 && !isFirst)
	fputc('+', out);

  bool isIdentity = true;
  for (size_t var = 0; var < term.size(); ++var)
	if (term[var] != 0)
	  isIdentity = false;

  if (isIdentity) {
	gmp_fprintf(out, "%Zd", coef.get_mpz_t());
	return;
  }

  if (coef == -1)
	fputc('-', out);
  else if (coef != 1)
	gmp_fprintf(out, "%Zd", coef.get_mpz_t());

  writeCoCoA4TermProduct(term, names, out);
}

void CoCoA4IOHandler::writePolynomialFooter(const VarNames& names,
											bool wroteAnyGenerators,
											FILE* out) {
  if (!wroteAnyGenerators)
	fputs("\n 0", out);
  fputs(";\n", out);
}

void CoCoA4IOHandler::readRing(Scanner& in, VarNames& names) {
  names.clear();

  in.expect("Use");
  in.expect('R');
  in.expect("::=");
  in.expect('Q');
  in.expect('[');
  in.expect('x');

  size_t varCount = 0;
  if (in.match('[')) {
	in.expect('1');
	in.expect("..");
	in.readSizeT(varCount);
	in.expect(']');
  }
  in.expect(']');
  in.expect(';');

  in.expect("Names");
  in.expect(":=");
  in.expect('[');

  for (size_t var = 0; var < varCount; ++var) {
	in.expect('\"');
	if (in.peekWhite())
	  reportSyntaxError(in, "Variable name contains space.");

	names.addVarSyntaxCheckUnique(in, in.readIdentifier());

	if (in.peekWhite())
	  reportSyntaxError(in, "Variable name contains space.");

	in.expect('\"');
	if (var < varCount - 1)
	  in.expect(',');
  }

  in.expect(']');
  in.expect(';');
}

void CoCoA4IOHandler::readBareIdeal(Scanner& in, const VarNames& names,
									BigTermConsumer& consumer) {
  consumer.beginConsuming(names);
  vector<mpz_class> term(names.getVarCount());

  in.expect('I');
  in.expect(":=");
  in.expect("Ideal");
  in.expect('(');

  if (!in.match(')')) {
	do {
	  readCoCoA4Term(term, in);
	  consumer.consume(term);
	} while (in.match(','));
	in.expect(')');
  }
  in.match(';');

  consumer.doneConsuming();
}

void CoCoA4IOHandler::readBarePolynomial
(Scanner& in, const VarNames& names, CoefBigTermConsumer& consumer) {
  consumer.consumeRing(names);
  vector<mpz_class> term(names.getVarCount());
  mpz_class coef;

  in.expect('p');
  in.expect(":=");

  consumer.beginConsuming();
  bool first = true;
  do {
	readCoCoA4CoefTerm(coef, term, first, in);
	consumer.consume(coef, term);
	first = false;
  } while (!in.match(';'));
  consumer.doneConsuming();
}

bool CoCoA4IOHandler::peekRing(Scanner& in) {
  return in.peek('U') || in.peek('u');
}

void CoCoA4IOHandler::writeRing(const VarNames& names, FILE* out) {
  if (names.getVarCount() ==  0) {
	fputs("Use R ::= Q[x];\nNames := [];\n", out);
	return;
  }

  fprintf(out, "Use R ::= Q[x[1..%lu]];\n",
		  (unsigned long)names.getVarCount());

  fputs("Names := [", out);

  const char* pre = "\"";
  for (size_t i = 0; i < names.getVarCount(); ++i) {
	fputs(pre, out);
	fputs(names.getName(i).c_str(), out);
	pre = "\", \"";
  }
  fputs("\"];\n", out);
}

void CoCoA4IOHandler::writeCoCoA4TermProduct(const Term& term,
											 const TermTranslator* translator,
											 FILE* out) {
  bool seenNonZero = false;
  size_t varCount = term.getVarCount();
  for (size_t var = 0; var < varCount; ++var) {
	const char* exp = translator->getExponentString(var, term[var]);
	if (exp == 0)
	  continue;
	seenNonZero = true;

	fprintf(out, "x[%lu]", (unsigned long)(var + 1));
	if (exp[0] != '1' || exp[1] != '\0') {
	  fputc('^', out);
	  fputs(exp, out);
	}
  }

  if (!seenNonZero)
	fputc('1', out);
}

void CoCoA4IOHandler::writeCoCoA4TermProduct(const vector<mpz_class>& term,
											 const VarNames& names,
											 FILE* out) {
  bool seenNonZero = false;
  size_t varCount = term.size();
  for (size_t var = 0; var < varCount; ++var) {
	if (term[var] == 0)
	  continue;
	seenNonZero = true;

	fprintf(out, "x[%lu]", (unsigned long)(var + 1));
	if (term[var] != 1) {
	  fputc('^', out);
	  mpz_out_str(out, 10, term[var].get_mpz_t());
	}
  }

  if (!seenNonZero)
	fputc('1', out);
}

void CoCoA4IOHandler::readCoCoA4Term(vector<mpz_class>& term, Scanner& in) {
  for (size_t var = 0; var < term.size(); ++var)
	term[var] = 0;

  if (in.match('1'))
	return;

  do {
	readCoCoA4VarPower(term, in);
	in.eatWhite();
  } while (in.peek() == 'x');
}

void CoCoA4IOHandler::readCoCoA4VarPower(vector<mpz_class>& term,
										 Scanner& in) {
  in.expect('x');
  in.expect('[');

  size_t var;
  in.readSizeT(var);
  if (var > term.size()) {
	FrobbyStringStream errorMsg;
	errorMsg << "There is no variable x[" << var << "].";
	reportSyntaxError(in, errorMsg);
  }
  --var;

  in.expect(']');

  if (term[var] != 0) {
	FrobbyStringStream errorMsg;
	errorMsg << "The variable x["
			 << (var + 1)
			 << "] appears twice in the same monomial.";
	reportSyntaxError(in, errorMsg);
  }

  if (in.match('^')) {
	in.readInteger(term[var]);
	if (term[var] <= 0) {
	  FrobbyStringStream errorMsg;
	  errorMsg << "Expected positive integer as exponent but got "
			   << term[var] << '.';
	  reportSyntaxError(in, errorMsg);
	}
  } else
	term[var] = 1;
}

void CoCoA4IOHandler::readCoCoA4CoefTerm
(mpz_class& coef,
 vector<mpz_class>& term,
 bool firstTerm,
 Scanner& in) {
  for (size_t var = 0; var < term.size(); ++var)
	term[var] = 0;

  bool positive = true;
  if (!firstTerm && in.match('+'))
	positive = !in.match('-');
  else if (in.match('-'))
	positive = false;
  else if (!firstTerm) {
	in.expect('+');
	return;
  }
  if (in.match('+') || in.match('-'))
	reportSyntaxError(in, "Too many adjacent signs.");

  if (in.peekIdentifier()) {
	coef = 1;
	readCoCoA4VarPower(term, in);
  } else
	in.readInteger(coef);

  in.eatWhite();
  while (in.peek() == 'x') {
	readCoCoA4VarPower(term, in);
	in.eatWhite();
  }

  if (!positive)
	coef = -coef;
}

void CoCoA4IOHandler::readCoCoA4CoefTerm(BigPolynomial& polynomial,
										 bool firstTerm,
										 Scanner& in) {
  polynomial.newLastTerm();
  mpz_class& coef = polynomial.getLastCoef();
  vector<mpz_class>& term = polynomial.getLastTerm();

  readCoCoA4CoefTerm(coef, term, firstTerm, in);
}
