/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)
   Copyright (C) 2009 University of Aarhus

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
#ifndef POLY_WRITER_GUARD
#define POLY_WRITER_GUARD

#include "CoefBigTermConsumer.h"
#include "VarNames.h"

namespace IO {
  class PolyWriter : public CoefBigTermConsumer {
  public:
    PolyWriter(FILE* out);

    virtual void consumeRing(const VarNames& names);

    virtual void beginConsuming();
    virtual void consume(const mpz_class& coef,
                         const Term& term,
                         const TermTranslator& translator);
    virtual void consume(const mpz_class& coef, const vector<mpz_class>& term);
    virtual void doneConsuming();

    virtual void consume(const BigPolynomial& poly);

    FILE* getFile() {return _out;}
    const VarNames& getNames() const {return _names;}

  private:
    virtual void doWriteHeader(size_t generatorCount);
    virtual void doWriteHeader() = 0;
    virtual void doWriteTerm(const mpz_class& coef,
                             const Term& term,
                             const TermTranslator& translator,
                             bool firstGenerator) = 0;
    virtual void doWriteTerm(const mpz_class& coef,
                             const vector<mpz_class>& term,
                             bool firstGenerator) = 0;
    virtual void doWriteFooter(bool wasZero) = 0;

    FILE* const _out;
    bool _firstTerm;
    VarNames _names;
  };
}

#endif
