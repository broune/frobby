/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#ifndef IDEAL_WRITER_GUARD
#define IDEAL_WRITER_GUARD

#include "VarNames.h"
#include "BigTermConsumer.h"

class TermTranslator;
class Term;

namespace IO {
  class IdealWriter : public BigTermConsumer {
  public:
    IdealWriter(FILE* out);

    virtual void consumeRing(const VarNames& names);

    virtual void beginConsumingList();
    virtual void beginConsuming();
    virtual void consume(const Term& term, const TermTranslator& translator);
    virtual void consume(const vector<mpz_class>& term);
    virtual void doneConsuming();
    virtual void doneConsumingList();

    virtual void consume(const BigIdeal& ideal);

    FILE* getFile() {return _out;}
    const VarNames& getNames() {return _names;}

  private:
    virtual void doWriteHeader(bool firstIdeal, size_t generatorCount);
    virtual void doWriteHeader(bool firstIdeal) = 0;
    virtual void doWriteTerm(const Term& term,
                             const TermTranslator& translator,
                             bool firstGenerator) = 0;
    virtual void doWriteTerm(const vector<mpz_class>& term,
                             bool firstGenerator) = 0;
    virtual void doWriteFooter(bool wasZeroIdeal) = 0;
    virtual void doWriteEmptyList() = 0;

    FILE* const _out;
    bool _firstIdeal;
    bool _firstGenerator;
    VarNames _names;
  };
}

#endif
