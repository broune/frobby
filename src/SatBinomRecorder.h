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
#ifndef SAT_BINOM_RECORDER_GUARD
#define SAT_BINOM_RECORDER_GUARD

#include "SatBinomConsumer.h"
#include "VarNames.h"

class SatBinomIdeal;

/// SatBinomRecorder records all consumed saturated binomials into a
/// passed-in ideal.
class SatBinomRecorder : public SatBinomConsumer {
public:
  SatBinomRecorder(SatBinomIdeal& ideal);

  virtual void consumeRing(const VarNames& names);
  virtual void beginConsuming();
  virtual void consume(const vector<mpz_class>& term);
  virtual void doneConsuming();

private:
  SatBinomIdeal& _ideal;
};

#endif
