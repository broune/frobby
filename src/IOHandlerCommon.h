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
#ifndef IO_HANDLER_COMMON_GUARD
#define IO_HANDLER_COMMON_GUARD

#include "IOHandlerImpl.h"

class Scanner;
class VarNames;
class BigTermConsumer;
class CoefBigTermConsumer;
class InputConsumer;

namespace IO {
  /** This class contains further functionality that makes it more
   convenient to derive from than IOHandlerImpl in most but not all
   cases. */
  class IOHandlerCommon : public IOHandlerImpl {
  public:
    IOHandlerCommon(const char* formatName,
                    const char* formatDescription);

    /** Reads a ring, i.e. the names of the variables. */
    void readRing(Scanner& in, VarNames& names);

    /** Returns true if what follows in the input is a ring, assuming
     there are no syntax errors. */
    bool peekRing(Scanner& in);

    /** Reads an ideal without a ring preceding it. */
    void readBareIdeal(Scanner& in, InputConsumer& consumer);

    /** Reads a polynomial without a ring preceding it. */
    void readBarePolynomial(Scanner& in,
                            const VarNames& names,
                            CoefBigTermConsumer& consumer);

  private:
    // The following methods are implemented in terms of the new methods.
    virtual void doReadIdeal(Scanner& in, InputConsumer& consumer);
    virtual void doReadIdeals(Scanner& in, InputConsumer& consumer);
    virtual void doReadPolynomial(Scanner& in, CoefBigTermConsumer& consumer);

    // Override these methods except for those that do input
    // of un-supported type.
    virtual void doReadRing(Scanner& in, VarNames& names) = 0;
    virtual bool doPeekRing(Scanner& in) = 0;
    virtual void doReadBareIdeal(Scanner& in, InputConsumer& consumer);
    virtual void doReadBarePolynomial(Scanner& in,
                                      const VarNames& names,
                                      CoefBigTermConsumer& consumer);
  };
}

#endif
