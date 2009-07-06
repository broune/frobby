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

#include "IOHandlerCommon.h"

class VarNames;
class Scanner;
class BigIdeal;
class CoefTermConsumer;
class BigTermConsumer;

class Macaulay2IOHandler : public IOHandlerCommon {
public:
  Macaulay2IOHandler();

  virtual void writeTerm(const vector<mpz_class>& term,
						 const VarNames& names,
						 FILE* out);

  static const char* staticGetName();

 private:
  static string getRingName(const VarNames& names);

  virtual void readRing(Scanner& in, VarNames& names);
  virtual bool peekRing(Scanner& in);
  virtual void writeRing(const VarNames& names, FILE* out);

  virtual void readBareIdeal(Scanner& in, const VarNames& names,
							 BigTermConsumer& consumer);
  virtual void readBarePolynomial
	(Scanner& in, const VarNames& names, CoefBigTermConsumer& consumer);

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
};

#endif
