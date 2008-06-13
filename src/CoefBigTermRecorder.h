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
#ifndef COEF_BIG_TERM_RECORDER_GUARD
#define COEF_BIG_TERM_RECORDER_GUARD

#include "CoefBigTermConsumer.h"

class TermTranslator;
class BigPolynomial;

class CoefBigTermRecorder : public CoefBigTermConsumer {
 public:
  CoefBigTermRecorder(BigPolynomial* recordInto);
  virtual ~CoefBigTermRecorder();

  virtual void consume(const mpz_class& coef,
					   const Term& term,
					   TermTranslator* translator);

  virtual void consume(const mpz_class& coef, mpz_ptr* term);

  virtual void consume(const mpz_class& coef,
					   const vector<mpz_class>& term);

 private:
  BigPolynomial* _recordInto;
};

#endif
