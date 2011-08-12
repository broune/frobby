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
#ifndef MONOS_IO_HANDLER_GUARD
#define MONOS_IO_HANDLER_GUARD

#include "IOHandlerCommon.h"
#include <vector>

class Scanner;
class VarNames;
class BigTermConsumer;

namespace IO {
  class MonosIOHandler : public IOHandlerCommon {
  public:
    MonosIOHandler();

    static const char* staticGetName();

  private:
    virtual BigTermConsumer* doCreateIdealWriter(FILE* out);

    virtual void doWriteTerm(const vector<mpz_class>& term,
                             const VarNames& names,
                             FILE* out);
    virtual void doReadTerm(Scanner& in, InputConsumer& consumer);
    virtual void doReadRing(Scanner& in, VarNames& names);
    virtual bool doPeekRing(Scanner& in);
    virtual void doReadBareIdeal(Scanner& in, InputConsumer& consumer);
  };
}

#endif
