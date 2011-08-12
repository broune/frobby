
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
#ifndef NULL_IO_HANDLER_GUARD
#define NULL_IO_HANDLER_GUARD

#include "IOHandlerImpl.h"

class VarNames;
class Scanner;
class BigIdeal;
class BigPolynomial;
class SatBinomConsumer;
class InputConsumer;

namespace IO {
  /** This class ignores the input and discards all output. */
  class NullIOHandler : public IOHandlerImpl {
  public:
    NullIOHandler();

    static const char* staticGetName();

  private:
    virtual BigTermConsumer* doCreateIdealWriter(FILE* out);
    virtual CoefBigTermConsumer* doCreatePolynomialWriter(FILE* out);

    virtual void doWriteTerm(const vector<mpz_class>& term,
                             const VarNames& names,
                             FILE* out);
    virtual void doReadIdeal(Scanner& in, InputConsumer& consumer);
    virtual void doReadIdeals(Scanner& in, InputConsumer& consumer);
    virtual void doReadTerm(Scanner& in, InputConsumer& consumer);
    virtual void doReadPolynomial(Scanner& in, CoefBigTermConsumer& consumer);
    virtual void doReadSatBinomIdeal(Scanner& in, SatBinomConsumer& consumer);
    virtual bool doHasMoreInput(Scanner& in) const;
  };
}

#endif
