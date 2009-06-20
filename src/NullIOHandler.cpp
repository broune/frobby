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
#include "NullIOHandler.h"

#include "BigIdeal.h"
#include "NullCoefTermConsumer.h"
#include "NullTermConsumer.h"
#include "BigPolynomial.h"
#include "VarNames.h"
#include "DataType.h"

NullIOHandler::NullIOHandler():
  IOHandler(staticGetName(), "Ignores input and produces no output.", false) {
  registerInput(DataType::getMonomialIdealType());
  registerInput(DataType::getPolynomialType());
  registerInput(DataType::getMonomialIdealListType());

  registerOutput(DataType::getMonomialIdealType());
  registerOutput(DataType::getPolynomialType());
  registerOutput(DataType::getMonomialIdealListType());
}

const char* NullIOHandler::staticGetName() {
  return "null";
}

void NullIOHandler::readIdeal(Scanner& in, BigTermConsumer& consumer) {
  consumer.consume(BigIdeal());
}

void NullIOHandler::readIdeals(Scanner& in, BigTermConsumer& consumer) {
}

void NullIOHandler::writeRing(const VarNames& names, FILE* out) {
}

void NullIOHandler::writeTerm(const vector<mpz_class>& term,
							  const VarNames& names,
							  FILE* out) {
}

auto_ptr<BigTermConsumer> NullIOHandler::createIdealWriter
(TermTranslator* translator, FILE* file) {
  return auto_ptr<BigTermConsumer>(new NullTermConsumer());
}

void NullIOHandler::writePolynomialHeader(const VarNames& names, FILE* out) {
}

void NullIOHandler::writeTermOfPolynomial(const mpz_class& coef,
										  const Term& term,
										  const TermTranslator* translator,
										  bool isFirst,
										  FILE* out) {
}
void NullIOHandler::writeTermOfPolynomial(const mpz_class& coef,
										  const vector<mpz_class>& term,
										  const VarNames& names,
										  bool isFirst,
										  FILE* out) {
}

void NullIOHandler::writePolynomialFooter(const VarNames& names,
										  bool wroteAnyGenerators,
										  FILE* out) {
}

void NullIOHandler::writeIdealHeader(const VarNames& names,
									 bool defineNewRing,
									 FILE* out) {
}

void NullIOHandler::writeTermOfIdeal(const Term& term,
									 const TermTranslator* translator,
									 bool isFirst,
									 FILE* out) {
}

void NullIOHandler::writeTermOfIdeal(const vector<mpz_class>& term,
									 const VarNames& names,
									 bool isFirst,
									 FILE* out) {
}

void NullIOHandler::writeIdealFooter(const VarNames& names,
									 bool wroteAnyGenerators,
									 FILE* out) {
}

auto_ptr<CoefBigTermConsumer> NullIOHandler::createPolynomialWriter
(const TermTranslator* translator, FILE* out) {
  return auto_ptr<CoefBigTermConsumer>(new NullCoefTermConsumer());
}

void NullIOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  ideal.clearAndSetNames(VarNames());
}

bool NullIOHandler::hasMoreInput(Scanner& scanner) const {
  return false;
}

void NullIOHandler::readPolynomial
(Scanner& in, CoefBigTermConsumer& consumer) {
}
