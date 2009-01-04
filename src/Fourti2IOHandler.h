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
#ifndef FOURTI2_IO_HANDLER_GUARD
#define FOURTI2_IO_HANDLER_GUARD

#include "IOHandler.h"

class Scanner;
class VarNames;
class BigIdeal;
class BigTermConsumer;
class BigCoefTermConsumer;

class Fourti2IOHandler : public IOHandler {
public:
  Fourti2IOHandler();

  virtual void readIdeal(Scanner& in, BigTermConsumer& consumer);
  virtual void readIdeals(Scanner& in, BigTermConsumer& consumer);

  virtual void readTerm(Scanner& in, const VarNames& names,
						vector<mpz_class>& term);
  virtual void readPolynomial(Scanner& in, CoefBigTermConsumer& consumer);

  virtual void writeTerm(const vector<mpz_class>& term,
						 const VarNames& names,
						 FILE* out);

  auto_ptr<BigTermConsumer> createIdealWriter(FILE* out);
  auto_ptr<CoefBigTermConsumer> createPolynomialWriter(FILE* out);

  static const char* staticGetName();

 protected:
  void readIdeal(Scanner& in, BigTermConsumer& consumer,
				 size_t generatorCount, size_t varCount);
  void readRing(Scanner& in, VarNames& names);
  void readRing(Scanner& in, VarNames& names, size_t varCount);
  void writeRingWithoutHeader(const VarNames& names, FILE* out);

  virtual void writeRing(const VarNames& names, FILE* out);

  virtual void writePolynomialHeader(const VarNames& names,
									 size_t termCount,
									 FILE* out);
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
								size_t generatorCount,
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

  // These are not supported.
  virtual void writePolynomialHeader(const VarNames& names, FILE* out);
  virtual void writeIdealHeader(const VarNames& names, bool
								defineNewRing,
								FILE* out);
};

#endif
