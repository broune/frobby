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
#ifndef BIG_TERM_RECORDER_GUARD
#define BIG_TERM_RECORDER_GUARD

#include "BigTermConsumer.h"

class BigIdeal;

class BigTermRecorder : public BigTermConsumer {
public:
  // DecomRecorder does not take over ownership of recordInto.
  BigTermRecorder(BigIdeal* recordInto);
  virtual ~BigTermRecorder();

  virtual void consume(const Term& term, TermTranslator* translator);
  virtual void consume(mpz_ptr* term);

private:
  BigIdeal* _recordInto;
};

#endif
