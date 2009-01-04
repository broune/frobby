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
#include "BigTermConsumer.h"
#include "DataType.h"

#include <cstdio>

MonosIOHandler::MonosIOHandler():
  IOHandlerCommon(staticGetName(), "Older format used by the program Monos.") {
  registerInput(DataType::getMonomialIdealType());
  registerInput(DataType::getMonomialIdealListType());
  registerOutput(DataType::getMonomialIdealType());
  registerOutput(DataType::getMonomialIdealListType());
}

const char* MonosIOHandler::staticGetName() {
  return "monos";
}

void MonosIOHandler::writeRing(const VarNames& names, FILE* out) {
  fputs("vars ", out);
  const char* pre = "";
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
	fputs(pre, out);
	fputs(names.getName(i).c_str(), out);
	pre = ", ";
  }
  fputs(";\n", out);
}

void MonosIOHandler::writeTerm(const vector<mpz_class>& term,
							   const VarNames& names,
							   FILE* out) {
  writeTermProduct(term, names, out);
}

void MonosIOHandler::writeIdealHeader(const VarNames& names,
									  bool defineNewRing,
									  FILE* out) {
  writeRing(names, out);
  fputc('[', out);
}

void MonosIOHandler::writeTermOfIdeal(const Term& term,
									  const TermTranslator* translator,
									  bool isFirst,
									  FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);
  IOHandler::writeTermProduct(term, translator, out);
}

void MonosIOHandler::writeTermOfIdeal(const vector<mpz_class>& term,
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

void MonosIOHandler::readRing(Scanner& in, VarNames& names) {
  names.clear();
  in.expect("vars");
  if (!in.match(';')) {
	do {
	  names.addVarSyntaxCheckUnique(in, in.readIdentifier());
	} while (in.match(','));
	in.expect(';');
  }  
}

bool MonosIOHandler::peekRing(Scanner& in) {
  return in.peek('v');
}

void MonosIOHandler::readBareIdeal(Scanner& in,
								   const VarNames& names,
								   BigTermConsumer& consumer) {
  consumer.beginConsuming(names);
  vector<mpz_class> term(names.getVarCount());

  in.expect('[');
  if (!in.match(']')) {
    do {
      readTerm(in, names, term);
	  consumer.consume(term);
    } while (in.match(','));
	if (!in.match(']')) {
	  if (in.peekIdentifier())
		in.expect('*');
	  else
		in.expect(']');
	}
  }
  in.expect(';');

  consumer.doneConsuming();
}

void MonosIOHandler::readBarePolynomial
(Scanner& in, const VarNames& names, CoefBigTermConsumer& consumer) {
  ASSERT(false);
  reportInternalError("Called MonosIOHandler::readBarePolynomial.");
}
