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
#ifndef NULL_IO_HANDLER_GUARD
#define NULL_IO_HANDLER_GUARD

#include "IOHandler.h"

class VarNames;
class Scanner;
class BigIdeal;
class BigPolynomial;

// Reads the empty ideal without actually consulting the input, and
// never writes anything to the output.
class NullIOHandler : public IOHandler {
public:
  NullIOHandler();

  virtual void readIdeal(Scanner& in, BigTermConsumer& consumer);
  virtual void readIdeals(Scanner& in, BigTermConsumer& consumer);
  virtual void readPolynomial(Scanner& in, CoefBigTermConsumer& consumer);

  virtual void readIdeal(Scanner& scanner, BigIdeal& ideal);
  virtual void writeTerm(const vector<mpz_class>& term,
						 const VarNames& names,
						 FILE* out);

  virtual auto_ptr<BigTermConsumer> createIdealWriter
	(TermTranslator* translator, FILE* out);

  virtual auto_ptr<CoefBigTermConsumer> createPolynomialWriter
	(const TermTranslator* translator, FILE* out);

  static const char* staticGetName();

 protected:
  virtual void writeRing(const VarNames& names, FILE* out);

  virtual void writePolynomialHeader(const VarNames& names, FILE* out);
  virtual void writeTermOfPolynomial(const mpz_class& coef,
									 const Term& term,
									 const TermTranslator* translator,
									 bool isFirst,
									 FILE* out);
  virtual void writeTermOfPolynomial(const mpz_class& coef,
									 const vector<mpz_class>& term,
									 const VarNames& names,
									 bool isFirst,
									 FILE* out);
  virtual void writePolynomialFooter(const VarNames& names,
									 bool wroteAnyGenerators,
									 FILE* out);

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

  virtual bool hasMoreInput(Scanner& scanner) const;
};

#endif
