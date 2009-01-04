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
#ifndef NEW_MONOS_HANDLER_IO
#define NEW_MONOS_HANDLER_IO

#include "IOHandler.h"

class Scanner;
class VarNames;
class BigIdeal;

class NewMonosIOHandler : public IOHandler {
public:
  NewMonosIOHandler();

  virtual void readIdeal(Scanner& in, BigTermConsumer& consumer);
  virtual void readIdeals(Scanner& in, BigTermConsumer& consumer);

  virtual void writeTerm(const vector<mpz_class>& term,
						 const VarNames& names,
						 FILE* out);

  static const char* staticGetName();

 private:
  void readRingNoLeftParen(Scanner& in, VarNames& names);
  void readIdealNoLeftParen(Scanner& in, BigTermConsumer& consumer);
  virtual void writeRing(const VarNames& names, FILE* out);

  virtual void writeIdealHeader(const VarNames& names, 
								bool defineNewRing,
								FILE* out);
  virtual void writeTermOfIdeal(const Term& term,
								const TermTranslator* translator,
								bool isFirst,
								FILE* out);
  virtual void writeTermOfIdeal(const vector<mpz_class>& term,
								const VarNames& names,
								bool isFirst,
								FILE* out);
  virtual void writeIdealFooter(const VarNames& names,
								bool wroteAnyGenerators,
								FILE* out);

  // Not supported.
  virtual void readPolynomial(Scanner& in, CoefBigTermConsumer& consumer);
};

#endif
