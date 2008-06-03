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
#ifndef MACAULAY_2_IO_HANDLER_GUARD
#define MACAULAY_2_IO_HANDLER_GUARD

#include "IOHandler.h"

class VarNames;
class Scanner;
class BigIdeal;
class CoefTermConsumer;

class Macaulay2IOHandler : public IOHandler {
public:
  Macaulay2IOHandler();

  virtual void readIdeal(Scanner& scanner, BigIdeal& ideal);

  virtual void writeIdealHeader(const VarNames& names, FILE* out);

  virtual void writeTermOfIdeal(const Term& term,
								const TermTranslator* translator,
								bool isFirst,
								FILE* out);

  virtual void writeTermOfIdeal(const vector<mpz_class> term,
								const VarNames& names,
								bool isFirst,
								FILE* out);

  virtual void writeIdealFooter(const VarNames& names,
								FILE* out);

  virtual CoefTermConsumer* createCoefTermWriter
	(FILE* file, const TermTranslator* translator);

private:
  void readVarsAndClearIdeal(BigIdeal& ideal, Scanner& scanner);
};

#endif
