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
#include "stdinc.h"
#include "IOHandlerCommon.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "BigTermRecorder.h"

IOHandlerCommon::IOHandlerCommon(const char* formatName,
								 const char* formatDescription):
  IOHandler(formatName, formatDescription, false) {
}

void IOHandlerCommon::readIdeal(Scanner& in, BigTermConsumer& consumer) {
  VarNames names;
  readRing(in, names);
  readBareIdeal(in, names, consumer);
}

void IOHandlerCommon::readIdeals(Scanner& in, BigTermConsumer& consumer) {
  VarNames names;
  readRing(in, names);
  if (!hasMoreInput(in)) {
	consumer.consumeRing(names);
	return;
  }
  readBareIdeal(in, names, consumer);

  while (hasMoreInput(in)) {
	if (peekRing(in))
	  readRing(in, names);
	readBareIdeal(in, names, consumer);
  }
}

void IOHandlerCommon::readPolynomial
(Scanner& in, CoefBigTermConsumer& consumer) {
  VarNames names;
  readRing(in, names);
  readBarePolynomial(in, names, consumer);
}
