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
#ifndef IO_FACADE_GUARD
#define IO_FACADE_GUARD

#include "Facade.h"
#include <vector>

class BigIdeal;
class Scanner;
class IOHandler;

class IOFacade : private Facade {
 public:
  IOFacade(bool printActions);

  bool isValidMonomialIdealFormat(const string& format);
  void readIdeal(Scanner& in, BigIdeal& ideal);
  void readIdeals(Scanner& in,
				  vector<BigIdeal*>& ideal); // inserts the read ideals
  void writeIdeal(BigIdeal& ideal, IOHandler* handler, FILE* out);

  void readFrobeniusInstance(Scanner& in, vector<mpz_class>& instance);
  void readFrobeniusInstanceWithGrobnerBasis
    (Scanner& in, BigIdeal& ideal, vector<mpz_class>& instance);
  void writeFrobeniusInstance(FILE* out, vector<mpz_class>& instance);

  bool readAlexanderDualInstance
	(Scanner& in, BigIdeal& ideal, vector<mpz_class>& term);

  bool isValidLatticeFormat(const string& format);
  void readLattice(Scanner& in, BigIdeal& ideal);
  void writeLattice(FILE* out, const BigIdeal& ideal, const string& format);
};

#endif
