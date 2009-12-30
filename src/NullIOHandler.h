
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

#include "IOHandlerImpl.h"

class VarNames;
class Scanner;
class BigIdeal;
class BigPolynomial;

/** This IO format reads the empty ideal without actually consulting
 the input, and does nothing when asked to writed output.
*/
class NullIOHandler : public IOHandlerImpl {
public:
  NullIOHandler();

  static const char* staticGetName();

 private:
  virtual void doReadIdeal(Scanner& in, BigTermConsumer& consumer);
  virtual void doReadIdeals(Scanner& in, BigTermConsumer& consumer);
  virtual void doReadPolynomial(Scanner& in, CoefBigTermConsumer& consumer);
  virtual void doReadSatBinomIdeal(Scanner& in, SatBinomConsumer& consumer);

  virtual void doReadIdeal(Scanner& scanner, BigIdeal& ideal);
  virtual void doWriteTerm(const vector<mpz_class>& term,
						 const VarNames& names,
						 FILE* out);

  virtual auto_ptr<BigTermConsumer> doCreateIdealWriter
	(TermTranslator* translator, FILE* out);

  virtual auto_ptr<CoefBigTermConsumer> doCreatePolynomialWriter
	(const TermTranslator* translator, FILE* out);

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

  virtual bool doHasMoreInput(Scanner& scanner) const;
};

#endif
