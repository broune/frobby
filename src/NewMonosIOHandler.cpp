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
#include "error.h"
#include "BigTermConsumer.h"
#include "DataType.h"

#include <cstdio>

NewMonosIOHandler::NewMonosIOHandler():
  IOHandler(staticGetName(),
			"Newer format used by the program Monos.",
			false) {
  registerInput(DataType::getMonomialIdealType());
  registerInput(DataType::getMonomialIdealListType());
  registerOutput(DataType::getMonomialIdealType());
}

const char* NewMonosIOHandler::staticGetName() {
  return "newmonos";
}

void NewMonosIOHandler::writeTerm(const vector<mpz_class>& term,
								  const VarNames& names,
								  FILE* out) {
  writeTermProduct(term, names, out);
}

void NewMonosIOHandler::writeRing(const VarNames& names, FILE* out) {
  fputs("(lex-order", out);
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
	putc(' ', out);
	fputs(names.getName(i).c_str(), out);
  }
  fputc(')', out);
}

void NewMonosIOHandler::writeIdealHeader(const VarNames& names,
										 bool defineNewRing,
										 FILE* out) {
  fputs("(monomial-ideal-with-order\n ", out);
  writeRing(names, out);
}

void NewMonosIOHandler::writeTermOfIdeal(const Term& term,
										 const TermTranslator* translator,
										 bool isFirst,
										 FILE* out) {
  fputs("\n ", out);
  IOHandler::writeTermProduct(term, translator, out);
}

void NewMonosIOHandler::writeTermOfIdeal(const vector<mpz_class>& term,
										 const VarNames& names,
										 bool isFirst,
										 FILE* out) {
  fputs("\n ", out);
  IOHandler::writeTermProduct(term, names, out);
}

void NewMonosIOHandler::writeIdealFooter(const VarNames& names,
										 bool wroteAnyGenerators,
										 FILE* out) {
  fputs("\n)\n", out);
}

void NewMonosIOHandler::readRingNoLeftParen(Scanner& in, VarNames& names) {
  in.expect("lex-order");
  while (!in.match(')'))
	names.addVarSyntaxCheckUnique(in, in.readIdentifier());
}

void NewMonosIOHandler::readIdealNoLeftParen(Scanner& in,
											 BigTermConsumer& consumer) {
  in.expect("monomial-ideal-with-order");

  VarNames names;
  in.expect('(');
  readRingNoLeftParen(in, names);
  consumer.consumeRing(names);

  consumer.beginConsuming();
  vector<mpz_class> term(names.getVarCount());

  while (!in.match(')')) {
	readTerm(in, names, term);
	consumer.consume(term);
  }

  consumer.doneConsuming();
}

void NewMonosIOHandler::readIdeal(Scanner& in, BigTermConsumer& consumer) {
  in.expect('(');
  readIdealNoLeftParen(in, consumer);
}

void NewMonosIOHandler::readIdeals(Scanner& in, BigTermConsumer& consumer) {
  in.expect('(');
  if (in.peek('l') || in.peek('L')) {
	VarNames names;
	readRingNoLeftParen(in, names);
	consumer.consumeRing(names);
	return;
  }

  do {
	readIdealNoLeftParen(in, consumer);
  } while (in.match('('));
}

void NewMonosIOHandler::readPolynomial
(Scanner& in, CoefBigTermConsumer& consumer) {
  ASSERT(false);
  reportInternalError("Called NewMonosIOHandler::readPolynomial.");
}
