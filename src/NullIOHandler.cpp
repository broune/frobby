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
#include "CoefTermConsumer.h"
#include "TermConsumer.h"

TermConsumer* NullIOHandler::createIdealWriter(TermTranslator* translator,
											   FILE* file) {
  return new NullTermConsumer();
}

void NullIOHandler::writeIdealHeader(const VarNames& names, FILE* out) {
}

void NullIOHandler::writeTermOfIdeal(const Term& term,
									 const TermTranslator* translator,
									 bool isFirst,
									 FILE* out) {
}

void NullIOHandler::writeTermOfIdeal(const vector<mpz_class> term,
									 const VarNames& names,
									 bool isFirst,
									 FILE* out) {
}

void NullIOHandler::writeIdealFooter(FILE* out) {
}

CoefTermConsumer* NullIOHandler::createCoefTermWriter
(FILE* file, const TermTranslator* translator) {
  return new NullCoefTermConsumer();
}

void NullIOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  ideal.clearAndSetNames(VarNames());
}

void NullIOHandler::readIrreducibleDecomposition(Scanner& scanner,
												 BigIdeal& decom) {
}

bool NullIOHandler::hasMoreInput(Scanner& scanner) const {
  return false;
}

const char* NullIOHandler::getFormatName() const {
  return "null";
}
