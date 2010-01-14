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
#include "stdinc.h"
#include "IdealWriter.h"

#include "BigIdeal.h"
#include "Term.h"

namespace IO {
  IdealWriter::IdealWriter(FILE* out):
    _out(out),
    _firstIdeal(true),
    _firstGenerator(true) {
  }

  void IdealWriter::consumeRing(const VarNames& names) {
    if (_names != names) {
      _names = names;
      _firstIdeal = true;
    }
  }

  void IdealWriter::beginConsumingList() {
    _firstIdeal = true;
  }

  void IdealWriter::beginConsuming() {
    _firstGenerator = true;
    doWriteHeader(_firstIdeal);
  }

  void IdealWriter::consume(const Term& term, const TermTranslator& translator) {
    ASSERT(term.getVarCount() == _names.getVarCount());
    bool firstGenerator = _firstGenerator; // To get tail recursion.
    _firstGenerator = false;
    doWriteTerm(term, translator, firstGenerator);
  }

  void IdealWriter::consume(const vector<mpz_class>& term) {
    ASSERT(term.size() == _names.getVarCount());
    bool firstGenerator = _firstGenerator; // To get tail recursion.
    _firstGenerator = false;
    doWriteTerm(term, firstGenerator);
  }

  void IdealWriter::doneConsuming() {
    _firstIdeal = false;
    doWriteFooter(_firstGenerator);
  }

  void IdealWriter::doneConsumingList() {
    if (_firstIdeal)
      doWriteEmptyList();
  }

  void IdealWriter::consume(const BigIdeal& ideal) {
    consumeRing(ideal.getNames());
    _firstGenerator = true;
    doWriteHeader(_firstIdeal, ideal.getGeneratorCount());

    for (size_t term = 0; term < ideal.getGeneratorCount(); ++term)
      consume(ideal.getTerm(term));
    doneConsuming();
  }

  void IdealWriter::doWriteHeader(bool firstIdeal, size_t generatorCount) {
    doWriteHeader(firstIdeal);
  }
}
