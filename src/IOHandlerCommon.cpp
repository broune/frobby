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
#include "IOHandlerCommon.h"

#include "VarNames.h"
#include "BigTermConsumer.h"
#include "BigTermRecorder.h"
#include "error.h"
#include "InputConsumer.h"

IO::IOHandlerCommon::IOHandlerCommon(const char* formatName,
                                     const char* formatDescription):
  IOHandlerImpl(formatName, formatDescription) {
}

void IO::IOHandlerCommon::readRing(Scanner& in, VarNames& names) {
  doReadRing(in, names);
}

bool IO::IOHandlerCommon::peekRing(Scanner& in) {
  return doPeekRing(in);
}

void IO::IOHandlerCommon::readBareIdeal(Scanner& in, InputConsumer& consumer) {
  doReadBareIdeal(in, consumer);
}

void IO::IOHandlerCommon::readBarePolynomial(Scanner& in,
                                            const VarNames& names,
                                            CoefBigTermConsumer& consumer) {
  doReadBarePolynomial(in, names, consumer);
}

void IO::IOHandlerCommon::doReadIdeal(Scanner& in, InputConsumer& consumer) {
  VarNames names;
  readRing(in, names);
  consumer.consumeRing(names);
  readBareIdeal(in, consumer);
}

void IO::IOHandlerCommon::doReadIdeals(Scanner& in, InputConsumer& consumer) {
  VarNames names;
  readRing(in, names);
  consumer.consumeRing(names);
  if (!hasMoreInput(in))
    return;
  readBareIdeal(in, consumer);

  while (hasMoreInput(in)) {
    if (peekRing(in)) {
      readRing(in, names);
	  consumer.consumeRing(names);
	}
    readBareIdeal(in, consumer);
  }
}

void IO::IOHandlerCommon::doReadPolynomial(Scanner& in,
                                          CoefBigTermConsumer& consumer) {
  VarNames names;
  readRing(in, names);
  readBarePolynomial(in, names, consumer);
}

void IO::IOHandlerCommon::doReadBarePolynomial(Scanner& in,
                                              const VarNames& names,
                                              CoefBigTermConsumer& consumer) {
  INTERNAL_ERROR_UNIMPLEMENTED();
}

void IO::IOHandlerCommon::doReadBareIdeal
(Scanner& in, InputConsumer& consumer) {
  INTERNAL_ERROR_UNIMPLEMENTED();
}
