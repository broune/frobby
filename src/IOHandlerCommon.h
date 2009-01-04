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
#ifndef IO_HANDLER_COMMON_GUARD
#define IO_HANDLER_COMMON_GUARD

#include "IOHandler.h"

// IOHandlerCommon is an abstract base class that implements functionality
// that is useful for most but not all derivates of IOHandler.
class IOHandlerCommon : public IOHandler {
 public:
  // TODO: make protected.
  IOHandlerCommon(const char* formatName, const char* formatDescription);

  virtual void readIdeal(Scanner& in, BigTermConsumer& consumer);
  virtual void readIdeals(Scanner& in, BigTermConsumer& consumer);
  virtual void readPolynomial(Scanner& in, CoefBigTermConsumer& consumer);

 protected:
  virtual void readRing(Scanner& in, VarNames& names) = 0;
  virtual bool peekRing(Scanner& in) = 0;

  virtual void readBareIdeal
	(Scanner& in, const VarNames& names, BigTermConsumer& consumer) = 0;
  virtual void readBarePolynomial
	(Scanner& in, const VarNames& names, CoefBigTermConsumer& consumer) = 0;
};

#endif
