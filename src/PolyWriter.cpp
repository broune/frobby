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
#include "stdinc.h"
#include "PolyWriter.h"

#include "BigPolynomial.h"
#include "Term.h"

namespace IO {
  PolyWriter::PolyWriter(FILE* out):
    _out(out),
    _firstTerm(true) {
  }

  void PolyWriter::consumeRing(const VarNames& names) {
    _names = names;
  }

  void PolyWriter::beginConsuming() {
    _firstTerm = true;
    doWriteHeader();
  }

  void PolyWriter::consume(const mpz_class& coef,
                               const Term& term,
                               const TermTranslator& translator) {
    ASSERT(term.getVarCount() == _names.getVarCount());
    bool firstTerm = _firstTerm; // To get tail recursion.
    _firstTerm = false;
    doWriteTerm(coef, term, translator, firstTerm);
  }

  void PolyWriter::consume(const mpz_class& coef, const vector<mpz_class>& term) {
    ASSERT(term.size() == _names.getVarCount());
    bool firstTerm = _firstTerm; // To get tail recursion.
    _firstTerm = false;
    doWriteTerm(coef, term, firstTerm);
  }

  void PolyWriter::doneConsuming() {
    doWriteFooter(_firstTerm);
  }

  void PolyWriter::consume(const BigPolynomial& poly) {
    consumeRing(poly.getNames());
    _firstTerm = true;
    doWriteHeader(poly.getTermCount());
    for (size_t index = 0; index < poly.getTermCount(); ++index)
      consume(poly.getCoef(index), poly.getTerm(index));
    doneConsuming();
  }

  void PolyWriter::doWriteHeader(size_t generatorCount) {
    doWriteHeader();
  }
}
